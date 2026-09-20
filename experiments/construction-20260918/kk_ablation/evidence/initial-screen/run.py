"""Run a frozen 2x2 KittyKnight guidance/rematching diagnostic on isolated GRID hosts."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 'tools'))
from benchmark_cgar import FIELDS, header
from cpu_resources import cpu_resources

QUEUES = ','.join('debian.q@research%s.grid.gsb' % n for n in [31, 38, 44, 46, 50, 52, 57])


def now():
    return datetime.datetime.now(datetime.timezone.utc).isoformat()


def digest(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for block in iter(lambda: stream.read(4 * 1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()


def write(path, data):
    Path(path).write_text(json.dumps(data, indent=2) + '\n')


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--build', type=Path)
    p.add_argument('--steps', type=int, choices=[200, 5000], default=5000)
    p.add_argument('--execute', action='store_true')
    args = p.parse_args(); out = args.output.resolve()
    if not args.execute:
        assert args.build, 'a verified frozen build is required'
        assert out.parent == ROOT / 'runs'
        frozen = args.build.resolve()
        build = json.loads((frozen / 'build.json').read_text())
        fixtures = json.loads((frozen / 'fixture-results.json').read_text())
        assert fixtures['native_and_enabled_identical'] and fixtures['disabled_preserves_unopened_and_started']
        assert digest(frozen / 'lifelong') == build['binary_sha256']
        assert digest(frozen / 'fixture-results.json') == build['fixture_results_sha256']
        out.mkdir(parents=True, exist_ok=False)
        (out / 'support').mkdir(); shutil.copy2(Path(__file__), out / 'support/run.py')
        shutil.copy2(frozen / 'lifelong', out / 'lifelong')
        (out / 'configs').mkdir()
        original = json.loads((frozen / 'source/wppl_planner/configs/warehouse_large.json').read_text())
        for guidance in ['supplied', 'uniform']:
            config = json.loads(json.dumps(original))
            if guidance == 'uniform': config['map_weights_path'] = ''
            write(out / 'configs' / (guidance + '.json'), config)
        cases = [{'name': '%s_%s-r0' % (guide, 'rematch' if rematch else 'keep'),
                  'guidance': guide, 'rematch': rematch, 'repeat': 0}
                 for guide in ['supplied', 'uniform'] for rematch in [1, 0]]
        if args.steps == 5000:
            cases.append({'name': 'supplied_rematch-r1', 'guidance': 'supplied', 'rematch': 1, 'repeat': 1})
        input_path = (ROOT / 'mr24/warehouse.domain/WAREHOUSE.json').resolve()
        data = json.loads(input_path.read_text())
        inputs = [input_path] + [(input_path.parent / data[k]).resolve() for k in ['mapFile', 'agentFile', 'taskFile']]
        spec = {'prepared_utc': now(), 'cases': cases, 'steps': args.steps,
            'input': str(input_path), 'input_hashes': {str(x): digest(x) for x in inputs},
            'build': str(frozen), 'build_manifest_sha256': digest(frozen / 'build.json'),
            'binary_sha256': build['binary_sha256'], 'source_hashes': build['sources'],
            'source': str(frozen / 'source'), 'runner_sha256': digest(out / 'support/run.py'),
            'config_hashes': {g: digest(out / 'configs' / (g + '.json')) for g in ['supplied', 'uniform']},
            'time_limit_ms': 1000, 'preprocess_limit_ms': 1800000,
            'physical_cores_per_case': 16, 'logical_cpus_per_case': 32, 'reserved_memory_gib_per_case': 128,
            'expected_cpu_model': 'AMD EPYC 9354 32-Core Processor',
            'source_commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=str(ROOT), text=True).strip(),
            'scope': 'Clock-limited competitor ablation; diagnostic only, not a CGAR score or resource-compliance claim.'}
        write(out / 'spec.json', spec)
        command = ['/usr/bin/python3', str(out / 'support/run.py'), '--execute', '--output', str(out)]
        script = out / 'job.sh'
        script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', QUEUES,
            '-t', '1-%d' % len(cases), '-tc', str(len(cases)), '-pe', 'threaded', '32', '-binding', 'linear:16',
            '-l', 'exclusive=true,h_rt=03:00:00,h_vmem=4G', '-m', 'n', '-N', 'kk_diag_matrix',
            '-j', 'y', '-o', str(out), '-S', '/bin/bash', str(script)]
        proc = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(out / 'submission.json', {'command': submit, 'returncode': proc.returncode, 'response': proc.stdout})
        print(proc.stdout, end=''); return proc.returncode
    spec = json.loads((out / 'spec.json').read_text())
    case = spec['cases'][int(os.environ['SGE_TASK_ID']) - 1]
    work = out / case['name']; work.mkdir()
    source = Path(spec['source'])
    assert digest(Path(__file__)) == spec['runner_sha256']
    assert digest(Path(spec['build']) / 'build.json') == spec['build_manifest_sha256']
    assert digest(out / 'lifelong') == spec['binary_sha256']
    for path, expected in spec['input_hashes'].items(): assert digest(path) == expected, path
    for path, expected in spec['source_hashes'].items(): assert digest(source / path) == expected, path
    config = out / 'configs' / (case['guidance'] + '.json')
    assert digest(config) == spec['config_hashes'][case['guidance']]
    shutil.copytree(source, work / 'cwd')
    allocation = cpu_resources()
    assert len(allocation['logical_cpu_affinity']) == 32 and allocation['physical_cores_visible'] == 16
    assert all(len(cpus) == 2 for cpus in allocation['logical_cpus_by_physical_core'])
    assert allocation['effective_cpu_quota'] is None and allocation['cpu_model'] == spec['expected_cpu_model']
    started = now()
    write(work / 'allocation.json', {'started_utc': started, 'resources': allocation,
                                    'job_id': os.environ['JOB_ID'], 'task_id': os.environ['SGE_TASK_ID']})
    env = {k: v for k, v in os.environ.items() if not k.startswith(('CGAR_', 'LORR_KK_'))
           and k not in ['CONFIG_PATH', 'MAP_WEIGHT_PATH', 'LNS_NUM_THREADS', 'SHARE_HEURISTICS']}
    env.update(OMP_NUM_THREADS='32', OMP_DYNAMIC='FALSE', CONFIG_PATH=str(config),
               LORR_KK_REMATCH_UNOPENED=str(case['rematch']))
    command = ['/usr/bin/python3', str(ROOT / 'tools/run_with_usage.py'), '--output', str(work / 'usage.json'), '--',
        str(out / 'lifelong'), '-i', spec['input'], '-o', str(work / 'result.json'),
        '-s', str(spec['steps']), '-t', str(spec['time_limit_ms']), '-p', str(spec['preprocess_limit_ms'])]
    write(work / 'metadata.json', {'case': case, 'started_utc': started, 'command': command,
        'binary_sha256': spec['binary_sha256'], 'config_sha256': digest(config),
        'source_hashes': spec['source_hashes'], 'build_manifest_sha256': spec['build_manifest_sha256'],
        'environment': {k: env[k] for k in ['OMP_NUM_THREADS', 'OMP_DYNAMIC', 'CONFIG_PATH', 'LORR_KK_REMATCH_UNOPENED']}})
    wall = time.monotonic()
    with (work / 'native.log').open('w') as log:
        proc = subprocess.run(command, cwd=str(work / 'cwd'), env=env, stdout=log, stderr=subprocess.STDOUT)
    data = header(work / 'result.json') if (work / 'result.json').exists() else dict.fromkeys(FIELDS)
    valid = proc.returncode == 0 and data['makespan'] == spec['steps'] and all(data[k] == 0 for k in FIELDS[3:])
    usage = json.loads((work / 'usage.json').read_text())
    # Compact post-run observations; raw logs and trajectories stay under runs/.
    rematch, lns = [], []
    with (work / 'native.log').open() as stream:
        for line in stream:
            if not line.startswith(('[kk-rematch-diag]', '[kk-lns-diag]')): continue
            fields = {k: float(v) if any(c in v for c in '.eE') else int(v)
                      for k, v in re.findall(r'(\w+)=([-+\d.eE]+)', line)}
            (rematch if line.startswith('[kk-rematch-diag]') else lns).append(fields)
    assert all(r['rematch'] == case['rematch'] for r in rematch)
    write(work / 'search-diagnostics.json', {'rematching': rematch, 'lns': lns,
        'note': 'iteration_records are merged iteration records, not all speculative attempts.'})
    write(work / 'summary.json', {'case': case, 'exit': proc.returncode, 'valid_simulator_counters': valid,
        'result': data, 'wall_seconds': time.monotonic() - wall, 'usage': usage,
        'peak_rss_bytes': usage['peak_rss_kib'] * 1024,
        'within_cgar_memory_cap': usage['peak_rss_kib'] * 1024 < 32000000000,
        'complete_entry_deadline_series_verified': False,
        'started_utc': started, 'finished_utc': now(), 'scope': spec['scope']})
    return 0 if valid else 1


if __name__ == '__main__':
    raise SystemExit(main())
