#!/usr/bin/env python3
"""Build and compare upstream PIBT and CGAR on reserved physical cores."""
import argparse
import concurrent.futures
import hashlib
import json
import os
from pathlib import Path
import random
import shlex
import shutil
import subprocess
import sys

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources
from benchmark_gridengine import SOURCES, write


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def generate():
    cases = []
    for topology in ['open', 'obstacles', 'bridge', 'disconnected', 'pockets', 'cycle']:
        rows = cols = 8
        obstacles = [0] * (rows * cols)
        if topology == 'obstacles':
            for p in [10, 11, 18, 22, 26, 27, 35, 38, 42, 43, 50, 54]: obstacles[p] = 1
        elif topology in ['bridge', 'disconnected']:
            for r in range(rows): obstacles[r * cols + 4] = int(topology == 'disconnected' or r != 4)
        elif topology == 'pockets':
            for r in range(1, 7):
                for c in [2, 4, 6]: obstacles[r * cols + c] = 1
        elif topology == 'cycle':
            for r in range(1, 7):
                for c in range(1, 7): obstacles[r * cols + c] = 1
        free = [p for p, blocked in enumerate(obstacles) if not blocked]
        for fill in [0.25, 0.75, 1.0]:
            for seed in range(8):
                rng = random.Random(70000 + seed)
                n = max(1, int(len(free) * fill))
                positions = rng.sample(free, n); goals = rng.sample(free, n)
                states = [[p, rng.randrange(4)] for p in positions]
                for model in ['native', 'rotation']:
                    name = '{}-n{}-s{}-{}'.format(topology, n, seed, model)
                    cases.append({'name': name, 'topology': topology, 'rows': rows, 'cols': cols,
                                  'map': obstacles, 'states': states, 'goals': goals,
                                  'seed': seed, 'steps': 256, 'model': model})
    return cases


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--execute', action='store_true')
    p.add_argument('--snapshots', action='store_true')
    p.add_argument('--build-dir', type=Path)
    p.add_argument('--jobs', type=int, default=4)
    a = p.parse_args(); out = a.output.resolve()
    if a.jobs < 1 or (a.snapshots and not a.build_dir): p.error('positive jobs and --build-dir for snapshots are required')
    if not a.execute:
        out.mkdir(parents=True, exist_ok=False)
        sources = SOURCES + [str(p.relative_to(ROOT)) for p in HERE.glob('*.cpp')] + [str(p.relative_to(ROOT)) for p in HERE.glob('*.hpp')]
        write(out / 'requested.json', {name: digest(ROOT / name) for name in sources})
        command = [sys.executable, str(Path(__file__).resolve()), '--execute', '--output', str(out)]
        command += ['--jobs', str(a.jobs)]
        if a.snapshots: command += ['--snapshots', '--build-dir', str(a.build_dir.resolve())]
        script = out / 'job.sh'
        script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q', '-pe', 'threaded', str(a.jobs),
                  '-binding', 'linear:' + str(a.jobs), '-l', 'exclusive=true,h_rt=01:00:00,h_vmem=8G', '-m', 'n', '-N', 'pibt_equivalence',
                  '-j', 'y', '-o', str(out / 'job.log'), '-S', '/bin/bash', str(script)]
        r = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(out / 'submission.json', {'command': submit, 'exit': r.returncode, 'response': r.stdout})
        print(r.stdout, end=''); return r.returncode
    requested = json.loads((out / 'requested.json').read_text())
    for name, expected in requested.items(): assert digest(ROOT / name) == expected, name
    upstream = json.loads((HERE / 'upstream.json').read_text())
    for name, expected in upstream['files'].items(): assert digest(HERE / 'vendor/pibt2' / name) == expected, name
    resources = cpu_resources(); write(out / 'allocation.json', resources)
    cpus = resources['representative_cpus'][:a.jobs]
    assert len(cpus) == a.jobs and (resources['effective_cpu_quota'] is None or resources['effective_cpu_quota'] >= a.jobs)
    env = {k: v for k, v in os.environ.items() if not k.startswith('CGAR_')}
    env['PATH'] = str(ROOT / 'env/bin') + ':' + env['PATH']
    if not a.snapshots:
        build = out / 'build'
        subprocess.run(['cmake', '-S', str(HERE), '-B', str(build), '-DCMAKE_PREFIX_PATH=' + str(ROOT / 'env'),
                        '-DCMAKE_CXX_COMPILER=' + str(ROOT / 'env/bin/x86_64-conda-linux-gnu-c++')], env=env, check=True)
        subprocess.run(['cmake', '--build', str(build), '-j4'], env=env, check=True)
        write(out / 'build.json', {'sources': requested, 'upstream': upstream,
              'binaries': {name: digest(build / name) for name in ['pibt_reference', 'cgar_reference']},
              'exposed_header_sha256': digest(build / 'upstream/pibt2/include/pibt.hpp'),
              'header_change': 'Added friend class PibtStepHarness; no algorithm statements changed.'})
    else:
        build = a.build_dir.resolve()
        recorded = json.loads((build.parent / 'build.json').read_text())
        for name, expected in recorded['binaries'].items(): assert digest(build / name) == expected
        for name, expected in recorded['sources'].items(): assert requested[name] == expected, name
        write(out / 'build.json', recorded)
    if a.snapshots:
        inputs = HERE.parent / 'motion-20260918'
        cases = []
        for item in json.loads((inputs / 'results/snapshot-inputs.json').read_text()):
            source = inputs / item['snapshot']; assert digest(source) == item['sha256']
            case = json.loads(source.read_text())
            case.update(name=item['name'] + '-rotation', topology=case['instance'], seed=0,
                        steps=100, model='rotation', reference_snapshot_sha256=item['sha256'])
            cases.append(case)
    else:
        cases = generate()
    write(out / 'cases.json', cases)
    def run_group(items):
        cpu, group = items; rows = []
        for case in group:
            work = out / case['name']; work.mkdir()
            write(work / 'input.json', case); outputs = {}
            row = {'name': case['name'], 'topology': case['topology'], 'model': case['model'],
                   'seed': case['seed'], 'robots': len(case['goals']), 'input_sha256': digest(work / 'input.json')}
            for team in ['pibt_reference', 'cgar_reference']:
                target = work / (team + '.json')
                with (work / (team + '.log')).open('w') as log:
                    result = subprocess.run(['taskset', '-c', str(cpu), str(build / team), str(work / 'input.json'), str(target)],
                                            env=env, stdout=log, stderr=subprocess.STDOUT)
                data = json.loads(target.read_text()) if target.exists() else {'valid': False}
                outputs[team] = data
                row[team] = {'exit': result.returncode, 'valid': data.get('valid'), 'error': data.get('error'),
                             'steps': data.get('steps'), 'solved': data.get('solved'),
                             'preprocess_seconds': data.get('preprocess_seconds'),
                             'total_decision_seconds': sum(data.get('decision_seconds', [])),
                             'max_decision_seconds': max(data.get('decision_seconds', [0])),
                             'matches_unmodified_run': data.get('matches_unmodified_run')}
                behavior = {k: data[k] for k in ['positions', 'orientations', 'targets', 'actions'] if k in data}
                row[team]['trajectory_sha256'] = hashlib.sha256(json.dumps(behavior, sort_keys=True).encode()).hexdigest()
            row['identical'] = all(outputs[t].get('valid') for t in outputs) and row['pibt_reference']['trajectory_sha256'] == row['cgar_reference']['trajectory_sha256']
            if not row['identical']:
                for key in ['positions', 'orientations', 'targets', 'actions']:
                    x, y = outputs['pibt_reference'].get(key, []), outputs['cgar_reference'].get(key, [])
                    for t, (xx, yy) in enumerate(zip(x, y)):
                        if xx != yy:
                            row['first_difference'] = {'field': key, 'step': t, 'upstream': xx, 'cgar': yy}; break
                    if 'first_difference' in row: break
                print('MISMATCH', case['name'], row.get('first_difference'), flush=True)
            rows.append(row)
        return rows
    with concurrent.futures.ThreadPoolExecutor(max_workers=a.jobs) as pool:
        groups = list(pool.map(run_group, [(cpus[k], cases[k::a.jobs]) for k in range(a.jobs)]))
    results = [row for group in groups for row in group]
    write(out / 'results.json', results)
    summary = {'cases': len(results), 'identical': sum(r['identical'] for r in results),
               'upstream_full_run_checks': sum(r['pibt_reference']['matches_unmodified_run'] is True for r in results),
               'agent_decisions': sum(r['robots'] * (r['cgar_reference']['steps'] or 0) for r in results)}
    write(out / 'completion.json', summary); print(json.dumps(summary), flush=True)
    for name, expected in requested.items(): assert digest(ROOT / name) == expected, name
    return int(summary['identical'] != summary['cases'])

if __name__ == '__main__': raise SystemExit(main())
