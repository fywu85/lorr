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
ROOT = HERE.parents[2]
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
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', ','.join('debian.q@research%d.grid.gsb' % h for h in [31,38,44,46,50,52,57]), '-pe', 'threaded', '4',
                  '-binding', 'linear:4', '-l', 'exclusive=true,h_rt=00:30:00,h_vmem=4G', '-m', 'n', '-N', 'deadline_timing',
                  '-j', 'y', '-o', str(out / 'job.log'), '-S', '/bin/bash', str(script)]
        r = subprocess.run(submit, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(out / 'submission.json', {'command': submit, 'response': r.stdout, 'exit': r.returncode})
        print(r.stdout, end=''); return r.returncode
    resources = cpu_resources(); write(out / 'allocation.json', resources)
    cpus = resources['representative_cpus'][:4]
    assert len(cpus) == 4 and resources['effective_cpu_quota'] is None
    assert resources['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
    write(out / 'allocation.json', {'resources': resources, 'selected_cpus': cpus})
    env = {k: v for k, v in os.environ.items() if not k.startswith('CGAR_')}
    profile = next(iter(json.loads((HERE.parent / 'warehouse-reference-variants.json').read_text()).values()))
    env.update(profile)
    cases = [{'name': label, 'variant': label, 'seed': 0, 'environment': profile} for label in ['original_a', 'optimized', 'original_b']]
    write(out / 'spec.json', {'horizons': {'WAREHOUSE': 200}, 'time_limit_ms': 1000, 'cpus_per_instance': 4, 'cases': cases, 'purpose': 'A/B/A timing on identical cores; exact prefix equivalence only, not throughput ranking.'})
    codes = []
    manifests = {}
    for label, build in [('original_a', a.original), ('optimized', a.optimized), ('original_b', a.original)]:
        manifests[label] = json.loads((build / 'build.json').read_text())
        command = [sys.executable, str(ROOT / 'tools/benchmark_cgar.py'), '--output', str(out / label),
                   '--binary', str(build / 'lifelong'), '--source-manifest', str(build / 'build.json'),
                   '--jobs', '1', '--cpus-per-instance', '4', '--log-detail-level', '2', '--cpu-list', ','.join(map(str, cpus)), '--seed', '0', '--steps', '200',
                   '--plan-time-limit-ms', '1000', '--instances', 'WAREHOUSE']
        codes.append(subprocess.run(command, env=env, cwd=ROOT).returncode)
    write(out / 'builds.json', manifests)
    write(out / 'completion.json', {'completed': True, 'rounds': list(manifests), 'returncode': int(any(codes)), 'case_returncodes': codes, 'finished_utc': __import__('datetime').datetime.now(__import__('datetime').timezone.utc).isoformat()})
    return int(any(codes))


if __name__ == '__main__':
    raise SystemExit(main())
