#!/usr/bin/env python3
"""Freeze and run independent unfinished-work accounting after full validation."""
import argparse
import datetime
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys

ROOT = next(p for p in Path(__file__).resolve().parents if (p / 'tools/cpu_resources.py').is_file())
BASE = ROOT / 'experiments/construction-20260918'

def read(p):
    return json.loads(Path(p).read_text())

def write(p, value):
    Path(p).write_text(json.dumps(value, indent=2) + '\n')

def digest(p):
    h = hashlib.sha256()
    with Path(p).open('rb') as f:
        for block in iter(lambda: f.read(4 * 1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()

def module(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result

def trajectory(data):
    h = hashlib.sha256()
    encoder = json.JSONEncoder(separators=(',', ':'))
    for key in ['actualPaths', 'actualSchedule', 'events', 'tasks']:
        h.update(key.encode())
        for chunk in encoder.iterencode(data[key]):
            h.update(chunk.encode())
    return h.hexdigest()

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--verification', type=Path, required=True)
    p.add_argument('--raw', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--hold-job')
    p.add_argument('--execute', action='store_true')
    a = p.parse_args(); raw = a.raw.resolve(); out = a.output.resolve()
    if not a.execute:
        raw.mkdir(parents=True, exist_ok=False)
        files = {}
        for source, name in [(Path(__file__), 'outcomes.py'), (BASE / 'horizon_audit/analyze.py', 'work.py'),
                             (BASE / 'tricks/horizon_margin/analyze.py', 'margin.py')]:
            target = raw / name; shutil.copy2(source, target); files[name] = digest(target)
        write(raw / 'request.json', dict(verification=str(a.verification.resolve()), output=str(out), files=files,
              created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat()))
        command = ['/usr/bin/python3', str(raw / 'outcomes.py'), '--verification', str(a.verification.resolve()),
                   '--raw', str(raw), '--output', str(out), '--execute']
        job = raw / 'job.sh'; job.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        submit = ['qsub', '-h', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=false,h_rt=00:30:00,h_vmem=8G', '-m', 'n',
                  '-N', 'frontier_outcomes', '-j', 'y', '-o', str(raw / 'job.log'), '-S', '/bin/bash']
        if a.hold_job:
            submit += ['-hold_jid', a.hold_job]
        submit.append(str(job))
        result = subprocess.run(submit, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(raw / 'submission.json', dict(command=submit, returncode=result.returncode, response=result.stdout))
        result.check_returncode(); assert re.fullmatch(r'\d+\s*', result.stdout)
        print(result.stdout, end='', flush=True); subprocess.run(['qrls', result.stdout.strip()], check=True); return
    request = read(raw / 'request.json')
    assert request['verification'] == str(a.verification.resolve()) and request['output'] == str(out)
    for name, value in request['files'].items():
        assert digest(raw / name) == value
    sys.path.insert(0, str(ROOT / 'tools')); from cpu_resources import cpu_resources
    cpu = cpu_resources(); assert cpu['effective_cpu_quota'] is None and cpu['physical_cores_visible'] == 1
    os.sched_setaffinity(0, cpu['representative_cpus'])
    out.mkdir(parents=True, exist_ok=False)
    verification_sha = digest(a.verification); v = read(a.verification)
    assert v['all_completed_runs_valid_within_deadline_and_memory'] and v['disjoint_physical_bindings_during_concurrent_execution']
    work = module(raw / 'work.py', 'unfinished_work'); margin = module(raw / 'margin.py', 'margin_prefix')
    write(out / 'fixtures.json', work.fixtures())
    records = {}; prefixes = {}; mode = v['mode']
    control = v.get('control_variant') or {'work':'trick_native_work4m_regions2', 'pickup':'trick_native_pickup5',
               'percentile':'trick_native_percentile0', 'seeds':'trick_native_horizon5000_margin1'}[mode]
    for row in v['rows']:
        path = Path(row['raw_case']) / 'WAREHOUSE.json'; sha = digest(path); data = read(path)
        assert data['makespan'] == 5000 and data['teamSize'] == 10000 and data['numTaskFinished'] == row['tasks']
        assert not any(data[k] for k in ['numPlannerErrors','numScheduleErrors','numEntryTimeouts'])
        assert trajectory(data) == row['trajectory_sha256']
        r = work.audit(data, [4000,4250,4500,4600,4750,4900])
        assert r['phase_steps']['empty'] == row['empty_robot_steps']
        assert digest(path) == sha
        r.update(input_path=str(path), input_sha256=sha, trajectory_sha256=row['trajectory_sha256'])
        records[row['case']] = r
        if mode == 'percentile':
            # All profiles share the ordinary policy before the first horizon
            # comparator reversal. This deliberately uses the earliest possible
            # intervention, not an assumed completion-count divergence time.
            lines = path.with_suffix('.log').read_text().splitlines()
            ticks = [int(line.split('t=')[1].split()[0]) for line in lines if line.startswith('[cgar-horizon-first-rank-change] ')]
            assert len(ticks) == 1 and 0 < ticks[0] < 5000
            prefixes[row['case']] = dict(first_rank_change=ticks[0], seed=row['seed'], variant=row['variant'])
        print('OUTCOME_AUDITED', row['case'], r['unfinished_task_total'], flush=True)
        del data
    pairs = []
    controls = {r['seed']:r for r in v['rows'] if r['variant'] == control}
    for row in v['rows']:
        if row['variant'] == control or row['seed'] not in controls:
            continue
        c = controls[row['seed']]; before = records[c['case']]; after = records[row['case']]
        pair = dict(seed=row['seed'], variant=row['variant'], tasks_difference=row['tasks']-c['tasks'],
                    empty_work_difference=row['empty_robot_steps']-c['empty_robot_steps'],
                    unfinished_work_difference=after['unfinished_task_total']-before['unfinished_task_total'])
        if mode == 'percentile':
            t = min(prefixes[c['case']]['first_rank_change'], prefixes[row['case']]['first_rank_change'])
            x = read(Path(c['raw_case']) / 'WAREHOUSE.json'); sha = margin.prefix(x, t); del x
            x = read(Path(row['raw_case']) / 'WAREHOUSE.json'); assert margin.prefix(x, t) == sha; del x
            pair.update(exact_pre_intervention_steps=t, prefix_sha256=sha)
        pairs.append(pair)
    assert digest(a.verification) == verification_sha
    for name in ['request.json','submission.json']:
        shutil.copy2(raw / name, out / name)
    shutil.copy2(a.verification, out / 'verification.json')
    write(out / 'accounting.json', dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
          all_valid=True, all_cases_completed=not v['failures'], mode=mode, reports=records, pairs=pairs,
          verification_sha256=verification_sha, allocation=cpu, job_id=os.environ['JOB_ID'],
          limits='Independent accounting of verified complete trajectories. End-censored work and changed cohorts are not causal savings or a throughput upper bound. Prefix checks apply only to percentile profiles.'))
    print('OUTCOMES_COMPLETE', len(records), flush=True)

if __name__ == '__main__':
    main()
