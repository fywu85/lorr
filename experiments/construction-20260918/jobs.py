#!/usr/bin/env python3
"""Frozen NMS construction ablations on exclusive GRID allocations."""
import argparse
import concurrent.futures
import difflib
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
MOTION = ROOT / 'experiments/motion-20260918'
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources
from instrument import instrument, snapshot_driver


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def write(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2) + '\n')


def submit(out, command, name, slots, cores, memory, array=None, hold=None):
    script = out / (name + '.sh')
    script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
    cmd = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q',
           '-pe', 'threaded', str(slots), '-binding', 'linear:' + str(cores), '-l',
           f'exclusive=true,h_rt=01:00:00,h_vmem={memory}G', '-m', 'n', '-N', name,
           '-j', 'y', '-o', str(out), '-S', '/bin/bash']
    if array:
        cmd += ['-t', '1-' + str(array), '-tc', '8']
    if hold:
        cmd += ['-hold_jid', hold]
    run = subprocess.run(cmd + [str(script)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    write(out / (name + '-submission.json'), {'command': cmd + [str(script)], 'returncode': run.returncode, 'response': run.stdout})
    print(run.stdout, end='')
    return run.returncode


def flags_for(team):
    build = (ROOT / team / 'build').resolve()
    flags = {}
    for line in (build / 'CMakeFiles/lifelong.dir/flags.make').read_text().splitlines():
        if ' = ' in line:
            key, value = line.split(' = ', 1)
            flags[key] = shlex.split(value)
    return build, flags, shlex.split((build / 'CMakeFiles/lifelong.dir/link.txt').read_text())


def freeze(out):
    sources = out / 'sources/nms'
    for folder in ['inc', 'src', 'default_planner', 'Solution', 'Solution2']:
        if (ROOT / 'nms' / folder).exists():
            shutil.copytree(ROOT / 'nms' / folder, sources / folder,
                            ignore=shutil.ignore_patterns('build', '*.o', '__pycache__'))
    before = {str(p.relative_to(sources)): p.read_text() for p in sources.rglob('*')
              if p.is_file() and p.suffix in ['.hpp', '.cpp', '.h']}
    instrument(sources)
    shutil.copy2(MOTION / 'nms_uniform_environment.cpp', sources / 'Solution/Objects/Environment/environment.cpp')
    (out / 'snapshot_driver.cpp').write_text(snapshot_driver(MOTION / 'snapshot_driver.cpp'))
    changes = []
    for p in sorted(sources.rglob('*')):
        if p.is_file() and p.suffix in ['.hpp', '.cpp', '.h']:
            name = str(p.relative_to(sources)); old = before.get(name, ''); new = p.read_text()
            if old != new:
                changes.extend(difflib.unified_diff(old.splitlines(True), new.splitlines(True), 'a/' + name, 'b/' + name))
    (out / 'nms-instrumentation.patch').write_text(''.join(changes))
    write(out / 'requested.json', {
        'commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
        'sources': {str(p.relative_to(out)): sha(p) for p in sources.rglob('*') if p.is_file()},
        'driver_sha256': sha(out / 'snapshot_driver.cpp'),
        'scripts': {p.name: sha(p) for p in HERE.glob('*.py')},
    })


def extract_snapshots(out):
    # The source trajectories are full valid runs. Preserve goal semantics used
    # in the existing independently checked common-state probe.
    config = next((ROOT / 'mr24').glob('*/WAREHOUSE.json'))
    inp = json.loads(config.read_text()); mp = config.parent / inp['mapFile']
    lines = mp.read_text().splitlines(); rows = int(lines[1].split()[1]); cols = int(lines[2].split()[1])
    grid = [int(c in '@T') for line in lines[4:] for c in line]
    cases = []
    sources = [
        ('cgar', ROOT / 'runs/cgar-warehouse-final-full-20260918/orientation_8192-s0-r0/WAREHOUSE.json', [100, 4000]),
        ('kittyknight', ROOT / 'runs/warehouse-leaders-full-20260918/kk-WAREHOUSE/result.json', [4000]),
    ]
    for label, source, times in sources:
        data = json.loads(source.read_text()); source_sha = sha(source)
        assert not any(data[k] for k in ['numPlannerErrors', 'numScheduleErrors', 'numEntryTimeouts'])
        tasks = {i: [cells[k] * cols + cells[k + 1] for k in range(0, len(cells), 2)] for i, _, cells in data['tasks']}
        for t in times:
            states = []
            for i, (r, c, o) in enumerate(data['start']):
                pos = r * cols + c; direction = 'ESWN'.index(o)
                for action in data['actualPaths'][i][:2 * t:2]:
                    if action == 'F': pos += [1, cols, -1, -cols][direction]
                    elif action == 'R': direction = (direction + 1) % 4
                    elif action == 'C': direction = (direction + 3) % 4
                states.append([pos, direction])
            assert len({s[0] for s in states}) == len(states)
            assert all(not grid[s[0]] for s in states)
            stops = {}
            for at, robot, task, stop in data['events']:
                if at <= t: stops[task] = stop
            goals = []
            for entries in data['actualSchedule']:
                task = -1
                for entry in entries.split(','):
                    if not entry: continue
                    at, tid = map(int, entry.split(':'))
                    if at > t + 1: break
                    task = tid
                stop = stops.get(task, 0)
                goals.append(-1 if task < 0 or stop >= len(tasks[task]) else tasks[task][stop])
            name = f'{label}-t{t}'
            dest = out / 'snapshots' / (name + '.json'); dest.parent.mkdir(exist_ok=True)
            value = {'instance': 'WAREHOUSE', 'source_timestep': t, 'source': str(source.relative_to(ROOT)),
                     'source_sha256': source_sha, 'map_name': mp.name, 'rows': rows, 'cols': cols,
                     'map': grid, 'states': states, 'goals': goals,
                     'contract': 'Only current goals retained; no later stops or reassignment. Reached goals retired, agents remain movable.'}
            dest.write_text(json.dumps(value, separators=(',', ':')) + '\n')
            cases.append({'name': name, 'snapshot': str(dest), 'sha256': sha(dest), 'source_sha256': source_sha})
    write(out / 'snapshots/manifest.json', cases)


def build(out):
    write(out / 'allocation.json', cpu_resources())
    spec = json.loads((out / 'requested.json').read_text())
    for name, digest in spec['sources'].items(): assert sha(out / name) == digest, name
    assert sha(out / 'snapshot_driver.cpp') == spec['driver_sha256']
    b, flags, link = flags_for('nms'); source = out / 'sources/nms'
    old_source = (ROOT / 'nms').resolve()
    includes = [x.replace(str(old_source), str(source)) for x in flags['CXX_INCLUDES']]
    common = [link[0]] + flags['CXX_DEFINES'] + includes + flags['CXX_FLAGS']
    objects = {}; commands = []
    for arg in link:
        if not arg.endswith('.o') or arg.endswith('/src/driver.cpp.o'): continue
        relative = arg.split('CMakeFiles/lifelong.dir/')[1][:-2]
        obj = out / 'objects' / (relative + '.o'); obj.parent.mkdir(parents=True, exist_ok=True)
        command = common + ['-c', str(source / relative), '-o', str(obj)]
        objects[arg] = str(obj); commands.append(command)
    def compile_one(command):
        log = Path(command[-1] + '.log')
        with log.open('w') as stream:
            subprocess.run(command, cwd=out, stdout=stream, stderr=subprocess.STDOUT, check=True)
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        list(pool.map(compile_one, commands))
    obj = out / 'nms-snapshot.o'
    snapshot_compile = common + ['-DSNAPSHOT_NMS=1', '-c', str(out / 'snapshot_driver.cpp'), '-o', str(obj)]
    compile_one(snapshot_compile)
    command = [str(obj) if x.endswith('/src/driver.cpp.o') else objects.get(x, str(out / 'nms-snapshot') if x == 'lifelong' else x) for x in link]
    subprocess.run(command, cwd=b, check=True)
    nms_link = command
    # CGAR uses the verified production object files; record each hash.
    cb, cf, cl = flags_for('cgar'); cobj = out / 'cgar-snapshot.o'
    ccompile = [cl[0]] + cf['CXX_DEFINES'] + cf['CXX_INCLUDES'] + cf['CXX_FLAGS'] + ['-c', str(out / 'snapshot_driver.cpp'), '-o', str(cobj)]
    compile_one(ccompile)
    cobjects = {}
    command = []
    for x in cl:
        if x.endswith('/src/driver.cpp.o'): command.append(str(cobj))
        elif x.endswith('.o'):
            objpath = cb / x; cobjects[str(objpath)] = sha(objpath); command.append(str(objpath))
        elif x == 'lifelong': command.append(str(out / 'cgar-snapshot'))
        else: command.append(x)
    subprocess.run(command, cwd=cb, check=True)
    extract_snapshots(out)
    write(out / 'build.json', {'nms_binary_sha256': sha(out / 'nms-snapshot'), 'cgar_binary_sha256': sha(out / 'cgar-snapshot'),
          'nms_compile': commands + [snapshot_compile], 'nms_link': nms_link, 'cgar_compile': ccompile, 'cgar_link': command,
          'cgar_objects': cobjects, 'nms_instrumentation_sha256': sha(out / 'nms-instrumentation.patch'), 'valid': True})


PROFILES = {
    'cgar_cache': {'team': 'cgar', 'one_core': True, 'env': {'CGAR_ORIENTATION_GUIDANCE': '1', 'CGAR_TURN_FIRST': '1', 'CGAR_TURN_TABLE_MB': '8192'}},
    'nms_build_1': {'team': 'nms', 'one_core': True, 'env': {'NMS_HELPERS': '1', 'NMS_WORKERS': '1', 'NMS_STEPS': '0'}},
    'nms_search_1': {'team': 'nms', 'one_core': True, 'env': {'NMS_HELPERS': '1', 'NMS_WORKERS': '1'}},
    'nms_build_32': {'team': 'nms', 'one_core': False, 'env': {'NMS_HELPERS': '32', 'NMS_WORKERS': '32', 'NMS_STEPS': '0'}},
    'nms_search_32': {'team': 'nms', 'one_core': False, 'env': {'NMS_HELPERS': '32', 'NMS_WORKERS': '32'}},
}


def execute(out):
    spec = json.loads((out / 'spec.json').read_text())
    case = spec['cases'][int(os.environ['SGE_TASK_ID']) - 1]
    work = out / case['name']; work.mkdir()
    resources = cpu_resources(); write(work / 'allocation.json', resources)
    assert resources['physical_cores_visible'] == 16 and len(resources['logical_cpu_affinity']) == 32, resources
    assert resources['effective_cpu_quota'] is None or resources['effective_cpu_quota'] >= 32, resources
    build_dir = Path(spec['build_dir']); manifest = json.loads((build_dir / 'build.json').read_text())
    assert manifest['valid'] and sha(Path(case['snapshot'])) == case['sha256']
    profile = spec['profiles'][case['profile']]; team = profile['team']; binary = build_dir / (team + '-snapshot')
    assert sha(binary) == manifest[team + '_binary_sha256']
    env = {k: v for k, v in os.environ.items() if not k.startswith(('CGAR_', 'NMS_', 'PROBE_'))}
    env.update({'CGAR_SEED': '0', 'NMS_SEED': '202', 'NMS_PREPROCESS_THREADS': '32', 'OMP_NUM_THREADS': '32', 'OMP_DYNAMIC': 'FALSE'})
    env.update(profile['env'])
    if profile['one_core']: env['PROBE_PLAN_CPU'] = str(resources['representative_cpus'][0])
    command = [sys.executable, str(ROOT / 'tools/run_with_usage.py'), '--output', str(work / 'usage.json'), '--',
               str(binary), case['snapshot'], str(work / 'result.json'), str(spec['steps']), str(spec['limit_ms'])]
    write(work / 'command.json', {'command': command, 'environment': {k: v for k, v in env.items() if k.startswith(('CGAR_', 'NMS_', 'PROBE_', 'OMP_'))}})
    with (work / 'run.log').open('w') as log:
        run = subprocess.run(command, cwd=work, env=env, stdout=log, stderr=subprocess.STDOUT)
    write(work / 'completion.json', {'exit': run.returncode})
    return run.returncode


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('mode', choices=['build', 'execute-build', 'run', 'execute-run'])
    p.add_argument('--output', required=True, type=Path)
    p.add_argument('--build-dir', type=Path)
    p.add_argument('--steps', type=int, default=100)
    p.add_argument('--limit-ms', type=int, default=1000)
    p.add_argument('--profiles', nargs='+', choices=PROFILES, default=list(PROFILES))
    a = p.parse_args(); out = a.output.resolve()
    if a.mode == 'execute-build': build(out); return 0
    if a.mode == 'execute-run': return execute(out)
    out.mkdir(parents=True, exist_ok=False)
    command = [sys.executable, str(Path(__file__).resolve()), 'execute-' + a.mode, '--output', str(out)]
    if a.mode == 'build':
        freeze(out)
        return submit(out, command, 'nms_ablate_build', 4, 4, 4)
    if not a.build_dir: p.error('--build-dir required')
    build_dir = a.build_dir.resolve()
    snapshots = json.loads((build_dir / 'snapshots/manifest.json').read_text())
    cases = [dict(s, name=s['name'] + '-' + profile, profile=profile) for s in snapshots for profile in a.profiles]
    write(out / 'spec.json', {'build_dir': str(build_dir), 'cases': cases, 'profiles': {k: PROFILES[k] for k in a.profiles}, 'steps': a.steps, 'limit_ms': a.limit_ms})
    return submit(out, command, 'nms_ablate_probe', 32, 16, 2, array=len(cases))


if __name__ == '__main__':
    raise SystemExit(main())
