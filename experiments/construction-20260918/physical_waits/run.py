#!/usr/bin/env python3
"""Freeze and submit an independent physical-wait audit of validated full runs."""
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

ROOT = next(p for p in Path(__file__).resolve().parents if (p / 'tools/cpu_resources.py').is_file())
BASE = ROOT / 'experiments/construction-20260918'


def read(p):
    return json.loads(p.read_text())


def write(p, d):
    p.write_text(json.dumps(d, indent=2) + '\n')


def digest(p):
    h = hashlib.sha256()
    with p.open('rb') as f:
        for b in iter(lambda: f.read(4 * 1024 * 1024), b''):
            h.update(b)
    return h.hexdigest()


def fixtures(raw, binary):
    m = raw / 'fixture.map'
    m.write_text('type octile\nheight 2\nwidth 5\nmap\n.....\n.....\n')
    reports = {}

    def run(label, starts, paths, goals, events=(), invalid=None):
        d = dict(teamSize=len(starts), makespan=len(paths[0]), numTaskFinished=sum(e[3] == len(goals[e[2]]) // 2 for e in events),
                 numPlannerErrors=0, numScheduleErrors=0, numEntryTimeouts=0, start=starts,
                 actualPaths=[','.join(p) for p in paths], actualSchedule=['1:' + str(i) for i in range(len(starts))],
                 tasks=[[i, 0, g] for i, g in enumerate(goals)], events=list(events))
        src, dst = raw / (label + '.json'), raw / (label + '-result.json')
        write(src, d)
        r = subprocess.run([str(binary), str(m), str(src), str(dst)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        if invalid:
            assert r.returncode and invalid in r.stdout, (label, r.returncode, r.stdout)
            reports[label] = {'rejected': True, 'reason': invalid}
            return None
        assert r.returncode == 0, (label, r.stdout)
        result = read(dst)
        reports[label] = result
        return result['cohorts']['1']

    r = run('repeated_goal_service', [[0, 0, 'E']], ['WW'], [[0, 0, 0, 0]], [[1, 0, 0, 1], [2, 0, 0, 2]])
    assert r['at_current_goal'] == r['wait_actions'] == 2
    assert r['wait_phases_idle_empty_loaded'] == [0, 1, 1]
    r = run('free_forward', [[0, 0, 'E']], ['WFF'], [[0, 2]], [[3, 0, 0, 1]])
    assert r['free_ahead_after_other_actions'] == r['free_and_manhattan_closer'] == 1
    r = run('stationary_blocker', [[0, 0, 'E'], [0, 1, 'E']], ['WW', 'WW'], [[0, 3], [0, 4]])
    assert r['occupied_ahead_after_other_actions'] == r['blocker_waiting'] == 2
    assert r['free_ahead_after_other_actions'] == 2
    r = run('vacated_forward', [[0, 0, 'E'], [0, 1, 'E']], ['WF', 'FF'], [[0, 1], [0, 3]], [[2, 0, 0, 1], [2, 1, 1, 1]])
    assert r['free_ahead_after_other_actions'] == r['ahead_vacated_by_other_action'] == 1
    r = run('incoming_blocker', [[0, 0, 'E'], [1, 1, 'N']], ['WW', 'FW'], [[0, 3], [0, 3]])
    assert r['incoming_blocker'] == r['blocker_moving'] == 1
    assert r['occupied_ahead_after_other_actions'] == 2 and r['obstacle_ahead'] == 1
    r = run('completed_waiter_unfinished_blocker', [[0, 0, 'E'], [0, 1, 'E']], ['WFF', 'WFF'], [[0, 2], [0, 4]], [[3, 0, 0, 1]])
    assert r['completed_waiter_unfinished_blocker'] == r['completed_task_waiter'] == 1
    run('reject_edge_swap', [[0, 0, 'E'], [0, 1, 'W']], ['F', 'F'], [[0, 4], [1, 4]], invalid='edge collision')
    run('reject_vertex_collision', [[0, 0, 'E'], [0, 2, 'W']], ['F', 'F'], [[0, 4], [1, 4]], invalid='vertex or obstacle collision')
    run('reject_missing_service', [[0, 0, 'E']], ['F'], [[0, 1]], invalid='service event missing or spurious')
    return reports


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--raw', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--reference', type=Path, default=BASE / 'results/p90-pickup-combined-v99-outcomes/accounting.json')
    p.add_argument('--work-budget', type=Path, default=BASE / 'results/p90-pickup-combined-v99/work-budget.json')
    p.add_argument('--execute', action='store_true')
    a = p.parse_args()
    raw, out = a.raw.resolve(), a.output.resolve()
    if not a.execute:
        ref, budget = read(a.reference), read(a.work_budget)
        assert ref['all_valid'] and ref['all_cases_completed'] and budget['all_valid']
        names = ['trick_p90_pickup8-s0-r0', 'trick_p90_pickup8-s2-r0']
        cases = {k: ref['reports'][k] for k in names}
        raw.mkdir(parents=True, exist_ok=False)
        files = []
        for src, name in [(Path(__file__), 'run.py'), (Path(__file__).with_name('audit.cpp'), 'audit.cpp'),
                          (a.reference, 'reference.json'), (a.work_budget, 'work-budget.json')]:
            dest = raw / name
            shutil.copy2(src, dest)
            files.append(dest)
        tracked = [ROOT / 'mr24/warehouse.domain/maps/warehouse_large.map', ROOT / 'cgar/inc/nlohmann/json.hpp', ROOT / 'tools/cpu_resources.py']
        write(raw / 'request.json', dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
              root=str(ROOT), output=str(out), cases=cases, files={str(f): digest(f) for f in files + tracked},
              limits='Read-only saved-trajectory replay; future outcomes label observations only. No production mutation or new solver score.'))
        command = ['/usr/bin/python3', str(raw / 'run.py'), '--raw', str(raw), '--output', str(out), '--execute']
        job = raw / 'job.sh'
        job.write_text('#!/bin/bash\nset -eu\nulimit -c 0\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        submit = ['qsub', '-h', '-terse', '-w', 'n', '-cwd', '-q', 'debian.q@research44.grid.gsb,debian.q@research57.grid.gsb', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=false,h_rt=00:30:00,h_vmem=8G', '-m', 'n',
                  '-N', 'physical_wait_audit', '-j', 'y', '-o', str(raw / 'job.log'), '-S', '/bin/bash', str(job)]
        r = subprocess.run(submit, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(raw / 'submission.json', dict(command=submit, returncode=r.returncode, response=r.stdout))
        r.check_returncode()
        assert r.stdout.strip().isdigit()
        print(r.stdout, end='')
        subprocess.run(['qrls', r.stdout.strip()], check=True)
        return
    request = read(raw / 'request.json')
    assert request['root'] == str(ROOT) and request['output'] == str(out)
    for name, sha in request['files'].items():
        assert digest(Path(name)) == sha, name
    sys.path.insert(0, str(ROOT / 'tools'))
    from cpu_resources import cpu_resources
    cpu = cpu_resources()
    assert cpu['effective_cpu_quota'] is None
    write(raw / 'allocation.json', cpu)
    assert cpu['physical_cores_visible'] == 1, 'GRID binding was not applied exactly'
    os.sched_setaffinity(0, cpu['representative_cpus'][:1])
    selected = cpu_resources()
    assert selected['physical_cores_visible'] == 1
    out.mkdir(parents=True, exist_ok=False)
    binary = raw / 'audit'
    compiler = ROOT / 'env/bin/x86_64-conda-linux-gnu-c++'
    command = [str(compiler), '-std=c++17', '-O2', '-I', str(ROOT / 'cgar/inc'), str(raw / 'audit.cpp'),
               '-Wl,-rpath,' + str(ROOT / 'env/lib'), '-o', str(binary)]
    subprocess.run(command, check=True)
    write(out / 'fixtures.json', fixtures(raw, binary))
    work = {d['case']: d for d in read(raw / 'work-budget.json')['rows']}
    reports = {}
    for name, ref in request['cases'].items():
        path = Path(ref['input_path'])
        assert digest(path) == ref['input_sha256']
        dest = raw / (name + '.json')
        subprocess.run([str(binary), str(ROOT / 'mr24/warehouse.domain/maps/warehouse_large.map'), str(path), str(dest)], check=True)
        d = read(dest)
        assert d['tasks_completed'] == ref['completed'] and d['steps'] == 5000 and d['robots'] == 10000
        assert d['physical_actions_checked'] == ref['all_robot_steps'] == 50000000
        assert d['phase_steps_idle_empty_loaded'] == [ref['phase_steps'][p] for p in ['idle', 'empty', 'loaded']]
        c = d['cohorts']['1']
        assert c['wait_actions'] == work[name]['actions']['wait']
        assert c['wait_phases_idle_empty_loaded'] == [work[name]['phases'][p]['actions']['wait'] for p in ['idle', 'empty', 'loaded']]
        assert digest(path) == ref['input_sha256']
        reports[name] = dict(input_path=str(path), input_sha256=ref['input_sha256'],
                             trajectory_sha256=ref['trajectory_sha256'], tasks=ref['completed'], audit=d)
        shutil.copy2(dest, out / dest.name)
        print('PHYSICAL_WAIT_AUDIT_VERIFIED', name, json.dumps(d['cohorts']), flush=True)
    for name, sha in request['files'].items():
        assert digest(Path(name)) == sha, name
    for name in ['request.json', 'submission.json', 'reference.json', 'work-budget.json']:
        shutil.copy2(raw / name, out / name)
    write(out / 'accounting.json', dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
          all_valid=True, all_cases_completed=True, independent_fixtures=9, reports=reports,
          binary_sha256=digest(binary), compiler_command=command, allocation=cpu, bound_allocation=selected,
          job_id=os.environ['JOB_ID']))


if __name__ == '__main__':
    main()
