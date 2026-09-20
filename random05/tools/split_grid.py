#!/usr/bin/env python3
"""Submit one GRID job per case so short runs release their cores promptly."""
import argparse
import datetime
import json
from pathlib import Path
import subprocess
import sys

RUNNER = Path(__file__).with_name('grid.py')


def write(path, value):
    path.write_text(json.dumps(value, indent=2) + '\n')


def now():
    return datetime.datetime.now(datetime.timezone.utc).isoformat()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=['submit', 'collect'])
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--cases', type=Path)
    parser.add_argument('--hosts')
    parser.add_argument('--cpu-model')
    args = parser.parse_args()
    out = args.output.resolve()
    if args.action == 'submit':
        cases = json.loads(args.cases.read_text())
        names = [c['name'] for c in cases]
        assert len(set(names)) == len(names), 'case names must be unique'
        assert all(Path(n).name == n and n not in ('.', '..') for n in names), 'unsafe case name'
        out.mkdir(parents=True, exist_ok=False)
        batch = dict(created_utc=now(), cases_input=str(args.cases.resolve()), jobs=[])
        for case in cases:
            name = case['name']
            manifest = out / (name + '.cases.json')
            write(manifest, [case])
            command = [sys.executable, str(RUNNER), 'submit', '--kind', 'benchmark',
                       '--output', str(out / name), '--cases', str(manifest)]
            if args.hosts:
                command += ['--hosts', args.hosts]
            if args.cpu_model:
                command += ['--cpu-model', args.cpu_model]
            result = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            batch['jobs'].append(dict(name=name, output=str(out / name),
                                      returncode=result.returncode, response=result.stdout))
            write(out / 'batch.json', batch)
            print('{}: {}'.format(name, result.stdout.strip()), flush=True)
        return int(any(job['returncode'] for job in batch['jobs']))
    batch = json.loads((out / 'batch.json').read_text())
    results = []
    for job in batch['jobs']:
        summary = Path(job['output']) / 'summary.json'
        if not summary.exists():
            continue
        rows = json.loads(summary.read_text())
        assert len(rows) == 1
        result = dict(rows[0], evidence=str(summary))
        results.append(result)
        print('{}: {} tasks, valid={}'.format(result['name'], result.get('result', {}).get('numTaskFinished'), result['valid']))
    write(out / 'summary.json', results)
    if len(results) == len(batch['jobs']):
        write(out / 'completion.json', dict(finished_utc=now(), valid=all(r['valid'] for r in results)))
    print('{}/{} cases complete'.format(len(results), len(batch['jobs'])))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
