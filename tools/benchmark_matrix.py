#!/usr/bin/env python3
"""Run fixed CGAR variants and paired seeds on reserved Grid Engine cores."""
import argparse
import concurrent.futures
import datetime
import hashlib
import json
import os
from pathlib import Path
import queue
import shlex
import shutil
import subprocess
import sys
from benchmark_gridengine import ROOT, SOURCES, write
from cpu_resources import cpu_resources


def execute(out):
    spec = json.loads((out / 'spec.json').read_text())
    resources = cpu_resources()
    count = spec['parallel_suites'] * spec['jobs_per_suite']
    cpus = resources['representative_cpus'][:count]
    assert len(cpus) == count and int(os.environ['NSLOTS']) >= count, resources
    assert resources['effective_cpu_quota'] is None or resources['effective_cpu_quota'] >= count, resources
    assert hashlib.sha256((out / 'lifelong').read_bytes()).hexdigest() == spec['build']['binary_sha256']
    write(out / 'allocation.json', {'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
          'resources': resources, 'selected_cpus': cpus, 'job_id': os.environ.get('JOB_ID')})
    groups = queue.Queue()
    for start in range(0, count, spec['jobs_per_suite']):
        groups.put(cpus[start:start + spec['jobs_per_suite']])

    def run(case):
        selected = groups.get()
        try:
            env = {k: v for k, v in os.environ.items() if not k.startswith('CGAR_')}
            env.update(case['environment'])
            command = [sys.executable, str(ROOT / 'tools/benchmark_cgar.py'), '--output', str(out / case['name']),
                       '--binary', str(out / 'lifelong'), '--source-manifest', str(out / 'build.json'),
                       '--jobs', str(spec['jobs_per_suite']), '--cpu-list', ','.join(map(str, selected)),
                       '--seed', str(case['seed']), '--plan-time-limit-ms', str(spec['time_limit_ms']),
                       '--instances'] + spec['instances']
            if spec['horizons']:
                command += ['--horizon-profile', str(out / 'horizons.json')]
            with (out / (case['name'] + '.log')).open('w') as log:
                result = subprocess.run(command, cwd=str(ROOT), env=env, stdout=log, stderr=subprocess.STDOUT)
            status = {'case': case['name'], 'returncode': result.returncode,
                      'finished_utc': datetime.datetime.now(datetime.timezone.utc).isoformat()}
            write(out / (case['name'] + '-completion.json'), status)
            print(json.dumps(status), flush=True)
            return result.returncode
        finally:
            groups.put(selected)
    with concurrent.futures.ThreadPoolExecutor(max_workers=spec['parallel_suites']) as pool:
        codes = list(pool.map(run, spec['cases']))
    code = int(any(codes))
    write(out / 'completion.json', {'returncode': code, 'case_returncodes': codes,
          'finished_utc': datetime.datetime.now(datetime.timezone.utc).isoformat()})
    return code


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--variants', type=Path, help='JSON object mapping variant names to CGAR environment overrides')
    parser.add_argument('--binary', type=Path, default=ROOT / 'cgar/build/lifelong')
    parser.add_argument('--source-manifest', type=Path)
    parser.add_argument('--horizon-profile', type=Path)
    parser.add_argument('--seeds', type=int, nargs='+', default=[0, 1, 2])
    parser.add_argument('--repeat', type=int, default=1)
    parser.add_argument('--instances', nargs='+')
    parser.add_argument('--parallel-suites', type=int, default=6)
    parser.add_argument('--jobs-per-suite', type=int, default=5)
    parser.add_argument('--time-limit-ms', type=int, default=1000)
    parser.add_argument('--memory-gib-per-slot', type=int, default=8)
    parser.add_argument('--runtime', default='01:00:00', help='Grid Engine wall-time limit; distinct from the per-decision limit')
    parser.add_argument('--execute', action='store_true', help=argparse.SUPPRESS)
    args = parser.parse_args()
    out = args.output.resolve()
    if args.execute:
        return execute(out)
    if not args.variants or min(args.repeat, args.parallel_suites, args.jobs_per_suite, args.time_limit_ms, args.memory_gib_per_slot) < 1:
        parser.error('variants and positive work limits are required')
    variants = json.loads(args.variants.read_text())
    if not isinstance(variants, dict) or not variants:
        parser.error('variants must be a nonempty JSON object')
    if len(set(args.seeds)) != len(args.seeds):
        parser.error('seeds must be distinct')
    for name, environment in variants.items():
        if not name.replace('_', '').replace('-', '').isalnum() or not isinstance(environment, dict):
            parser.error('variant names must be simple labels, with environment dictionaries')
        if any(not k.startswith('CGAR_') or not isinstance(v, str) for k, v in environment.items()):
            parser.error('variant overrides must map CGAR_ keys to string values')
    known = json.loads((ROOT / 'mr24/simulation_time.json').read_text())
    instances = args.instances or list(known)
    if len(set(instances)) != len(instances) or not set(instances).issubset(known):
        parser.error('instances must be distinct MR24 names')
    cases = [{'name': '{}-s{}-r{}'.format(name, seed, repeat), 'variant': name, 'seed': seed,
              'repeat': repeat, 'environment': env} for repeat in range(args.repeat) for seed in args.seeds for name, env in variants.items()]
    if not cases:
        parser.error('at least one variant is required')
    build = json.loads(args.source_manifest.read_text()) if args.source_manifest else {
        'source_commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=str(ROOT), text=True).strip(),
        'source_dirty': bool(subprocess.check_output(['git', 'status', '--porcelain'], cwd=str(ROOT), text=True)),
        'sources': {p: hashlib.sha256((ROOT / p).read_bytes()).hexdigest() for p in SOURCES},
        'binary_sha256': hashlib.sha256(args.binary.read_bytes()).hexdigest()}
    if hashlib.sha256(args.binary.read_bytes()).hexdigest() != build['binary_sha256']:
        parser.error('source-manifest does not match the executable')
    horizons = json.loads(args.horizon_profile.read_text()) if args.horizon_profile else None
    if horizons is not None and (not isinstance(horizons, dict) or not set(instances).issubset(horizons) or
                                 any(type(v) is not int or v < 1 for v in horizons.values())):
        parser.error('horizon-profile must give a positive integer horizon for every selected instance')
    out.mkdir(parents=True, exist_ok=False)
    shutil.copy2(args.binary, out / 'lifelong')
    write(out / 'build.json', build)
    if horizons:
        write(out / 'horizons.json', horizons)
    spec = {'cases': cases, 'build': build, 'horizons': horizons, 'instances': instances,
            'parallel_suites': min(args.parallel_suites, len(cases)), 'jobs_per_suite': min(args.jobs_per_suite, len(instances)),
            'time_limit_ms': args.time_limit_ms, 'memory_gib_per_slot': args.memory_gib_per_slot, 'runtime': args.runtime}
    write(out / 'spec.json', spec)
    command = ['/usr/bin/python3', str(Path(__file__).resolve()), '--execute', '--output', str(out)]
    (out / 'job.sh').write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
    cores = spec['parallel_suites'] * spec['jobs_per_suite']
    submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q', '-pe', 'threaded', str(cores),
              '-binding', 'linear:' + str(cores), '-l', 'exclusive=true,h_rt=' + args.runtime + ',h_vmem=' + str(args.memory_gib_per_slot) + 'G', '-m', 'n', '-N', 'lorr_matrix',
              '-j', 'y', '-o', str(out / 'scheduler.log'), '-S', '/bin/bash', str(out / 'job.sh')]
    result = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    write(out / 'submission.json', {'command': submit, 'returncode': result.returncode, 'response': result.stdout})
    print(result.stdout, end='', flush=True)
    return result.returncode


if __name__ == '__main__':
    raise SystemExit(main())
