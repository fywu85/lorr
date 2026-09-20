"""Freeze, minimally patch, build and integration-test KittyKnight on GRID."""
import argparse
import datetime
import difflib
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[3]
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources
from patch_source import apply


def now():
    return datetime.datetime.now(datetime.timezone.utc).isoformat()


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def write(path, value):
    Path(path).write_text(json.dumps(value, indent=2) + '\n')


def run(command, cwd, env):
    print('COMMAND ' + shlex.quote(str(command[0])) + ' (' + str(len(command)) + ' arguments)', flush=True)
    subprocess.run(command, cwd=str(cwd), env=env, check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--execute', action='store_true')
    args = parser.parse_args()
    out = args.output.resolve()
    if not args.execute:
        assert out.parent == ROOT / 'runs', 'Frozen runner root assumes runs/<job>/support'
        out.mkdir(parents=True, exist_ok=False)
        reference = ROOT / 'runs/warehouse-leaders-full-20260918/spec.json'
        old = json.loads(reference.read_text())
        original = Path(old['reference_assets']) / 'kk'
        for name, expected in old['source_hashes']['kk'].items():
            assert sha(original / name) == expected, name
        shutil.copytree(original, out / 'source')
        (out / 'support').mkdir()
        for name in ['build.py', 'patch_source.py', 'scheduler_fixture.cpp', 'config_fixture.cpp']:
            shutil.copy2(HERE / name, out / 'support' / name)
        native = apply(out / 'source')
        (out / 'support/native_scheduler.cpp').write_text(native)
        modified = {}
        patch = []
        for name, expected in old['source_hashes']['kk'].items():
            path = out / 'source' / name
            if sha(path) != expected:
                modified[name] = sha(path)
                patch.extend(difflib.unified_diff((original / name).read_text().splitlines(True),
                    path.read_text().splitlines(True), fromfile='a/' + name, tofile='b/' + name))
        assert set(modified) == {'wppl_planner/src/scheduler.cpp', 'wppl_planner/src/LNS/LNSSolver.cpp', 'src/Entry.cpp'}
        (out / 'source.patch').write_text(''.join(patch))
        sources = {name: sha(out / 'source' / name) for name in old['source_hashes']['kk']}
        # Preserve the established compiler flags; CMake appends Wfatal-errors itself.
        cache = (ROOT / 'kk/build/CMakeCache.txt').read_text().splitlines()
        flags = next(x.split('=', 1)[1] for x in cache if x.startswith('CMAKE_CXX_FLAGS:STRING='))
        if flags.endswith(' -Wfatal-errors'):
            flags = flags[:-len(' -Wfatal-errors')]
        request = {'prepared_utc': now(), 'base_commit': subprocess.check_output(
            ['git', 'rev-parse', 'HEAD'], cwd=str(ROOT), text=True).strip(),
            'reference_spec': str(reference), 'reference_spec_sha256': sha(reference),
            'reference_binary_sha256': old['binary_sha256']['kk'],
            'native_source_hashes': old['source_hashes']['kk'], 'sources': sources,
            'changed_sources': modified, 'compiler_flags': flags,
            'support_hashes': {p.name: sha(p) for p in (out / 'support').iterdir() if p.is_file()},
            'scope': 'Competitor diagnostic only; no CGAR or archive source modified.'}
        write(out / 'requested.json', request)
        command = ['/usr/bin/python3', str(out / 'support/build.py'), '--execute', '--output', str(out)]
        script = out / 'job.sh'
        script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q',
            '-pe', 'threaded', '4', '-binding', 'linear:4', '-l', 'exclusive=true,h_rt=00:30:00,h_vmem=4G',
            '-m', 'n', '-N', 'kk_diag_build', '-j', 'y', '-o', str(out / 'build.log'), '-S', '/bin/bash', str(script)]
        proc = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(out / 'submission.json', {'command': submit, 'returncode': proc.returncode, 'response': proc.stdout})
        print(proc.stdout, end='')
        return proc.returncode
    request = json.loads((out / 'requested.json').read_text())
    def verify_sources():
        for name, expected in request['sources'].items():
            assert sha(out / 'source' / name) == expected, name
        for name, expected in request['support_hashes'].items():
            assert sha(out / 'support' / name) == expected, name
    verify_sources()
    allocation = cpu_resources()
    assert allocation['physical_cores_visible'] >= 4 and allocation['effective_cpu_quota'] is None
    os.sched_setaffinity(0, allocation['representative_cpus'][:4])
    write(out / 'allocation.json', {'before': allocation, 'bound': cpu_resources(), 'started_utc': now()})
    env = {k: v for k, v in os.environ.items() if not k.startswith(('CGAR_', 'LORR_KK_'))}
    env.update(PATH=str(ROOT / 'env/bin') + ':' + env['PATH'], OMP_NUM_THREADS='1', OMP_DYNAMIC='FALSE')
    compiler = str(ROOT / 'env/bin/x86_64-conda-linux-gnu-c++')
    build = out / 'build'
    run([str(ROOT / 'env/bin/cmake'), '-S', str(out / 'source'), '-B', str(build),
        '-DCMAKE_BUILD_TYPE=Release', '-DCMAKE_CXX_COMPILER=' + compiler,
        '-DCMAKE_PREFIX_PATH=' + str(ROOT / 'env'), '-DPYTHON_EXECUTABLE=' + str(ROOT / 'env/bin/python'),
        '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON', '-DCMAKE_CXX_FLAGS=' + request['compiler_flags'], '-DPYTHON=OFF'], ROOT, env)
    run([str(ROOT / 'env/bin/cmake'), '--build', str(build), '--target', 'lifelong', '-j4'], ROOT, env)
    commands = json.loads((build / 'compile_commands.json').read_text())
    def compile_like(suffix, source, target):
        entries = [c for c in commands if c['file'].endswith(suffix)]
        assert len(entries) == 1, suffix
        entry = entries[0]
        command = entry.get('arguments') or shlex.split(entry['command'])
        command[command.index('-o') + 1] = str(target)
        assert command.count(entry['file']) == 1
        command = [str(source) if x == entry['file'] else x for x in command]
        run(command, entry['directory'], env)
    fixture_object = out / 'scheduler_fixture.o'
    native_object = out / 'native_scheduler.o'
    config_object = out / 'config_fixture.o'
    compile_like('/src/driver.cpp', out / 'support/config_fixture.cpp', config_object)
    compile_like('/src/driver.cpp', out / 'support/scheduler_fixture.cpp', fixture_object)
    compile_like('/wppl_planner/src/scheduler.cpp', out / 'support/native_scheduler.cpp', native_object)
    link = shlex.split((build / 'CMakeFiles/lifelong.dir/link.txt').read_text())
    driver_objs = [x for x in link if x.endswith('/src/driver.cpp.o')]
    scheduler_objs = [x for x in link if x.endswith('/wppl_planner/src/scheduler.cpp.o')]
    assert len(driver_objs) == len(scheduler_objs) == 1
    for kind in ['native', 'patched', 'config']:
        object_file = config_object if kind == 'config' else fixture_object
        command = [str(object_file) if x == driver_objs[0] else x for x in link]
        if kind == 'native':
            command = [str(native_object) if x == scheduler_objs[0] else x for x in command]
        command[command.index('-o') + 1] = str(out / ('fixture-' + kind))
        run(command, build, env)
    cases = [('native', None, '1'), ('native', '0', '1'),
             ('patched', None, '1'), ('patched', '1', '1'), ('patched', '0', '0')]
    records = []
    for weighted in ['0', '1']:
        offset = len(records)
        for kind, mode, expected in cases:
            case_env = env.copy()
            if mode is not None:
                case_env['LORR_KK_REMATCH_UNOPENED'] = mode
            proc = subprocess.run([str(out / ('fixture-' + kind)), expected, weighted], cwd=str(out), env=case_env,
                                  stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, check=True)
            lines = [line[len('KK_FIXTURE '):] for line in proc.stdout.splitlines() if line.startswith('KK_FIXTURE ')]
            assert len(lines) == 1
            records.append({'kind': kind, 'mode': mode, 'expected_rematch': expected, 'nonuniform_costs': weighted,
                            'cases': json.loads(lines[0])})
        assert all(records[offset + i]['cases'] == records[offset]['cases'] for i in [1, 2, 3])
        assert records[offset + 4]['cases'] != records[offset]['cases']
    supplied = json.loads((out / 'source/wppl_planner/configs/warehouse_large.json').read_text())
    uniform = json.loads(json.dumps(supplied))
    for item in uniform['map_weights_path']: item['value'] = ''
    malformed = json.loads(json.dumps(supplied)); malformed['map_weights_path'] = ''
    paths = []
    for name, config in [('supplied', supplied), ('uniform', uniform), ('malformed', malformed)]:
        path = out / (name + '-config.json'); write(path, config); paths.append(str(path))
    proc = subprocess.run([str(out / 'fixture-config')] + paths, cwd=str(out), env=env,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, check=True)
    lines = [line[len('KK_CONFIG_FIXTURE '):] for line in proc.stdout.splitlines() if line.startswith('KK_CONFIG_FIXTURE ')]
    assert len(lines) == 1
    config_checks = json.loads(lines[0])
    invalid_env = env.copy(); invalid_env['LORR_KK_REMATCH_UNOPENED'] = 'invalid'
    proc = subprocess.run([str(out / 'fixture-patched'), '1', '0'], cwd=str(out), env=invalid_env,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    assert proc.returncode != 0 and 'LORR_KK_REMATCH_UNOPENED must be 0 or 1' in proc.stdout
    write(out / 'fixture-results.json', {'completed_utc': now(), 'records': records,
        'native_and_enabled_identical': True, 'disabled_preserves_unopened_and_started': True,
        'free_robots_assigned_unique_tasks': True, 'invalid_option_rejected': True,
        'task_progress_and_reveal_time_unchanged': True, 'real_task_manager_lifecycle': True,
        'nonuniform_costs_checked': True, 'configuration_loader': config_checks})
    verify_sources()
    shutil.copy2(build / 'lifelong', out / 'lifelong')
    write(out / 'build.json', {'completed_utc': now(), 'requested_sha256': sha(out / 'requested.json'),
        'sources': request['sources'], 'changed_sources': request['changed_sources'],
        'binary_sha256': sha(out / 'lifelong'), 'compiler': compiler,
        'fixture_source_sha256': sha(out / 'support/scheduler_fixture.cpp'),
        'fixture_results_sha256': sha(out / 'fixture-results.json'),
        'scope': request['scope']})
    print('KittyKnight native/ablated real-scheduler integration fixtures passed', flush=True)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
