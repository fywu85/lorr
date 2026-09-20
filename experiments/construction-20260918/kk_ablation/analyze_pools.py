"""Apply the validated historical task-pool replay to the completed KK factorial."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[3]
BASE = ROOT / 'experiments/construction-20260918'
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources


def read(path):
    return json.loads(Path(path).read_text())


def digest(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for chunk in iter(lambda: stream.read(4 * 1024 * 1024), b''):
            h.update(chunk)
    return h.hexdigest()


def write(path, data):
    Path(path).write_text(json.dumps(data, indent=2) + '\n')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--raw', type=Path, required=True)
    parser.add_argument('--execute', action='store_true')
    args = parser.parse_args(); raw = args.raw.resolve()
    spec = read(raw / 'spec.json')
    request_path = raw / 'pool-analysis-request.json'
    if not args.execute:
        assert not request_path.exists()
        audited = BASE / 'results/task-pool-audit-v50'
        cohorts = read(audited / 'cohorts.json')
        assert cohorts['pool_fixture_passed'] and cohorts['all_original_accounting_identical']
        provenance = read(audited / 'provenance.json')
        original = ROOT / 'runs/cgar-kk-task-pool-audit-v50-20260920/replay/decompose'
        assert digest(original) == provenance['binary_sha256']
        shutil.copy2(original, raw / 'pool-decompose')
        shutil.copy2(Path(__file__), raw / 'support/analyze_pools.py')
        files = [raw / 'pool-decompose', raw / 'support/analyze_pools.py', raw / 'spec.json',
                 audited / 'cohorts.json', audited / 'provenance.json',
                 ROOT / 'mr24/warehouse.domain/maps/warehouse_large.map']
        write(request_path, {'files': {str(p): digest(p) for p in files},
            'output': str(BASE / 'results/kk-guidance-rematch-full-v2/pools'),
            'validated_source_sha256': provenance['source_sha256']})
        command = ['/usr/bin/python3', str(raw / 'support/analyze_pools.py'), '--raw', str(raw), '--execute']
        script = raw / 'pool-analysis.sh'
        script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
        job = read(raw / 'analysis-submission.json')['response'].strip()
        assert job.isdigit()
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q',
            '-hold_jid', job, '-pe', 'threaded', '1', '-binding', 'linear:1',
            '-l', 'exclusive=true,h_rt=00:30:00,h_vmem=8G', '-m', 'n', '-N', 'kk_pool_analysis',
            '-j', 'y', '-o', str(raw / 'pool-analysis.log'), '-S', '/bin/bash', str(script)]
        proc = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(raw / 'pool-analysis-submission.json', {'command': submit, 'returncode': proc.returncode, 'response': proc.stdout})
        print(proc.stdout, end=''); return proc.returncode
    request = read(request_path)
    for path, expected in request['files'].items():
        assert digest(path) == expected, path
    cpu = cpu_resources()
    assert cpu['physical_cores_visible'] >= 1 and cpu['effective_cpu_quota'] is None
    os.sched_setaffinity(0, cpu['representative_cpus'][:1])
    out = Path(request['output']); out.mkdir(exist_ok=False)
    prior = read(out.parent / 'provenance.json')
    comparison = read(out.parent / 'comparison.json')
    rows = {}
    for case in spec['cases']:
        name = case['name']; target = raw / name / 'result.json'
        if name not in prior['inputs']:
            rows[name] = {'not_analyzed': 'No complete trajectory in prior full accounting.'}; continue
        assert digest(target) == prior['inputs'][name]['sha256']
        generated = raw / (name + '-pool-decomposition.json')
        subprocess.run([str(raw / 'pool-decompose'), str(ROOT / 'mr24/warehouse.domain/maps/warehouse_large.map'),
                        str(target), str(generated)], check=True)
        new = read(generated); old = read(out.parent / (name + '-decomposition.json'))
        assert {k: new[k] for k in old} == old, name
        assert all(s['outstanding']['chain']['n'] == 15000 for s in new['pool_snapshots'])
        row = next(r for r in comparison['rows'] if r['case'] == case)
        rows[name] = {'pool_snapshots': new['pool_snapshots'], 'limits': new['pool_limits'],
            'valid_simulator_counters': row['valid_simulator_counters'],
            'completed_tasks': new['completed_tasks'], 'completed_chain_shortest': new['completed_chain_shortest'],
            'trajectory_file_sha256': prior['inputs'][name]['sha256']}
    for path, expected in request['files'].items():
        assert digest(path) == expected, path
    write(out / 'cohorts.json', {'completed_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
        'cases': rows, 'all_original_accounting_identical': True, 'allocation': cpu_resources(),
        'scope': 'Descriptive KK factorial task-pool snapshots; clock and shared-weight confounds remain.'})
    for path in [request_path, raw / 'pool-analysis-submission.json']:
        shutil.copy2(path, out / path.name)
    print('KK_FACTORIAL_POOL_ANALYSIS_COMPLETE')


if __name__ == '__main__':
    raise SystemExit(main())
