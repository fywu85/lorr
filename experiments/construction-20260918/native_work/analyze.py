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

def normalize_environment(environment):
    result = dict(environment)
    # These absent selectors have exactly the same semantics as explicit 0.
    for key in ['CGAR_TRICK_HORIZON_MARGIN_PERCENTILE', 'CGAR_TRICK_NATIVE_NEUTRAL_TAIL', 'CGAR_FRESH_PICKUP_AUDIT', 'CGAR_TRICK_MATCH_HORIZON', 'CGAR_TRICK_NATIVE_PREWARM_THREADS']:
        if result.get(key) == '0':
            result.pop(key)
    if result.get('CGAR_TRICK_NATIVE_TURN_COST') == '1':
        result.pop('CGAR_TRICK_NATIVE_TURN_COST')
    if result.get('CGAR_REASSIGN_MATCH_INTERVAL') == '10':
        result.pop('CGAR_REASSIGN_MATCH_INTERVAL')
    return result

def fields(line):
    return dict(word.split('=', 1) for word in line.split()[1:])

def distribution(values):
    v = sorted(values)
    return dict(n=len(v), mean=sum(v) / len(v) if v else None,
                p90=v[int(.9 * (len(v)-1))] if v else None,
                p99=v[int(.99 * (len(v)-1))] if v else None, max=v[-1] if v else None)

