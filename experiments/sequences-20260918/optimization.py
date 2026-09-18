#!/usr/bin/env python3
"""Compare original/optimized/original binaries on the same reserved physical cores."""
import argparse
import json
import os
from pathlib import Path
import shlex
import subprocess
import sys

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from benchmark_gridengine import write
from cpu_resources import cpu_resources


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--original', type=Path, required=True)
    p.add_argument('--optimized', type=Path, required=True)
    p.add_argument('--execute', action='store_true')
    a = p.parse_args(); out = a.output.resolve()
    if not a.execute:
        out.mkdir(parents=True, exist_ok=False)
        command = [sys.executable, str(Path(__file__).resolve()), '--execute', '--output', str(out),
                   '--original', str(a.original.resolve()), '--optimized', str(a.optimized.resolve())]
        script = out / 'job.sh'; script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(s) for s in command) + '\n')
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=true,h_rt=00:30:00,h_vmem=8G', '-m', 'n', '-N', 'sequence_timing',
                  '-j', 'y', '-o', str(out / 'job.log'), '-S', '/bin/bash', str(script)]
        r = subprocess.run(submit, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(out / 'submission.json', {'command': submit, 'response': r.stdout, 'exit': r.returncode})
        print(r.stdout, end=''); return r.returncode
    resources = cpu_resources(); write(out / 'allocation.json', resources)
    cpus = resources['representative_cpus'][:1]
    assert len(cpus) == 1 and (resources['effective_cpu_quota'] is None or resources['effective_cpu_quota'] >= 1)
    env = {k: v for k, v in os.environ.items() if not k.startswith('CGAR_')}
    env.update(CGAR_SEQUENCE_HORIZON='5', CGAR_SEQUENCE_PASSES='2', CGAR_DIAGNOSTICS='1')
    manifests = {}
    for label, build in [('original_a', a.original), ('optimized', a.optimized), ('original_b', a.original)]:
        manifests[label] = json.loads((build / 'build.json').read_text())
        command = [sys.executable, str(ROOT / 'tools/benchmark_cgar.py'), '--output', str(out / label),
                   '--binary', str(build / 'lifelong'), '--source-manifest', str(build / 'build.json'),
                   '--jobs', '1', '--cpu-list', ','.join(map(str, cpus)), '--seed', '0', '--steps', '200',
                   '--plan-time-limit-ms', '1000', '--instances', 'WAREHOUSE']
        subprocess.run(command, env=env, cwd=ROOT, check=True)
    write(out / 'builds.json', manifests)
    write(out / 'completion.json', {'completed': True, 'rounds': list(manifests)})
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
