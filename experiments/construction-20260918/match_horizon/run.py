#!/usr/bin/env python3
"""Freeze and submit a read-only late-rematching audit of validated full runs."""
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


def write(p, x):
    p.write_text(json.dumps(x, indent=2) + '\n')


def digest(p):
    h = hashlib.sha256()
    with p.open('rb') as f:
        for b in iter(lambda: f.read(4 * 1024 * 1024), b''):
            h.update(b)
    return h.hexdigest()


def fixtures(raw, binary):
    m = raw / 'rematch-fixture.map'
    m.write_text('type octile\nheight 2\nwidth 10\nmap\n..........\n..........\n')
    base = dict(teamSize=2, makespan=5, numTaskFinished=0,
                numPlannerErrors=0, numScheduleErrors=0, numEntryTimeouts=0,
                start=[[0, 0, 'E'], [0, 8, 'W']], actualPaths=['W,W,W,W,W'] * 2,
                actualSchedule=['1:0,3:1', '1:1,3:0'],
                tasks=[[0, 0, [0, 1, 0, 2]], [1, 0, [0, 7, 0, 6]]], events=[])
    reports = {}

    def run(label, d):
        src, dst = raw / (label + '.json'), raw / (label + '-result.json')
        write(src, d)
        subprocess.run([str(binary), str(m), str(src), str(dst)], check=True)
        r = read(dst)['rematch_cutoff_audit']
        reports[label] = r
        return r

    r = run('worse', base)
    assert r['cohorts']['1']['physical_feasible_to_impossible'] == 2
    assert r['cohorts']['1']['physical_pickup_distance_change_sum'] == 12
    rescue = json.loads(json.dumps(base))
    rescue['actualSchedule'] = ['1:1,3:0', '1:0,3:1']
    r = run('rescue', rescue)
    assert r['cohorts']['1']['physical_impossible_to_feasible'] == 2
    assert r['cohorts']['1']['physical_pickup_distance_change_sum'] == -12
    moved = json.loads(json.dumps(base))
    moved['actualPaths'][0] = 'F,W,W,W,W'
    moved['tasks'][0][2] = [0, 2, 0, 3]
    r = run('current_old_holder', moved)
    x = next(x for x in r['examples'] if x['task'] == 0)
    assert x['old_cell'] == 1 and x['old_pickup'] == 1 and x['new_pickup'] == 6
    assert r['cohorts']['1']['physical_pickup_distance_change_sum'] == 10
    repeated = json.loads(json.dumps(base))
    repeated['actualSchedule'] = ['1:0,4:1', '1:1,4:0']
    repeated['tasks'] = [[0, 0, [0, 1, 0, 1]], [1, 0, [0, 7, 0, 7]]]
    r = run('repeated_stops_inclusive', repeated)
    assert all(x['old_bound'] == 2 and x['remaining'] == 2 and x['chain_service_bound'] == 1
               for x in r['examples'])
    assert r['cohorts']['1']['physical_feasible_to_impossible'] == 2
    for label in ['observed', 'same_tick_unobserved', 'retargeted_excluded']:
        d = json.loads(json.dumps(repeated))
        d['teamSize'] = 3
        d['numTaskFinished'] = 1
        d['start'].append([1, 0, 'E'])
        d['actualPaths'].append('W,W,F,W,W')
        d['actualSchedule'].append('1:2,4:-1')
        d['tasks'].append([2, 0, [1, 1]])
        d['events'] = [[3, 2, 2, 1]]
        if label == 'same_tick_unobserved':
            d['actualPaths'][2] = 'W,W,W,F,W'
            d['actualSchedule'][2] = '1:2,5:-1'
            d['events'][0][0] = 4
        if label == 'retargeted_excluded':
            d['teamSize'] = 4
            d['actualPaths'][2] = 'W,W,W,W,W'
            d['actualSchedule'][2] = '1:2,2:-1'
            d['start'].append([1, 2, 'W'])
            d['actualPaths'].append('W,W,F,W,W')
            d['actualSchedule'].append('0:-1,2:2')
            d['events'][0][1] = 3
        r = run(label, d)
        x = next(x for x in r['examples'] if x['task'] == 0)
        assert x['old_diagnostic_tier'] == int(label == 'observed')
        assert x['new_diagnostic_tier'] == 2
        assert x['old_bucket_count'] == int(label == 'observed')
        assert x['old_margin'] == (2 if label == 'observed' else 0)
    return reports


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--raw', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--reference', type=Path, default=BASE / 'results/p90-pickup-combined-v99-outcomes/accounting.json')
    p.add_argument('--execute', action='store_true')
    a = p.parse_args()
    raw, out = a.raw.resolve(), a.output.resolve()
    if not a.execute:
        ref = read(a.reference)
        assert ref['all_valid'] and ref['all_cases_completed']
        cases = {k: v for k, v in ref['reports'].items() if k in ['trick_p90_pickup8-s0-r0', 'trick_p90_pickup8-s2-r0']}
        assert len(cases) == 2
        raw.mkdir(parents=True, exist_ok=False)
        files = []
        for src, name in [(Path(__file__), 'run.py'), (Path(__file__).with_name('augment.py'), 'augment.py'),
                          (Path(__file__).with_name('report.cpp.inc'), 'report.cpp.inc'),
                          (BASE / 'decompose.py', 'decompose.py'), (BASE / 'decompose.cpp', 'baseline.cpp'),
                          (a.reference, 'reference.json')]:
            dest = raw / name
            shutil.copy2(src, dest)
            files.append(dest)
        sys.path.insert(0, str(raw))
        from augment import augment
        cpp = raw / 'decompose.cpp'
        cpp.write_text(augment((raw / 'baseline.cpp').read_text(), (raw / 'report.cpp.inc').read_text()))
        files.append(cpp)
        tracked = [ROOT / 'mr24/warehouse.domain/maps/warehouse_large.map', ROOT / 'cgar/inc/nlohmann/json.hpp', ROOT / 'tools/cpu_resources.py']
        write(raw / 'request.json', dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
              root=str(ROOT), output=str(out), cases=cases, files={str(f): digest(f) for f in files + tracked},
              limits='Read-only saved-trajectory analysis; no production mutation or new solver score.'))
        command = ['/usr/bin/python3', str(raw / 'run.py'), '--raw', str(raw), '--output', str(out), '--execute']
        job = raw / 'job.sh'
        job.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        submit = ['qsub', '-h', '-terse', '-w', 'n', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=false,h_rt=00:30:00,h_vmem=8G', '-m', 'n',
                  '-N', 'rematch_cutoff_audit', '-j', 'y', '-o', str(raw / 'job.log'), '-S', '/bin/bash', str(job)]
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
    os.sched_setaffinity(0, cpu['representative_cpus'][:1])
    out.mkdir(parents=True, exist_ok=False)
    replay = raw / 'replay'
    write(raw / 'fixture-cases.json', {'extra_loaded_fixture': str(replay / 'loaded-fixture.json')})
    subprocess.run(['/usr/bin/python3', str(raw / 'decompose.py'), '--output', str(replay),
                    '--cases', str(raw / 'fixture-cases.json'), '--map', str(replay / 'fixture.map')], check=True)
    binary = replay / 'decompose'
    write(out / 'fixtures.json', fixtures(raw, binary))
    reports = {}
    for name, ref in request['cases'].items():
        path = Path(ref['input_path'])
        assert digest(path) == ref['input_sha256']
        dest = raw / (name + '.json')
        subprocess.run([str(binary), str(ROOT / 'mr24/warehouse.domain/maps/warehouse_large.map'), str(path), str(dest)], check=True)
        d = read(dest)
        assert d['completed_tasks'] == ref['completed'] and d['steps'] == 5000 and d['team_size'] == 10000
        assert not any(d[k] for k in ['planner_errors', 'schedule_errors', 'entry_timeouts', 'assignments_after_pickup'])
        assert d['empty_robot_steps_including_reassignments'] == ref['phase_steps']['empty']
        assert d['loaded_robot_steps_including_unfinished_tasks'] == ref['phase_steps']['loaded']
        r = d['rematch_cutoff_audit']
        c = r['cohorts']['1']
        assert c['observed_retargets'] == d['tasks_with_multiple_assignments']
        assert c['observed_retargets'] == c['completed'] + c['picked_unfinished'] + c['unpicked']
        assert c['physical_feasible_to_impossible_completed'] == 0
        assert digest(path) == ref['input_sha256']
        reports[name] = dict(input_path=str(path), input_sha256=ref['input_sha256'],
                             trajectory_sha256=ref['trajectory_sha256'], tasks=ref['completed'], audit=r)
        shutil.copy2(dest, out / dest.name)
        print('REMATCH_AUDIT_COMPLETE', name, json.dumps(r['cohorts']), flush=True)
    for name, sha in request['files'].items():
        assert digest(Path(name)) == sha, name
    for name in ['request.json', 'submission.json', 'reference.json']:
        shutil.copy2(raw / name, out / name)
    write(out / 'accounting.json', dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
          all_valid=True, all_cases_completed=True, independent_fixtures=7, reports=reports,
          binary_sha256=digest(binary), allocation=cpu, selected_cpu=cpu['representative_cpus'][0], job_id=os.environ['JOB_ID']))


if __name__ == '__main__':
    main()
