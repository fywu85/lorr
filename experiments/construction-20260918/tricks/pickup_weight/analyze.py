#!/usr/bin/env python3
"""Verify explicit Warehouse pickup-weight/short-preference factorial with fixed matching64."""
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
import statistics
import subprocess
import sys

ROOT = next(p for p in Path(__file__).resolve().parents if (p / 'tools/cpu_resources.py').is_file())
BASE = ROOT / 'experiments/construction-20260918'
KEY = 'CGAR_PICKUP_WEIGHT'
CHANGED = {KEY, 'CGAR_REASSIGN_MATCH_GROUPS', 'CGAR_TRICK_LANES', 'CGAR_TRICK_SHORT_TASKS', 'CGAR_TRICK_UNOPENED_MATCH'}


def read(p):
    return json.loads(p.read_text())


def write(p, data):
    p.write_text(json.dumps(data, indent=2) + '\n')


def digest(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def fields(line):
    return {k: float(v) if '.' in v else int(v) for k, v in re.findall(r'(\w+)=([-\d.]+)', line)}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--raw', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--commit', required=True)
    p.add_argument('--hold-job')
    p.add_argument('--execute', action='store_true')
    a = p.parse_args()
    raw, out = a.raw.resolve(), a.output.resolve()
    support = raw / 'trick-pickup-weight-analysis-support'
    if not a.execute:
        support.mkdir(exist_ok=False)
        own = support / 'analyze.py'
        shutil.copy2(Path(__file__), own)
        files = [own]
        for name in ['sequences-20260918/analyze.py', 'motion-20260918/analyze.py',
                     'assignment-20260918/analyze.py', 'throughput-20260918-next/analyze_matrix.py',
                     'throughput-20260918-strict/analyze.py', 'construction-20260918/verify_full.py',
                     'construction-20260918/collect_cold.py']:
            dest = support / 'experiments' / name
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(ROOT / 'experiments' / name, dest)
            files.append(dest)
        for name, source in [('reference.json', BASE / 'results/trick-match-full-v65/verification.json'),
                             ('reference-profile.json', BASE / 'warehouse-reference-variants.json'),
                             ('reference-screen.json', BASE / 'results/trick-short-tasks-screen-v58/trajectory-fingerprints.json')]:
            shutil.copy2(source, support / name)
            files.append(support / name)
        write(raw / 'trick-pickup-weight-analysis-request.json', {'root': str(ROOT), 'commit': a.commit,
              'files': {str(f): digest(f) for f in files}, 'raw': str(raw), 'output': str(out)})
        command = ['/usr/bin/python3', str(own), '--execute', '--raw', str(raw), '--output', str(out), '--commit', a.commit]
        job = raw / 'trick-pickup-weight-analysis.sh'
        job.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        submit = ['qsub', '-h', '-terse', '-w', 'n', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=false,h_rt=00:30:00,h_vmem=8G', '-m', 'n',
                  '-N', 'trick_pickup_weight', '-j', 'y', '-o', str(raw / 'trick-pickup-weight-analysis.log'), '-S', '/bin/bash']
        if a.hold_job:
            submit += ['-hold_jid', a.hold_job]
        submit.append(str(job))
        result = subprocess.run(submit, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        receipt = dict(command=submit, returncode=result.returncode, response=result.stdout)
        write(raw / 'trick-pickup-weight-analysis-submission.json', receipt)
        write(raw / 'motion-analysis-submission.json', receipt)
        print(result.stdout, end='')
        if result.returncode:
            return result.returncode
        assert re.fullmatch(r'\d+\s*', result.stdout), result.stdout
        subprocess.run(['qrls', result.stdout.strip()], check=True)
        return 0

    request = read(raw / 'trick-pickup-weight-analysis-request.json')
    assert request['commit'] == a.commit and Path(request['root']) == ROOT
    for filename, sha in request['files'].items():
        assert digest(Path(filename)) == sha, filename
    sys.path.insert(0, str(ROOT / 'tools'))
    from cpu_resources import cpu_resources
    cpu = cpu_resources()
    assert cpu['effective_cpu_quota'] is None and cpu['representative_cpus']
    os.sched_setaffinity(0, cpu['representative_cpus'][:1])
    spec, build = read(raw / 'spec.json'), read(raw / 'build.json')
    assert spec['trick'] == 'WAREHOUSE' and spec['experiment_track'] == 'TRICK'
    assert spec['instances'] == ['WAREHOUSE'] and spec['time_limit_ms'] in (1000, 5000) and spec['cpus_per_instance'] == 4
    if spec['time_limit_ms'] != 1000:
        assert spec['benchmark_mode'] == 'relaxed_development'
    sources = dict(build['sources'], **build['test_sources'])
    for name, sha in sources.items():
        assert hashlib.sha256(subprocess.check_output(['git', 'show', a.commit + ':' + name], cwd=ROOT)).hexdigest() == sha, name
    assert digest(raw / 'lifelong') == build['binary_sha256']
    horizon = (spec['horizons'] or {'WAREHOUSE': 5000})['WAREHOUSE']
    assert horizon == 5000
    profile = next(iter(read(support / 'reference-profile.json').values()))
    cases = spec['cases']
    seeds = sorted({c['seed'] for c in cases})
    assert (seeds == [0] if horizon == 200 else seeds in ([0, 2], [1, 3, 4, 5], [0, 1, 2, 3, 4, 5]))
    assert len(cases) == 4 * len(seeds)
    by_seed = {seed: {} for seed in seeds}
    receipts, intervals = {}, []
    allocation = read(raw / 'allocation.json')
    for case in cases:
        name, env = case['name'], case['environment']
        weight = int(env[KEY]); mode = int(env['CGAR_TRICK_UNOPENED_MATCH'])
        quota = int(env.get('CGAR_REASSIGN_MATCH_GROUPS', '4'))
        short = int(env['CGAR_TRICK_SHORT_TASKS'])
        arm = (short, weight)
        assert env['CGAR_TRICK_LANES'] == '1' and short in (0, 1)
        assert mode == 1 and quota == 64 and weight in (1,5) and arm not in by_seed[case['seed']]
        by_seed[case['seed']][arm] = name
        assert int(env.get('CGAR_TEMPORAL_REMAINING_FLOW', '0')) == 0
        assert {k: v for k, v in env.items() if k not in CHANGED} == {k: v for k, v in profile.items() if k not in CHANGED}
        meta, summary = read(raw / name / 'metadata.json'), read(raw / name / 'summary.json')[0]
        assert meta['trick'] == 'WAREHOUSE' and meta['trick_argv'] == ['--trick','WAREHOUSE'] and meta['experiment_track'] == 'TRICK'
        assert meta['build_provenance'] == build and meta['max_process_memory_bytes'] == 32000000000
        assert summary['trick_receipt_valid'] and summary['experiment_track_valid']
        resources = meta['cpu_resources']
        assert resources['effective_cpu_quota'] is None and resources['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
        binding = summary['cpu']
        assert len(binding) == 4 and binding == meta['cpu_binding'] and set(binding).issubset(allocation['selected_cpus'])
        core_of = {c: tuple(group) for group in resources['logical_cpus_by_physical_core'] for c in group}
        cores = {(resources['hostname'], core_of[c]) for c in binding}
        assert len(cores) == 4
        begin, end = (datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc', 'finished_utc'])
        for other_begin, other_end, other_cores in intervals:
            assert not (begin < other_end and other_begin < end and cores.intersection(other_cores))
        intervals.append((begin, end, cores))
        log = (raw / name / 'WAREHOUSE.log').read_text().splitlines()
        assert not any(line.startswith(('[cgar-temporal-score]', '[cgar-chain-pricing]', '[cgar-flow]')) for line in log)
        components = [line for line in log if line.startswith('[CGAR_TRICK_COMPONENTS] ')]
        assert len(components) == 1
        actual = dict(token.split('=',1) for token in components[0].split()[1:]); actual.setdefault('remaining_flow','0')
        expected = dict(instance='WAREHOUSE',lanes='1',short_tasks=str(short),matching='1',remaining_flow='0',hrrn=str(1-short),oldest_admission=str(1-short),started_tasks='protected')
        assert actual == expected
        declared=dict(meta['expected_trick_components']);declared.setdefault('remaining_flow',0)
        assert declared == {k:int(v) for k,v in expected.items() if k not in ('instance','started_tasks')}
        scheduler=[fields(line) for line in log if line.startswith('[cgar-scheduler] ')]
        assert scheduler and all(x['pickup_weight']==weight for x in scheduler)
        field_receipts = [line for line in log if line.startswith('[CGAR_TRICK] ')]
        assert len(field_receipts) == 1
        field = dict(token.split('=',1) for token in field_receipts[0].split()[1:])
        assert field['instance']=='WAREHOUSE' and field['field_sha256']==meta['expected_trick_field_sha256']
        assert field['forward_base']=='4' and field['opposing']=='16' and field['turn']=='4' and field['learned_publications']=='disabled'

        matching = [fields(line) for line in log if line.startswith('[cgar-unopened-match] t=')]
        assert matching and [x['t'] for x in matching] == list(range(0, horizon, 200)) if summary['valid'] else True
        for x in matching:
            assert x['enabled'] == mode
            if not mode:
                assert all(v == 0 for k, v in x.items() if k not in ('t', 'enabled'))
                continue
            assert x['passes'] == x['t'] // 10 + 1
            assert x['groups'] <= x['anchors'] <= quota * x['passes']
            assert 2 * x['groups'] <= x['selected'] <= 32 * x['groups']
            assert x['full_groups'] <= x['groups']
            assert x['nodes'] <= 2048 * x['anchors']
            assert x['matrix_entries'] <= 32 * x['selected']
            assert x['moved'] <= x['selected'] and x['accepted_cycles'] <= x['cycles']
            assert x['eligible'] >= x['resident'] + x['missing']
        if mode:
            assert any('local_pool=all_resident' in line for line in log if line.startswith('[cgar-unopened-match] enabled=1'))
        receipts[name] = {'matching': mode, 'pickup_weight':weight, 'group_quota': quota, 'short_tasks': short, 'explicit_trick_track_verified': True, 'sampled_cumulative_matching': matching}

        if summary['valid']:
            assert summary['makespan'] == summary['entry_compute_samples'] == horizon
            assert summary['entry_compute_max_seconds'] <= spec['time_limit_ms'] / 1000 and summary['peak_process_rss_bytes'] < 32000000000
            global_rows = [fields(line) for line in log if line.startswith('[cgar-temporal]')]
            regions = [fields(line) for line in log if line.startswith('[cgar-temporal-regions]')]
            expected_steps = list(range(200, horizon + 1, 200))
            assert [x['step'] for x in global_rows] == [x['step'] for x in regions] == expected_steps
            for x in global_rows:
                assert x['candidate_limit'] == 4000000 and x['workers'] == x['threads'] == 1
                assert x['selected_worker'] == 0
            starts = [fields(line) for line in log if line.startswith('[cgar-temporal-starts]')]
            assert [x['step'] for x in starts] == expected_steps
            for x in starts:
                assert x['mixed'] == x['warm'] == x['selected_warm'] == x['warm_runs'] == 0
                assert x['cold'] == 1 and x['cold_runs'] == x['step']
            for x in regions:
                assert x['regions'] == x['threads'] == 4 and x['rounds'] == 2 and x['temperature_ppm'] == 1000
                assert x['repairs'] == (x['kept'] + x['reverted']) * 25000 and x['score_after'] + 1e-6 >= x['score_before']

    assert all(set(arms) == {(0,1),(0,5),(1,1),(1,5)} for arms in by_seed.values())

    helpers = support / 'experiments/construction-20260918'
    subprocess.run(['/usr/bin/python3', str(support / 'experiments/sequences-20260918/analyze.py'),
                    '--input', str(raw), '--output', str(out), '--workers', '1'], check=True)
    sys.path.insert(0, str(helpers))
    verifier = importlib.import_module('verify_full')
    verifier.ROOT = ROOT
    result = verifier.verify(raw, out, a.commit, allow_failed=True, decision_limit_ms=spec['time_limit_ms'])
    write(out / 'verification.json', result)
    reference = {(r['seed'],int(r['environment']['CGAR_TRICK_SHORT_TASKS'])):r
                 for r in read(support / 'reference.json')['rows'] if r['environment']['CGAR_TRICK_UNOPENED_MATCH']=='1'}
    controls = {(r['seed'],int(r['environment']['CGAR_TRICK_SHORT_TASKS'])):r
                for r in result['rows'] if r['environment'][KEY]=='1'}
    for key,row in controls.items():
        assert row['tasks']==reference[key]['tasks'] and row['trajectory_sha256']==reference[key]['trajectory_sha256']
    metrics={row['case']:row for row in read(out/'metrics.json')}
    pairs=[]
    for row in result['rows']:
        short=int(row['environment']['CGAR_TRICK_SHORT_TASKS']);key=(row['seed'],short)
        if row['environment'][KEY]!='5' or key not in controls:continue
        control=controls[key]
        pairs.append(dict(seed=row['seed'],short_tasks=short,tasks=row['tasks'],control_tasks=control['tasks'],
             task_difference=row['tasks']-control['tasks'],final1000_difference=row['final1000']-control['final1000'],
             age_p90_difference=row['outstanding_age_p90']-control['outstanding_age_p90'],
             empty_robot_step_difference=row['empty_robot_steps']-control['empty_robot_steps'],
             empty_steps_per_completed_task=row['empty_robot_steps']/row['tasks'],
             control_empty_steps_per_completed_task=control['empty_robot_steps']/control['tasks'],
             candidate_per1000=metrics[row['case']]['completed_per_1000'],control_per1000=metrics[control['case']]['completed_per_1000']))
    complete=not result['failures'] and len(pairs)==2*len(seeds)
    result.update(full_run=True,exact_control_arms=[list(k) for k in sorted(controls)],pairs=pairs,
         mean_effect_percent={str(short):(statistics.mean(r['tasks'] for r in pairs if r['short_tasks']==short)/statistics.mean(r['control_tasks'] for r in pairs if r['short_tasks']==short)-1)*100 for short in (0,1)} if complete else None,
         complete_all_pairs=complete,promoted=False)
    result.update(decision_limit_ms=spec['time_limit_ms'], benchmark_mode=spec.get('benchmark_mode', 'competition_budget'),
                  competition_budget_confirmed=spec['time_limit_ms'] == 1000 and spec.get('exclusive_host', True) and result.get('all_valid', result.get('all_valid_within_deadline_and_memory', False)))
    result.update(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), source_commit=a.commit,
                  source_files_verified=len(sources), binary_sha256=build['binary_sha256'], receipts=receipts,
                  scope='TRICK: explicit --trick WAREHOUSE, static lanes and matching64 fixed; pickup weight1 versus5 crossed with short preference OFF/ON. No generic or native NMS equivalence claim. Short preference can defer long unpicked tasks indefinitely. Full controls must exactly match V65 matched lanes and matched lanes+short trajectories. Failures have no partial quality score; sampled counters are not complete totals.')
    for name in ['trick-pickup-weight-analysis-request.json', 'trick-pickup-weight-analysis-submission.json']:
        shutil.copy2(raw / name, out / name)
    write(out / 'comparison.json', result)
    print('TRICK_PICKUP_WEIGHT_ANALYSIS_COMPLETE', json.dumps({k: result.get(k) for k in ['full_run', 'all_valid', 'pairs', 'mean_effect_percent', 'eligible_for_full_comparison']}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
