#!/usr/bin/env python3
"""Evaluate archived leaders with 32 logical CPUs and competition preprocessing allowance."""
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
import time
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from benchmark_cgar import FIELDS, header
from benchmark_gridengine import write
from cpu_resources import cpu_resources


def execute(out):
    spec = json.loads((out / 'spec.json').read_text())
    index = int(os.environ['SGE_TASK_ID']) - 1
    case = spec['cases'][index]
    work = out / case['name']; work.mkdir()
    shutil.copytree(out / 'assets' / case['team'], work / 'cwd')
    resources = cpu_resources()
    assert len(resources['logical_cpu_affinity']) == 32 and resources['physical_cores_visible'] == 16, resources
    assert resources['effective_cpu_quota'] is None or resources['effective_cpu_quota'] >= 32, resources
    write(work / 'allocation.json', {'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
          'resources': resources, 'job_id': os.environ.get('JOB_ID'), 'task_id': index + 1})
    env = {k: v for k, v in os.environ.items() if not k.startswith('CGAR_') and k not in ['CONFIG_PATH', 'MAP_WEIGHT_PATH', 'LNS_NUM_THREADS']}
    env.update({'OMP_NUM_THREADS': '32', 'OMP_DYNAMIC': 'FALSE'})
    command = [sys.executable, str(ROOT / 'tools/run_with_usage.py'), '--output', str(work / 'usage.json'), '--',
               str(out / (case['team'] + '-lifelong')), '-i', case['input'], '-o', str(work / 'result.json'),
               '-s', str(case['steps']), '-t', '1000', '-p', '1800000']
    start = time.monotonic()
    with (work / 'native.log').open('w') as log:
        run = subprocess.run(command, cwd=str(work / 'cwd'), env=env, stdout=log, stderr=subprocess.STDOUT)
    data = header(work / 'result.json') if (work / 'result.json').exists() else dict.fromkeys(FIELDS)
    valid = run.returncode == 0 and data['makespan'] == case['steps'] and all(data[k] == 0 for k in FIELDS[3:])
    write(work / 'summary.json', dict(case, exit=run.returncode, valid=valid, result=data,
          wall_seconds=time.monotonic() - start, usage=json.loads((work / 'usage.json').read_text()),
          finished_utc=datetime.datetime.now(datetime.timezone.utc).isoformat()))
    return 0 if valid else 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--steps', type=int, default=1000, help='Cap archived horizons at this value')
    parser.add_argument('--execute', action='store_true', help=argparse.SUPPRESS)
    args = parser.parse_args(); out = args.output.resolve()
    if args.execute:
        return execute(out)
    out.mkdir(parents=True, exist_ok=False)
    times = json.loads((ROOT / 'mr24/simulation_time.json').read_text())
    inputs = {p.stem: str(p.resolve()) for p in (ROOT / 'mr24').glob('*/*.json')}
    sources, binaries = {}, {}
    for team in ['kk', 'nms']:
        project = (ROOT / team).resolve()
        shutil.copytree(project, out / 'assets' / team,
                        ignore=shutil.ignore_patterns('build', '.git', '__pycache__', '*.log', 'printer.txt'))
        shutil.copy2(project / 'build/lifelong', out / (team + '-lifelong'))
        binaries[team] = hashlib.sha256((out / (team + '-lifelong')).read_bytes()).hexdigest()
        sources[team] = {str(p.relative_to(out / 'assets' / team)): hashlib.sha256(p.read_bytes()).hexdigest()
                         for p in (out / 'assets' / team).rglob('*') if p.is_file()}
    cases = [{'name': team + '-' + name, 'team': team, 'instance': name, 'input': inputs[name],
              'steps': min(args.steps, steps)} for team in ['kk', 'nms'] for name, steps in times.items()]
    write(out / 'spec.json', {'cases': cases, 'binary_sha256': binaries, 'source_hashes': sources,
          'source_commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=str(ROOT), text=True).strip(),
          'time_limit_ms': 1000, 'preprocess_limit_ms': 1800000, 'logical_cpus_per_case': 32,
          'physical_cores_per_case': 16, 'memory_gib_per_case': 128,
          'note': 'Leader snapshots retain their supplied map-specific policies; no such policies are copied into CGAR.'})
    command = ['/usr/bin/python3', str(Path(__file__).resolve()), '--execute', '--output', str(out)]
    (out / 'job.sh').write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
    submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q', '-t', '1-20', '-tc', '20',
              '-pe', 'threaded', '32', '-binding', 'linear:16', '-l', 'exclusive=true,h_rt=01:00:00,h_vmem=4G',
              '-m', 'n', '-N', 'lorr_references', '-j', 'y', '-o', str(out), '-S', '/bin/bash', str(out / 'job.sh')]
    run = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    write(out / 'submission.json', {'command': submit, 'returncode': run.returncode, 'response': run.stdout})
    print(run.stdout, end='', flush=True)
    return run.returncode


if __name__ == '__main__':
    raise SystemExit(main())
