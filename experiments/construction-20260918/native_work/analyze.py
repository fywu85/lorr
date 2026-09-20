#!/usr/bin/env python3
"""Freeze and run full Warehouse frontier verification, including waiting tails."""
import argparse
import datetime
import hashlib
import importlib
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

def sha(p):
    h = hashlib.sha256()
    with Path(p).open('rb') as f:
        for data in iter(lambda: f.read(4 * 1024 * 1024), b''):
            h.update(data)
    return h.hexdigest()

def fields(line):
    return dict(word.split('=', 1) for word in line.split()[1:])

def distribution(values):
    v = sorted(values)
    return dict(n=len(v), mean=sum(v) / len(v) if v else None,
                p90=v[int(.9 * (len(v)-1))] if v else None,
                p99=v[int(.99 * (len(v)-1))] if v else None, max=v[-1] if v else None)

def waiting_audit(path, metric):
    d = read(path); H = d['makespan']; tasks = {t[0]: t for t in d['tasks']}
    completed = {}; picked = {}; assigned = {}
    for tick, robot, task, stop in d['events']:
        if stop == 1:
            picked[task] = tick
        if stop == len(tasks[task][2]) // 2:
            completed[task] = tick
    for line in d['actualSchedule']:
        for entry in line.split(','):
            if not entry:
                continue
            tick, task = map(int, entry.split(':'))
            if task >= 0:
                assigned.setdefault(task, []).append(tick)
    for values in assigned.values():
        values.sort()
    outstanding = set(tasks) - set(completed)
    never = [t for t in outstanding if t not in assigned]
    ages = distribution([H - tasks[t][1] for t in outstanding])
    assert len(completed) == d['numTaskFinished'] == metric['tasks']
    assert ages['n'] == metric['outstanding_task_age']['n'] and ages['p90'] == metric['outstanding_task_age']['p90']
    changes = max((len(v)-1 for v in assigned.values()), default=0)
    after_pickup = sum(tick > picked[t] for t, values in assigned.items() if t in picked for tick in values)
    assert changes <= 1 and after_pickup == 0
    return dict(tasks_completed=len(completed), outstanding_age=ages,
                never_assigned_age=distribution([H-tasks[t][1] for t in never]),
                initial_never_assigned=sum(tasks[t][1] == 0 for t in never),
                first_half_revealed_still_incomplete=sum(tasks[t][1] <= H//2 for t in outstanding),
                reveal_to_first_assignment=distribution([v[0]-1-tasks[t][1] for t, v in assigned.items()]),
                unpicked_assignment_age=distribution([H-v[0]+1 for t, v in assigned.items() if t not in picked]),
                max_reassignments=changes, assignments_after_pickup=after_pickup)

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--raw', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--commit', required=True)
    p.add_argument('--mode', choices=['work', 'seeds', 'percentile', 'pickup'], required=True)
    p.add_argument('--hold-job')
    p.add_argument('--control', help='Optional existing profile to use as the exact control')
    p.add_argument('--reference', type=Path, help='Verified full reference containing that control profile')
    p.add_argument('--profile', type=Path, help='One frozen base profile; only the mode-specific parameters may vary')
    p.add_argument('--execute', action='store_true')
    a = p.parse_args(); raw = a.raw.resolve(); out = a.output.resolve(); support = raw / 'frontier-analysis-support'
    if not a.execute:
        support.mkdir(exist_ok=False)
        copies = {'analyze.py': Path(__file__), 'reference.json': a.reference or BASE / 'results/horizon-margin-full-v92/comparison.json',
                  'profile.json': a.profile or BASE / 'results/horizon-margin-full-v92/best-variant.json'}
        for name in ['sequences-20260918/analyze.py', 'motion-20260918/analyze.py', 'assignment-20260918/analyze.py',
                     'throughput-20260918-next/analyze_matrix.py', 'throughput-20260918-strict/analyze.py',
                     'construction-20260918/verify_full.py']:
            copies['experiments/' + name] = ROOT / 'experiments' / name
        files = {}
        for name, source in copies.items():
            target = support / name; target.parent.mkdir(parents=True, exist_ok=True); shutil.copy2(source, target)
            files[str(target)] = sha(target)
        write(raw / 'frontier-analysis-request.json', dict(commit=a.commit, mode=a.mode, control=a.control, files=files))
        command = ['/usr/bin/python3', str(support / 'analyze.py'), '--raw', str(raw), '--output', str(out),
                   '--commit', a.commit, '--mode', a.mode, '--execute']
        if a.control:
            command += ['--control', a.control]
        script = raw / 'frontier-analysis.sh'; script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        submit = ['qsub', '-h', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '2', '-binding', 'linear:2',
                  '-l', 'exclusive=false,h_rt=00:30:00,h_vmem=8G', '-m', 'n', '-N', 'frontier_analysis', '-j', 'y',
                  '-o', str(raw / 'frontier-analysis.log'), '-S', '/bin/bash']
        if a.hold_job:
            submit += ['-hold_jid', a.hold_job]
        result = subprocess.run(submit + [str(script)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        receipt = dict(command=submit + [str(script)], returncode=result.returncode, response=result.stdout)
        write(raw / 'frontier-analysis-submission.json', receipt); write(raw / 'motion-analysis-submission.json', receipt)
        result.check_returncode(); assert re.fullmatch(r'\d+\s*', result.stdout)
        print(result.stdout, end='', flush=True); subprocess.run(['qrls', result.stdout.strip()], check=True); return
    request = read(raw / 'frontier-analysis-request.json')
    assert request['commit'] == a.commit and request['mode'] == a.mode and request.get('control') == a.control
    for name, value in request['files'].items():
        assert sha(name) == value, name
    sys.path.insert(0, str(ROOT / 'tools')); from cpu_resources import cpu_resources
    resources = cpu_resources(); assert resources['effective_cpu_quota'] is None
    assert resources['physical_cores_visible'] == 2; os.sched_setaffinity(0, resources['representative_cpus'])
    control = a.control or {'work':'trick_native_work4m_regions2', 'seeds':'trick_native_horizon5000_margin1', 'percentile':'trick_native_percentile0', 'pickup':'trick_native_pickup5'}[a.mode]
    subprocess.run(['/usr/bin/python3', str(support / 'experiments/sequences-20260918/analyze.py'), '--input', str(raw),
                    '--output', str(out), '--control', control, '--workers', '2'], check=True)
    sys.path.insert(0, str(support / 'experiments/construction-20260918'))
    verifier = importlib.import_module('verify_full'); verifier.ROOT = ROOT
    result = verifier.verify(raw, out, a.commit, allow_failed=True, decision_limit_ms=5000)
    spec = read(raw / 'spec.json'); assert spec['trick'] == 'WAREHOUSE' and spec['experiment_track'] == 'TRICK'
    baseline = next(iter(read(support / 'profile.json').values()))
    control_cases = [c for c in spec['cases'] if c['variant'] == control]
    assert control_cases, 'requested control is absent from the submitted matrix'
    control_percentiles = {int(c['environment'].get('CGAR_TRICK_HORIZON_MARGIN_PERCENTILE', '0')) for c in control_cases}
    assert len(control_percentiles) == 1
    control_percentile = next(iter(control_percentiles))
    ref = read(support / 'reference.json')
    assert ref['all_valid_within_deadline_and_memory'] and not ref['failures']
    reference = {r['seed']:r for r in ref['rows'] if r['environment']['CGAR_TRICK_HORIZON_MARGIN'] == '1'
                 and int(r['environment'].get('CGAR_TRICK_HORIZON_MARGIN_PERCENTILE', '0')) == control_percentile}
    assert reference, 'verified reference does not contain the requested control percentile'
    metrics = {m['case']:m for m in read(out / 'metrics.json')}
    samples = {}; fairness = {}
    for r in result['rows']:
        env = r['environment']; allowed = {'CGAR_TEMPORAL_CANDIDATE_LIMIT', 'CGAR_TEMPORAL_REGION_ROUNDS'} if a.mode == 'work' else {'CGAR_TRICK_HORIZON_MARGIN_PERCENTILE'} if a.mode == 'percentile' else {'CGAR_PICKUP_WEIGHT'} if a.mode == 'pickup' else set()
        assert {k:v for k,v in env.items() if k not in allowed} == {k:v for k,v in baseline.items() if k not in allowed}
        lines = (Path(r['raw_case']) / 'WAREHOUSE.log').read_text().splitlines()
        receipt = [fields(s) for s in lines if s.startswith('[CGAR_TRICK_COMPONENTS] ')]
        assert len(receipt) == 1 and receipt[0]['hrrn'] == receipt[0]['oldest_admission'] == '1' and receipt[0]['short_tasks'] == '0' and receipt[0]['started_tasks'] == 'protected'
        percentile = int(env.get('CGAR_TRICK_HORIZON_MARGIN_PERCENTILE', '0'))
        receipt = [fields(s) for s in lines if s.startswith('[CGAR_TRICK_HORIZON_MARGIN] ')]
        assert len(receipt) == 1 and receipt[0]['estimator'] == ('prospective_bucket_percentile' if percentile else 'prospective_bucket_mean')
        q_receipt = [fields(s) for s in lines if s.startswith('[CGAR_TRICK_HORIZON_PERCENTILE] ')]
        assert q_receipt == ([dict(percentile=str(percentile), rank='nearest', samples='completed_single_holder', fair='unchanged', held='unchanged')] if percentile else [])
        models = [fields(s) for s in lines if s.startswith('[cgar-horizon-margin] ')]
        assert models and all(int(m['bound_violations']) == 0 for m in models)
        q_samples = [fields(s) for s in lines if s.startswith('[cgar-horizon-percentile] ')]
        assert len(q_samples) == (len(models) if percentile else 0)
        for m in q_samples:
            assert int(m['percentile']) == percentile and all(int(m['q%d'%k]) >= 0 for k in range(5))
        global_samples = [fields(s) for s in lines if s.startswith('[cgar-temporal] ')]
        regional = [fields(s) for s in lines if s.startswith('[cgar-temporal-regions] ')]
        assert [int(m['step']) for m in global_samples] == [int(m['step']) for m in regional] == list(range(200,5001,200))
        assert all(m['candidate_limit'] == env['CGAR_TEMPORAL_CANDIDATE_LIMIT'] for m in global_samples)
        assert all(m['rounds'] == env['CGAR_TEMPORAL_REGION_ROUNDS'] for m in regional)
        samples[r['case']] = dict(global_work=global_samples, regional_work=regional, percentile=q_samples)
        if r['variant'] == control and r['seed'] in reference:
            ref = reference[r['seed']]
            assert r['tasks'] == ref['tasks'] and r['trajectory_sha256'] == ref['trajectory_sha256'], ('control mismatch', r['case'])
        fairness[r['case']] = waiting_audit(Path(r['raw_case']) / 'WAREHOUSE.json', metrics[r['case']])
        print('VALIDATED', r['case'], r['tasks'], 'oldest_never_assigned', fairness[r['case']]['never_assigned_age']['max'], flush=True)
    controls = {r['seed']:r for r in result['rows'] if r['variant'] == control}
    pairs = []
    for r in result['rows']:
        if r['variant'] == control or r['seed'] not in controls:
            continue
        c = controls[r['seed']]
        pairs.append(dict(seed=r['seed'], variant=r['variant'], tasks=r['tasks'], control_tasks=c['tasks'],
                          difference=r['tasks']-c['tasks'], final1000_difference=r['final1000']-c['final1000'],
                          age_p90_difference=r['outstanding_age_p90']-c['outstanding_age_p90']))
    result.update(mode=a.mode, control_variant=control, exact_reference_control_seeds=sorted(set(controls) & set(reference)), pairs=pairs,
                  promoted=False, scope='Full fixed-work Warehouse TRICK with ordinary fairness. Shared5s development,32decimalGB. Finite fairness observations do not prove starvation freedom.')
    write(out / 'verification.json', result); write(out / 'work-samples.json', samples); write(out / 'fairness.json', fairness)
    for name in ('frontier-analysis-request.json','frontier-analysis-submission.json'):
        shutil.copy2(raw / name, out / name)
    print('FRONTIER_VERIFIED', len(result['rows']), 'valid', len(result['failures']), 'failed', flush=True)

if __name__ == '__main__':
    main()