def waiting_audit(path, metric, retarget_budget=1):
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
    assert type(retarget_budget) is int and 1 <= retarget_budget <= 8
    assert changes <= retarget_budget and after_pickup == 0
    assert changes == metric['max_changes_per_task']
    return dict(tasks_completed=len(completed), outstanding_age=ages,
                never_assigned_age=distribution([H-tasks[t][1] for t in never]),
                initial_never_assigned=sum(tasks[t][1] == 0 for t in never),
                first_half_revealed_still_incomplete=sum(tasks[t][1] <= H//2 for t in outstanding),
                reveal_to_first_assignment=distribution([v[0]-1-tasks[t][1] for t, v in assigned.items()]),
                unpicked_assignment_age=distribution([H-v[0]+1 for t, v in assigned.items() if t not in picked]),
                max_reassignments=changes, declared_retarget_budget=retarget_budget, assignments_after_pickup=after_pickup)

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--raw', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--commit', required=True)
    p.add_argument('--mode', choices=['work', 'seeds', 'percentile', 'pickup', 'portfolio', 'neutral', 'workers', 'fresh', 'cadence', 'match_horizon', 'native_turn', 'prewarm', 'execution'], required=True)
    p.add_argument('--hold-job')
    p.add_argument('--control', help='Optional existing profile to use as the exact control')
    p.add_argument('--reference', type=Path, help='Verified full reference containing that control profile')
    p.add_argument('--profile', type=Path, help='One frozen base profile; only the mode-specific parameters may vary')
    p.add_argument('--execute', action='store_true')
    p.add_argument('--decision-limit-ms', type=int, choices=[1000,5000], default=5000)
    a = p.parse_args(); raw = a.raw.resolve(); out = a.output.resolve(); support = raw / 'frontier-analysis-support'
    if not a.execute:
        support.mkdir(exist_ok=False)
        copies = {'analyze.py': Path(__file__), 'reference.json': a.reference or BASE / 'results/horizon-margin-full-v92/comparison.json',
                  'profile.json': a.profile or BASE / 'results/horizon-margin-full-v92/best-variant.json'}
        for name in ['sequences-20260918/analyze.py', 'motion-20260918/analyze.py', 'assignment-20260918/analyze.py',
                     'throughput-20260918-next/analyze_matrix.py', 'throughput-20260918-strict/analyze.py',
                     'construction-20260918/verify_full.py']:
            copies['experiments/' + name] = ROOT / 'experiments' / name
        if a.mode == 'execution':
            assert a.reference and a.profile
            copies['reference-work.json'] = a.reference.parent / 'work-samples.json'
        if a.mode == 'fresh':
            copies['audit_tools.py'] = BASE / 'fresh_pickup/audit_tools.py'
        files = {}
        for name, source in copies.items():
            target = support / name; target.parent.mkdir(parents=True, exist_ok=True); shutil.copy2(source, target)
            files[str(target)] = sha(target)
        write(raw / 'frontier-analysis-request.json', dict(commit=a.commit, mode=a.mode, control=a.control, decision_limit_ms=a.decision_limit_ms, files=files))
        command = ['/usr/bin/python3', str(support / 'analyze.py'), '--raw', str(raw), '--output', str(out),
                   '--commit', a.commit, '--mode', a.mode, '--decision-limit-ms', str(a.decision_limit_ms), '--execute']
        if a.control:
            command += ['--control', a.control]
        script = raw / 'frontier-analysis.sh'; script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        submit = ['qsub', '-h', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q@research44.grid.gsb,debian.q@research57.grid.gsb', '-pe', 'threaded', '2', '-binding', 'linear:2',
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
    assert request.get('decision_limit_ms',5000) == a.decision_limit_ms
    for name, value in request['files'].items():
        assert sha(name) == value, name
    sys.path.insert(0, str(ROOT / 'tools')); from cpu_resources import cpu_resources
    resources = cpu_resources(); assert resources['effective_cpu_quota'] is None
    assert resources['physical_cores_visible'] == 2; os.sched_setaffinity(0, resources['representative_cpus'])
    control = a.control or {'work':'trick_native_work4m_regions2', 'seeds':'trick_native_horizon5000_margin1', 'percentile':'trick_native_percentile0', 'pickup':'trick_native_pickup5', 'portfolio':'trick_p90_workers1', 'neutral':'trick_p90_neutral0', 'workers':'trick_pickup8_workers1', 'fresh':'trick_fresh0', 'cadence':'trick_match10', 'match_horizon':'trick_matchguard0', 'native_turn':'trick_turn1', 'prewarm':'trick_prewarm_base', 'execution':'trick_runtime_combined8'}[a.mode]
    subprocess.run(['/usr/bin/python3', str(support / 'experiments/sequences-20260918/analyze.py'), '--input', str(raw),
                    '--output', str(out), '--control', control, '--workers', '2'], check=True)
    sys.path.insert(0, str(support / 'experiments/construction-20260918'))
    verifier = importlib.import_module('verify_full'); verifier.ROOT = ROOT
    result = verifier.verify(raw, out, a.commit, allow_failed=True, decision_limit_ms=a.decision_limit_ms)
    spec = read(raw / 'spec.json'); assert spec['trick'] == 'WAREHOUSE' and spec['experiment_track'] == 'TRICK'
    baseline = next(iter(read(support / 'profile.json').values()))
    control_cases = [c for c in spec['cases'] if c['variant'] == control]
    assert control_cases, 'requested control is absent from the submitted matrix'
    control_percentiles = {int(c['environment'].get('CGAR_TRICK_HORIZON_MARGIN_PERCENTILE', '0')) for c in control_cases}
    assert len(control_percentiles) == 1
    control_percentile = next(iter(control_percentiles))
    ref = read(support / 'reference.json')
    assert ref['all_valid_within_deadline_and_memory'] and not ref['failures']
    execution_keys = {'CGAR_TURN_PREFETCH_THREADS','CGAR_TEMPORAL_PREP_THREADS','CGAR_PICKUP_FULL_THREADS','CGAR_TURN_TABLE_MB'}
    def comparison_environment(env):
        normalized = normalize_environment(env)
        return {k:v for k,v in normalized.items() if k not in execution_keys} if a.mode=='execution' else normalized
    control_environment = comparison_environment(control_cases[0]['environment'])
    assert all(comparison_environment(c['environment']) == control_environment for c in control_cases)
    reference = {}
    for row in ref['rows']:
        if comparison_environment(row['environment']) != control_environment:
            continue
        if row['seed'] in reference:
            previous = reference[row['seed']]
            assert (row['tasks'], row['trajectory_sha256']) == (previous['tasks'], previous['trajectory_sha256'])
        reference[row['seed']] = row
    assert reference, 'verified reference does not contain the complete requested control environment'
    metrics = {m['case']:m for m in read(out / 'metrics.json')}
    samples = {}; fairness = {}; fresh_real = {}
    if a.mode == 'fresh':
        sys.path.insert(0, str(support))
        audit_tools = importlib.import_module('audit_tools')
    for r in result['rows']:
        env = r['environment']; allowed = {'CGAR_TEMPORAL_CANDIDATE_LIMIT', 'CGAR_TEMPORAL_REGION_ROUNDS'} if a.mode == 'work' else {'CGAR_TRICK_HORIZON_MARGIN_PERCENTILE'} if a.mode == 'percentile' else {'CGAR_PICKUP_WEIGHT'} if a.mode == 'pickup' else {'CGAR_TEMPORAL_WORKERS', 'CGAR_TEMPORAL_THREADS', 'CGAR_TEMPORAL_STEPS', 'CGAR_TEMPORAL_CANDIDATE_LIMIT'} if a.mode == 'portfolio' else {'CGAR_TRICK_NATIVE_NEUTRAL_TAIL'} if a.mode == 'neutral' else {'CGAR_TEMPORAL_WORKERS', 'CGAR_TEMPORAL_THREADS'} if a.mode == 'workers' else {'CGAR_FRESH_PICKUP_AUDIT'} if a.mode == 'fresh' else {'CGAR_REASSIGN_MATCH_INTERVAL'} if a.mode == 'cadence' else {'CGAR_TRICK_MATCH_HORIZON'} if a.mode == 'match_horizon' else {'CGAR_TRICK_NATIVE_TURN_COST'} if a.mode == 'native_turn' else {'CGAR_TRICK_NATIVE_PREWARM_THREADS', 'CGAR_TURN_TABLE_MB'} if a.mode == 'prewarm' else execution_keys if a.mode == 'execution' else set()
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
        neutral = int(env.get('CGAR_TRICK_NATIVE_NEUTRAL_TAIL', '0'))
        assert neutral in (0, 1)
        service_receipt = [fields(s) for s in lines if s.startswith('[CGAR_TRICK_NATIVE_SERVICE] ')]
        assert service_receipt == ([dict(policy='neutral_tail', credit='after_action_hit', reservations='complete', protected='unchanged')] if neutral else [])
        service_samples = [fields(s) for s in lines if s.startswith('[cgar-native-service] ')]
        assert [int(m['step']) for m in service_samples] == (list(range(200, 5001, 200)) if neutral else [])
        assert all(int(m['served_choices']) >= int(m['changed_choices']) > 0 for m in service_samples)
        for x, y in zip(service_samples, service_samples[1:]):
            assert int(y['served_choices']) >= int(x['served_choices']) and int(y['changed_choices']) >= int(x['changed_choices'])
        global_samples = [fields(s) for s in lines if s.startswith('[cgar-temporal] ')]
        regional = [fields(s) for s in lines if s.startswith('[cgar-temporal-regions] ')]
        assert [int(m['step']) for m in global_samples] == [int(m['step']) for m in regional] == list(range(200,5001,200))
        assert all(m['candidate_limit'] == env['CGAR_TEMPORAL_CANDIDATE_LIMIT'] for m in global_samples)
        assert all(m['rounds'] == env['CGAR_TEMPORAL_REGION_ROUNDS'] for m in regional)
        if a.mode == 'portfolio':
            workers = int(env['CGAR_TEMPORAL_WORKERS'])
            threads = int(env['CGAR_TEMPORAL_THREADS'])
            assert workers in (1, 2, 4) and threads == workers
            assert workers * int(env['CGAR_TEMPORAL_STEPS']) == 1000000
            assert workers * int(env['CGAR_TEMPORAL_CANDIDATE_LIMIT']) == 4000000
            allocations = [fields(s) for s in lines if s.startswith('[cgar-temporal-allocation] ')]
            assert len(allocations) == 1
            assert allocations[0]['workers'] == str(workers) and allocations[0]['threads'] == str(threads)
            assert allocations[0]['allowed_cpus'] == '4'
            assert all(int(m['workers']) == workers and int(m['threads']) == threads and
                       0 <= int(m['selected_worker']) < workers for m in global_samples)
            # Diagnostics expose the selected worker only. All-worker completion
            # is enforced by the frozen adapter's join/exception path; these
            # samples cannot be reported as total work across the portfolio.
            assert all(int(m['repairs']) == int(env['CGAR_TEMPORAL_STEPS']) or
                       int(m['candidates']) >= int(env['CGAR_TEMPORAL_CANDIDATE_LIMIT'])
                       for m in global_samples)
        if a.mode == 'workers':
            workers = int(env['CGAR_TEMPORAL_WORKERS'])
            assert workers in (1, 2, 4) and int(env['CGAR_TEMPORAL_THREADS']) == workers
            assert env['CGAR_TEMPORAL_STEPS'] == '1000000' and env['CGAR_TEMPORAL_CANDIDATE_LIMIT'] == '4000000'
            allocations = [fields(s) for s in lines if s.startswith('[cgar-temporal-allocation] ')]
            assert len(allocations) == 1 and allocations[0]['allowed_cpus'] == '4'
            assert allocations[0]['workers'] == allocations[0]['threads'] == str(workers)
            assert all(int(m['workers']) == int(m['threads']) == workers and 0 <= int(m['selected_worker']) < workers for m in global_samples)
            assert all(int(m['repairs']) == 1000000 or int(m['candidates']) >= 4000000 for m in global_samples)
        if a.mode == 'native_turn':
            turn = int(env.get('CGAR_TRICK_NATIVE_TURN_COST', '1'))
            assert turn in (1, 2, 4)
            turns = [fields(s) for s in lines if s.startswith('[CGAR_TRICK_NATIVE_TURN] ')]
            expected_turn = dict(cost=str(turn), default_cost='1', physical_ticks='1', forward_base='20', field='unchanged', scope='route_and_pickup')
            assert turns == ([expected_turn] if turn != 1 else [])
            native = [fields(s) for s in lines if s.startswith('[CGAR_TRICK] ')]
            assert len(native) == 1 and native[0]['provider'] == 'nms-native-metric'
            assert native[0]['turn'] == str(turn) and native[0]['forward_base'] == '20' and native[0]['opposing'] == '200' and native[0]['band'] == '1'
            assert native[0]['score'] == 'pure_potential' and native[0]['tie'] == 'raw' and native[0]['learned_publications'] == 'disabled'
            quoted = [fields(s) for s in lines if s.startswith('[cgar-native-metric] ')]
            assert [int(m['t']) for m in quoted] == list(range(0, 5000, 200))
            assert all(m['turn'] == str(turn) and m['forward_base'] == '20' and m['cost_limit'] == '201' for m in quoted)
        if a.mode == 'execution':
            assert spec['cpus_per_instance']==8 and a.decision_limit_ms==1000
            assert env.get('CGAR_TRICK_NATIVE_PREWARM_THREADS','0')=='0'
            assert env.get('CGAR_TURN_PREFETCH_THREADS','0') in ('0','8')
            assert env['CGAR_TEMPORAL_PREP_THREADS'] in ('4','8') and env['CGAR_PICKUP_FULL_THREADS'] in ('4','8')
            assert env['CGAR_TURN_TABLE_MB'] in ('8192','25600')
            reference_row=reference[r['seed']]
            assert (r['tasks'],r['trajectory_sha256']) == (reference_row['tasks'],reference_row['trajectory_sha256'])
            reference_work=read(support/'reference-work.json')[reference_row['case']]
            assert global_samples==reference_work['global_work'] and regional==reference_work['regional_work']
            assert q_samples==reference_work['percentile']
            allocations=[fields(s) for s in lines if s.startswith('[cgar-temporal-allocation] ')]
            assert len(allocations)==1 and allocations[0]['allowed_cpus']=='8'
            assert allocations[0]['workers']==allocations[0]['threads']=='1' and allocations[0]['region_threads']=='4'
            assert allocations[0]['preparation_threads']==env['CGAR_TEMPORAL_PREP_THREADS']
            assert allocations[0]['pickup_threads']==env['CGAR_PICKUP_FULL_THREADS']
        if a.mode == 'prewarm':
            prewarm = int(env['CGAR_TRICK_NATIVE_PREWARM_THREADS'])
            assert (r['variant'], int(env['CGAR_TURN_TABLE_MB']), prewarm) in {
                ('trick_prewarm_base', 8192, 0), ('trick_prewarm_cache', 25600, 0), ('trick_prewarm_all', 25600, 8)}
            assert spec['cpus_per_instance'] == 8
            receipts = [fields(s) for s in lines if s.startswith('[CGAR_TRICK_NATIVE_PREWARM] ')]
            if prewarm:
                assert len(receipts) == 1
                receipt = dict(receipts[0]); elapsed = int(receipt.pop('wall_us'))
                assert 0 < elapsed < 30000000
                assert receipt == dict(complete='1', goals='38586', threads='8', stored_bytes='11911035168',
                                       input='map_only', initial_dispatch='unchanged')
            else:
                assert not receipts
            allocations = [fields(s) for s in lines if s.startswith('[cgar-temporal-allocation] ')]
            assert len(allocations) == 1 and allocations[0]['allowed_cpus'] == '8'
            assert allocations[0]['workers'] == allocations[0]['threads'] == '1'
            assert allocations[0]['preparation_threads'] == allocations[0]['region_threads'] == allocations[0]['pickup_threads'] == '4'
            assert env['CGAR_TRICK_MATCH_HORIZON'] == '1'
            guarded = [fields(s) for s in lines if s.startswith('[cgar-match-horizon] ')]
            assert [int(m['t']) for m in guarded] == list(range(0, 5000, 200))
        if a.mode == 'cadence':
            interval = int(env['CGAR_REASSIGN_MATCH_INTERVAL']); assert interval in (5, 10)
            cadence_receipts = [fields(s) for s in lines if s.startswith('[cgar-match-cadence] ')]
            expected_cadence = dict(interval=str(interval), default_interval='10', cooldown='20', task_budget='1', same_group_quota='1', fixed_work='1')
            assert cadence_receipts == ([expected_cadence] if interval != 10 else [])
            matches = [fields(s) for s in lines if s.startswith('[cgar-unopened-match] t=')]
            assert [int(m['t']) for m in matches] == list(range(0, 5000, 200))
            for m in matches:
                assert int(m['passes']) == int(m['t']) // interval + 1
                assert int(m['groups']) <= int(m['passes']) * int(env['CGAR_REASSIGN_MATCH_GROUPS'])
                assert int(m['nodes']) <= int(m['groups']) * 2048
            print('CADENCE_VERIFIED', r['case'], interval, matches[-1]['passes'], flush=True)
        samples[r['case']] = dict(global_work=global_samples, regional_work=regional, percentile=q_samples, native_service=service_samples)
        if a.mode == 'cadence':
            samples[r['case']]['matching'] = matches
        if a.mode == 'fresh':
            fresh = int(env.get('CGAR_FRESH_PICKUP_AUDIT', '0')); assert fresh in (0, 1)
            samples[r['case']]['fresh_audit'] = audit_tools.audit_samples(lines, fresh, 5000, int(env['CGAR_FLOW_COST_SCALE']))
            fresh_real[r['case']] = hashlib.sha256(json.dumps(audit_tools.real_diagnostics(lines), separators=(',', ':')).encode()).hexdigest()
        if a.mode == 'match_horizon':
            guard = int(env['CGAR_TRICK_MATCH_HORIZON']); assert guard in (0, 1)
            receipts = [fields(s) for s in lines if s.startswith('[CGAR_TRICK_MATCH_HORIZON] ')]
            expected = dict(enabled='1', policy='reject_worse_task_tier', cycle='whole', fields='resident_spatial', fallback='manhattan', fair='unchanged', budget='one')
            assert receipts == ([expected] if guard else [])
            guarded = [fields(s) for s in lines if s.startswith('[cgar-match-horizon] ')]
            matches = [fields(s) for s in lines if s.startswith('[cgar-unopened-match] t=')]
            assert [int(m['t']) for m in matches] == list(range(0, 5000, 200))
            assert [int(m['t']) for m in guarded] == (list(range(0, 5000, 200)) if guard else [])
            for m in matches:
                assert int(m['passes']) == int(m['t']) // 10 + 1
                assert int(m['groups']) <= int(m['passes']) * int(env['CGAR_REASSIGN_MATCH_GROUPS'])
                assert int(m['nodes']) <= int(m['groups']) * 2048
            for m, original in zip(guarded, matches):
                assert int(m['cycles']) == int(original['accepted_cycles']) + int(m['rejected'])
                assert int(m['rows']) >= 2 * int(m['cycles'])
                assert int(m['worse_rows']) >= int(m['rejected'])
            for previous, current in zip(guarded, guarded[1:]):
                assert all(int(current[k]) >= int(previous[k]) for k in ['cycles', 'rows', 'rejected', 'worse_rows'])
            if guard:
                assert int(guarded[-1]['rejected']) > 0, 'cutoff guard never vetoed a cycle'
            samples[r['case']]['matching'] = matches
            samples[r['case']]['match_horizon'] = guarded
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
        if a.mode == 'fresh':
            assert r['environment']['CGAR_FRESH_PICKUP_AUDIT'] == '1' and c['environment']['CGAR_FRESH_PICKUP_AUDIT'] == '0'
            assert r['tasks'] == c['tasks'] and r['trajectory_sha256'] == c['trajectory_sha256']
            assert fresh_real[r['case']] == fresh_real[c['case']], 'audit changed real work counters'
        pairs.append(dict(seed=r['seed'], variant=r['variant'], tasks=r['tasks'], control_tasks=c['tasks'],
                          difference=r['tasks']-c['tasks'], final1000_difference=r['final1000']-c['final1000'],
                          age_p90_difference=r['outstanding_age_p90']-c['outstanding_age_p90']))
    result.update(mode=a.mode, control_variant=control, exact_reference_control_seeds=sorted(set(controls) & set(reference)), pairs=pairs,
                  promoted=False, scope='Full fixed-work Warehouse TRICK with ordinary fairness. Shared5s development,32decimalGB. Finite fairness observations do not prove starvation freedom.')
    if a.mode == 'execution':
        result.update(exact_execution_trajectories=True, exact_sampled_search_work=True, strict_one_second_complete_entries=not result['failures'], scope='Execution-only Warehouse TRICK: exact reference trajectories and sampled global/regional work, enforced1000ms complete entries, eight bound physical cores and32decimalGB RSS. Host exclusivity is recorded separately. Finite fairness observations are not a proof.')
    if a.mode == 'fresh':
        result.update(exact_audit_trajectories=True, exact_audit_real_diagnostics=True, real_diagnostic_sha256=fresh_real)
    write(out / 'verification.json', result); write(out / 'work-samples.json', samples); write(out / 'fairness.json', fairness)
    for name in ('frontier-analysis-request.json','frontier-analysis-submission.json'):
        shutil.copy2(raw / name, out / name)
    print('FRONTIER_VERIFIED', len(result['rows']), 'valid', len(result['failures']), 'failed', flush=True)

if __name__ == '__main__':
    main()
