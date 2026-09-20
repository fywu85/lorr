"""Freeze and submit full snapshot identity, deadline, and exhaustive joint checks."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
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
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def write(path, data):
    Path(path).write_text(json.dumps(data, indent=2) + '\n')


def fields(line):
    return {k: float(v) if '.' in v else int(v) for k, v in re.findall(r'(\w+)=([-\d.]+)', line)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--raw', type=Path, required=True)
    parser.add_argument('--commit')
    parser.add_argument('--execute', action='store_true')
    args = parser.parse_args(); raw = args.raw.resolve()
    if not args.execute:
        assert args.commit and raw.parent == ROOT / 'runs'
        (raw / 'support').mkdir(exist_ok=True)
        assert not (raw / 'group-analysis-request.json').exists()
        shutil.copy2(Path(__file__), raw / 'support/analyze_group_snapshots.py')
        shutil.copy2(Path(__file__).with_name('joint_probe.cpp'), raw / 'joint_probe.cpp')
        shutil.copy2(BASE / 'verify_full.py', raw / 'verify_full.py')
        shutil.copy2(BASE / 'results/pickup-full-regions-six-seed-v44.json', raw / 'reference.json')
        files = [raw / name for name in ['support/analyze_group_snapshots.py', 'joint_probe.cpp', 'verify_full.py', 'reference.json']]
        for name in ['sequences-20260918/analyze.py', 'motion-20260918/analyze.py', 'assignment-20260918/analyze.py',
                     'throughput-20260918-next/analyze_matrix.py', 'throughput-20260918-strict/analyze.py']:
            target = raw / 'support/experiments' / name; target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(ROOT / 'experiments' / name, target); files.append(target)
        write(raw / 'group-analysis-request.json', {'commit': args.commit, 'files': {str(p): digest(p) for p in files},
              'output': str(BASE / 'results/group-snapshot-full-v52')})
        command = ['/usr/bin/python3', str(raw / 'support/analyze_group_snapshots.py'), '--raw', str(raw), '--execute']
        script = raw / 'group-analysis.sh'
        script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
        job = read(raw / 'submission.json')['response'].strip(); assert job.isdigit()
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q',
                  '-hold_jid', job, '-pe', 'threaded', '1', '-binding', 'linear:1',
                  '-l', 'exclusive=true,h_rt=01:00:00,h_vmem=8G', '-m', 'n', '-N', 'group_analysis',
                  '-j', 'y', '-o', str(raw / 'group-analysis.log'), '-S', '/bin/bash', str(script)]
        proc = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(raw / 'motion-analysis-submission.json', {'command': submit, 'returncode': proc.returncode, 'response': proc.stdout})
        print(proc.stdout, end=''); return proc.returncode
    request = read(raw / 'group-analysis-request.json')
    for path, sha in request['files'].items():
        assert digest(path) == sha
    cpu = cpu_resources(); assert cpu['physical_cores_visible'] >= 1 and cpu['effective_cpu_quota'] is None
    os.sched_setaffinity(0, cpu['representative_cpus'][:1])
    out = Path(request['output'])
    subprocess.run(['/usr/bin/python3', str(raw / 'support/experiments/sequences-20260918/analyze.py'),
                    '--input', str(raw), '--output', str(out), '--control', 'snapshot_on', '--workers', '1'], check=True)
    sys.path.insert(0, str(raw))
    from verify_full import verify
    report = verify(raw, out, request['commit'], allow_failed=True)
    write(out / 'verification.json', report)
    references = {r['seed']: r for r in read(raw / 'reference.json')['rows'] if r['environment']['CGAR_TEMPORAL_REGIONS'] == '4'}
    binary = raw / 'joint_probe'
    compile_command = [str(ROOT / 'env/bin/x86_64-conda-linux-gnu-c++'), '-std=c++17', '-O2',
        '-I' + str(ROOT / 'cgar/inc'), str(raw / 'joint_probe.cpp'), '-Wl,-rpath,' + str(ROOT / 'env/lib'), '-o', str(binary)]
    subprocess.run(compile_command, check=True); subprocess.run([str(binary), '--self-test'], check=True)
    results = {}
    for row in report['rows']:
        ref = references[row['seed']]
        assert row['tasks'] == ref['tasks'] and row['trajectory_sha256'] == ref['trajectory_sha256']
        log = Path(row['raw_case']) / 'WAREHOUSE.log'; lines = log.read_text().splitlines()
        before = (Path(ref['raw_case']) / 'WAREHOUSE.log').read_text().splitlines()
        for tag in ['cgar-temporal', 'cgar-temporal-starts', 'cgar-temporal-rotation', 'cgar-temporal-regions',
                    'cgar-temporal-region-round', 'cgar-orientation']:
            prefix = '[' + tag + ']'
            a = [fields(x) for x in lines if x.startswith(prefix)]
            b = [fields(x) for x in before if x.startswith(prefix)]
            assert a == b and len(a) == (50 if tag == 'cgar-temporal-region-round' else 25), tag
        frames = [fields(x) for x in lines if x.startswith('[cgar-temporal-group-frame]')]
        assert [f['step'] for f in frames] == list(range(67, 5001, 67))
        captures = [json.loads(x.split('] ', 1)[1]) for x in lines if x.startswith('[cgar-temporal-group] ')]
        assert len(captures) == sum(f['groups'] for f in frames)
        for frame in frames:
            group = [g for g in captures if g['step'] == frame['step']]
            assert len(group) == frame['groups'] and frame['count_per_class'] == 8
            assert len({g['root'] for g in group}) == len(group)
            for n in [2, 3]:
                assert sum(len(g['robots']) == n for g in group) <= 8
        output = out / (row['case'] + '-joint.json')
        subprocess.run([str(binary), str(log), str(output)], check=True)
        joint = read(output); assert joint['all_groups_exhaustive'] and joint['fixtures_passed'] == 6
        groups = joint['groups']; assert len(groups) == len(captures)
        results[row['case']] = {'captured_frames': len(frames), 'captured_groups': len(captures), 'log_sha256': digest(log),
            'all_reference_search_counters_exact': True, 'windows': {label: {str(n): {
                'groups': sum(g['step'] > start and g['group_size'] == n for g in groups),
                'any_feasible': sum(g['step'] > start and g['group_size'] == n and g['feasible_forward_replacements'] > 0 for g in groups),
                'any_scalar_improving': sum(g['step'] > start and g['group_size'] == n and g['scalar_improving'] > 0 for g in groups),
                'any_both_improving': sum(g['step'] > start and g['group_size'] == n and g['both_improving'] > 0 for g in groups)
            } for n in [2, 3]} for label, start in [('all', 0), ('after1000', 1000), ('final1000', 4000)]}}
    report.update(capture_results=results, probe_binary_sha256=digest(binary), compile_command=compile_command,
                  scope='Same-state complete small-group diagnostic; no production reachability or throughput gain established.')
    write(out / 'audit.json', report)
    for name in ['group-analysis-request.json', 'joint_probe.cpp']:
        shutil.copy2(raw / name, out / name)
    for path, sha in request['files'].items():
        assert digest(path) == sha
    print('FULL_GROUP_SNAPSHOT_ANALYSIS_COMPLETE ' + json.dumps(results))


if __name__ == '__main__':
    raise SystemExit(main())
