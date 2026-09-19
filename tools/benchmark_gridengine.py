#!/usr/bin/env python3
"""Submit CGAR benchmarks to an exclusive Grid Engine allocation with per-core binding."""
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

from cpu_resources import cpu_resources

ROOT = Path(__file__).resolve().parents[1]
# Include prototype headers when an archived source patch adds one.
SOURCES = (['cgar/CMakeLists.txt', 'cgar/inc/Entry.h', 'cgar/inc/CompetitionSystem.h',
            'cgar/src/CompetitionSystem.cpp', 'cgar/cgar_planner/cgar.cpp'] +
           sorted(str(p.relative_to(ROOT)) for p in (ROOT / 'cgar/cgar_planner').glob('*.hpp')) +
           ['cgar/src/MAPFPlanner.cpp', 'cgar/src/TaskScheduler.cpp', 'cgar/src/Entry.cpp'])


def write(path, value):
    path.write_text(json.dumps(value, indent=2) + '\n')


def execute(spec_path):
    spec = json.loads(spec_path.read_text())
    out = spec_path.parent
    resources = cpu_resources()
    slots = int(os.environ.get('NSLOTS', '0'))
    cpus = resources['representative_cpus'][:spec['jobs']]
    quota = resources['effective_cpu_quota']
    assert slots >= spec['jobs'] and len(cpus) == spec['jobs'], (slots, resources)
    assert quota is None or quota >= spec['jobs'], resources
    for path, expected in spec['sources'].items():
        assert hashlib.sha256((ROOT / path).read_bytes()).hexdigest() == expected, path
    allocation = {'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  'resources': resources, 'selected_cpus': cpus,
                  'scheduler': {key: os.environ.get(key) for key in ['JOB_ID', 'QUEUE', 'NSLOTS', 'PE']}}
    write(out / 'allocation.json', allocation)
    env = {key: value for key, value in os.environ.items() if not key.startswith('CGAR_')}
    env.update(spec['environment'])
    command = [sys.executable, str(ROOT / 'tools/benchmark_cgar.py'), '--output', str(out / 'results'),
               '--binary', str(out / 'lifelong'), '--jobs', str(spec['jobs']),
               '--cpu-list', ','.join(map(str, cpus)), '--seed', str(spec['seed']),
               '--plan-time-limit-ms', str(spec['plan_time_limit_ms']),
               '--instances'] + spec['instances']
    if spec['steps'] is not None:
        command += ['--steps', str(spec['steps'])]
    if spec.get('horizon_profile') is not None:
        command += ['--horizon-profile', str(out / 'horizons.json')]
    result = subprocess.run(command, cwd=str(ROOT), env=env)
    write(out / 'completion.json', {'returncode': result.returncode,
          'finished_utc': datetime.datetime.now(datetime.timezone.utc).isoformat()})
    return result.returncode


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--jobs', type=int, default=10)
    parser.add_argument('--binary', type=Path, default=ROOT / 'cgar/build/lifelong')
    parser.add_argument('--instances', nargs='+')
    parser.add_argument('--steps', type=int)
    parser.add_argument('--horizon-profile', type=Path)
    parser.add_argument('--plan-time-limit-ms', type=int, default=1000)
    parser.add_argument('--seed', type=int, default=0)
    parser.add_argument('--env', action='append', default=[], help='Explicit CGAR_NAME=value override')
    parser.add_argument('--runtime', default='01:00:00', help='Grid Engine wall-time limit')
    parser.add_argument('--memory-per-slot', default='8G')
    parser.add_argument('--execute-spec', type=Path, help=argparse.SUPPRESS)
    args = parser.parse_args()
    if args.execute_spec:
        return execute(args.execute_spec.resolve())
    if not args.output or args.jobs < 1 or args.plan_time_limit_ms < 1 or (args.steps is not None and args.steps < 1):
        parser.error('a fresh --output directory and positive jobs/steps are required')
    if args.steps is not None and args.horizon_profile is not None:
        parser.error('steps and horizon-profile are mutually exclusive')
    qsub = shutil.which('qsub')
    if not qsub:
        parser.error('qsub is not installed')
    instances = args.instances or list(json.loads((ROOT / 'mr24/simulation_time.json').read_text()))
    known = set(json.loads((ROOT / 'mr24/simulation_time.json').read_text()))
    if len(set(instances)) != len(instances) or not set(instances).issubset(known):
        parser.error('instances must be distinct MR24 names')
    profile = json.loads(args.horizon_profile.read_text()) if args.horizon_profile else None
    if profile is not None and (not isinstance(profile, dict) or not set(instances).issubset(profile) or
                                any(type(v) is not int or v < 1 for v in profile.values())):
        parser.error('horizon-profile must give a positive integer horizon for every selected instance')
    environment = {}
    for setting in args.env:
        key, sep, value = setting.partition('=')
        if not sep or not key.startswith('CGAR_'):
            parser.error('--env must use CGAR_NAME=value')
        environment[key] = value
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=False)
    shutil.copy2(args.binary.resolve(), out / 'lifelong')
    spec = {'prepared_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
            'source_commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=str(ROOT), text=True).strip(),
            'sources': {p: hashlib.sha256((ROOT / p).read_bytes()).hexdigest() for p in SOURCES},
            'binary_sha256': hashlib.sha256((out / 'lifelong').read_bytes()).hexdigest(),
            'jobs': min(args.jobs, len(instances)), 'instances': instances, 'steps': args.steps,
            'seed': args.seed, 'environment': environment, 'plan_time_limit_ms': args.plan_time_limit_ms,
            'allocation_policy': 'one physical core per concurrent single-threaded instance',
            'source_dirty': bool(subprocess.check_output(['git', 'status', '--porcelain'], cwd=str(ROOT), text=True)),
            'requested_exclusive': True, 'memory_per_slot': args.memory_per_slot, 'runtime_limit': args.runtime}
    if profile is not None:
        write(out / 'horizons.json', profile)
    spec['horizon_profile'] = profile
    write(out / 'spec.json', spec)
    command = ['/usr/bin/python3', str(Path(__file__).resolve()), '--execute-spec', str(out / 'spec.json')]
    (out / 'job.sh').write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
    submit = [qsub, '-terse', '-w', 'e', '-cwd', '-q', 'debian.q', '-pe', 'threaded', str(spec['jobs']),
              '-binding', 'linear:' + str(spec['jobs']), '-l',
              'exclusive=true,h_rt={},h_vmem={}'.format(args.runtime, args.memory_per_slot),
              '-m', 'n', '-N', 'lorr_benchmark', '-j', 'y', '-o', str(out / 'scheduler.log'),
              '-S', '/bin/bash', str(out / 'job.sh')]
    result = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    write(out / 'submission.json', {'command': submit, 'returncode': result.returncode, 'response': result.stdout})
    print(result.stdout, end='', flush=True)
    return result.returncode


if __name__ == '__main__':
    raise SystemExit(main())
