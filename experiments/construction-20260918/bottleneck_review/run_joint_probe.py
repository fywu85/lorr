"""Compile and run exhaustive captured-group diagnostics on one reserved GRID core."""
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


def digest(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for data in iter(lambda: stream.read(4 * 1024 * 1024), b''):
            h.update(data)
    return h.hexdigest()


def write(path, data):
    path.write_text(json.dumps(data, indent=2) + '\n')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--logs', type=Path, help='JSON mapping report names to completed capture logs')
    parser.add_argument('--result-name')
    parser.add_argument('--execute', action='store_true')
    args = parser.parse_args(); raw = args.output.resolve()
    if not args.execute:
        assert raw.parent == ROOT / 'runs' and args.logs and args.result_name
        assert Path(args.result_name).name == args.result_name and args.result_name not in ('', '.', '..')
        logs = json.loads(args.logs.read_text())
        assert logs and all(Path(name).name == name and name not in ('', '.', '..') for name in logs)
        raw.mkdir(exist_ok=False); (raw / 'support').mkdir()
        shutil.copy2(Path(__file__), raw / 'support/run_joint_probe.py')
        shutil.copy2(Path(__file__).with_name('joint_probe.cpp'), raw / 'joint_probe.cpp')
        spec = {'logs': {name: {'path': str(Path(path).resolve()), 'sha256': digest(path)} for name, path in logs.items()},
                'files': {name: digest(raw / name) for name in ['joint_probe.cpp', 'support/run_joint_probe.py']},
                'result_name': args.result_name}
        write(raw / 'spec.json', spec)
        command = ['/usr/bin/python3', str(raw / 'support/run_joint_probe.py'), '--output', str(raw), '--execute']
        (raw / 'job.sh').write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q',
                  '-pe', 'threaded', '1', '-binding', 'linear:1', '-l', 'exclusive=true,h_rt=01:00:00,h_vmem=8G',
                  '-m', 'n', '-N', 'joint_probe', '-j', 'y', '-o', str(raw / 'job.log'), '-S', '/bin/bash', str(raw / 'job.sh')]
        proc = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(raw / 'submission.json', {'command': submit, 'returncode': proc.returncode, 'response': proc.stdout})
        print(proc.stdout, end=''); return proc.returncode
    spec = json.loads((raw / 'spec.json').read_text())
    for name, sha in spec['files'].items():
        assert digest(raw / name) == sha
    cpu = cpu_resources(); assert cpu['physical_cores_visible'] >= 1 and cpu['effective_cpu_quota'] is None
    os.sched_setaffinity(0, cpu['representative_cpus'][:1])
    binary = raw / 'joint_probe'
    command = [str(ROOT / 'env/bin/x86_64-conda-linux-gnu-c++'), '-std=c++17', '-O2',
               '-I' + str(ROOT / 'cgar/inc'), str(raw / 'joint_probe.cpp'), '-Wl,-rpath,' + str(ROOT / 'env/lib'), '-o', str(binary)]
    subprocess.run(command, check=True)
    subprocess.run([str(binary), '--self-test'], check=True)
    rows = {}
    for name, record in spec['logs'].items():
        assert digest(record['path']) == record['sha256']
        subprocess.run([str(binary), record['path'], str(raw / (name + '.json'))], check=True)
        assert digest(record['path']) == record['sha256']
        report = json.loads((raw / (name + '.json')).read_text())
        assert report['fixtures_passed'] == 6 and report['all_groups_exhaustive']
        groups = report['groups']
        rows[name] = {'groups': len(groups), 'by_size': {str(n): {
            'groups': sum(g['group_size'] == n for g in groups),
            'any_feasible_forward': sum(g['group_size'] == n and g['feasible_forward_replacements'] > 0 for g in groups),
            'any_scalar_improving': sum(g['group_size'] == n and g['scalar_improving'] > 0 for g in groups),
            'any_both_improving': sum(g['group_size'] == n and g['both_improving'] > 0 for g in groups)} for n in (2, 3)}}
    out = BASE / 'results' / spec['result_name']; out.mkdir(exist_ok=False)
    for name in spec['logs']:
        shutil.copy2(raw / (name + '.json'), out / (name + '.json'))
    for name in ['spec.json', 'submission.json', 'joint_probe.cpp']:
        shutil.copy2(raw / name, out / name)
    write(out / 'summary.json', {'completed_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
        'allocation': cpu, 'binary_sha256': digest(binary), 'command': command, 'fixtures_passed': 6,
        'rows': rows, 'scope': 'Complete same-state small-group enumeration; no policy gain or production reachability conclusion.'})
    print(json.dumps(rows, indent=2))


if __name__ == '__main__':
    raise SystemExit(main())
