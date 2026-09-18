#!/usr/bin/env python3
"""Extract full WAREHOUSE leader curves on reserved CPUs, retaining invalid runs."""
import argparse
import concurrent.futures
import json
from pathlib import Path
import shlex
import subprocess
import sys
HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE.parent / 'throughput-20260918-strict'))
from analyze import extract


def analyze_case(item):
    root, case = item
    work = root / case['name']
    summary = json.loads((work / 'summary.json').read_text())
    if not (work / 'result.log').exists():
        (work / 'result.log').symlink_to('native.log')
    metrics = extract((case['name'], work / 'result.json'))
    metrics['instance'] = 'WAREHOUSE'
    return dict(case=case, summary=summary, metrics=metrics,
                allocation=json.loads((work / 'allocation.json').read_text()))


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--input', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--hold-job')
    p.add_argument('--execute', action='store_true')
    a = p.parse_args(); root = a.input.resolve(); out = a.output.resolve()
    if not a.execute:
        command = [sys.executable, str(Path(__file__).resolve()), '--execute', '--input', str(root), '--output', str(out)]
        script = root / 'analysis.sh'
        script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(s) for s in command) + '\n')
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q',
                  '-pe', 'threaded', '2', '-binding', 'linear:2', '-l', 'exclusive=true,h_rt=00:30:00,h_vmem=8G',
                  '-m', 'n', '-N', 'warehouse_leader_analysis', '-j', 'y', '-o', str(root / 'analysis.log'), '-S', '/bin/bash']
        if a.hold_job: submit += ['-hold_jid', a.hold_job]
        result = subprocess.run(submit + [str(script)], cwd=ROOT, capture_output=True, text=True)
        (root / 'analysis-submission.json').write_text(json.dumps({'command': submit + [str(script)],
            'exit': result.returncode, 'stdout': result.stdout, 'stderr': result.stderr}, indent=2) + '\n')
        print(result.stdout, end=''); return result.returncode
    spec = json.loads((root / 'spec.json').read_text())
    with concurrent.futures.ProcessPoolExecutor(max_workers=2) as pool:
        rows = list(pool.map(analyze_case, [(root, c) for c in spec['cases']]))
    out.mkdir(parents=True, exist_ok=True)
    for name, value in [('results.json', rows), ('spec.json', spec),
                        ('submission.json', json.loads((root / 'submission.json').read_text()))]:
        (out / name).write_text(json.dumps(value, indent=2) + '\n')
    print('WAREHOUSE_LEADERS_ANALYZED', len(rows))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
