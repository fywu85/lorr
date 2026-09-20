#!/usr/bin/env python3
"""Verify native Warehouse metric against exact paid and remaining-potential controls."""
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
KEY = 'CGAR_TRICK_REMAINING_FLOW'
CHANGED = {KEY, 'CGAR_TRICK_UNOPENED_MATCH', 'CGAR_TRICK_LANES', 'CGAR_TRICK_SHORT_TASKS', 'CGAR_TRICK_NATIVE_METRIC', 'CGAR_TRICK_NATIVE_BANDS', 'CGAR_FLOW_COST_SCALE', 'CGAR_REASSIGN_MATCH_GROUPS', 'CGAR_PICKUP_WEIGHT'}


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
    p.add_argument('--matching-study', action='store_true', help='Native fields with/without matching64, using exact V73 native controls.')
    p.add_argument('--pickup-study',action='store_true',help='With matching study, add weight5 matched arm for each native field.')
    p.add_argument('--short-study', action='store_true', help='Native band field and matching64: weights1/5 crossed with explicit short preferenceOFF/ON.')
    p.add_argument('--execute', action='store_true')
    a = p.parse_args()
    if a.pickup_study and not a.matching_study:p.error('--pickup-study requires --matching-study')
    if a.short_study and (a.matching_study or a.pickup_study):p.error('--short-study is a separate four-arm experiment')
    raw, out = a.raw.resolve(), a.output.resolve()
    support = raw / 'trick-native-analysis-support'
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
        full_matching = a.matching_study and (read(raw/'spec.json')['horizons'] or {'WAREHOUSE':5000})['WAREHOUSE']==5000
        short_reference = 'results/native-assignment-full-v78/verification.json' if (read(raw/'spec.json')['horizons'] or {'WAREHOUSE':5000})['WAREHOUSE']==5000 else 'results/trick-potential-full-v68/verification.json'
        for name, source in [('reference.json', BASE / (short_reference if a.short_study else ('results/native-metric-full-v73/verification.json' if full_matching else 'results/trick-potential-full-v68/verification.json'))),
                             ('legacy-paid-screen.json', BASE / 'results/trick-short-tasks-screen-v58/trajectory-fingerprints.json'),
                             ('matching-screen.json', BASE / 'results/native-match-screen-v75/trajectory-fingerprints.json'),
                             ('reference-profile.json', BASE / 'warehouse-regional-reference-variants.json'),
                             ('reference-screen.json', BASE / ('results/native-assignment-screen-v78/trajectory-fingerprints.json' if a.short_study else ('results/native-metric-screen-v73/trajectory-fingerprints.json' if a.matching_study else 'results/trick-potential-screen-v68/trajectory-fingerprints.json')))]:
            shutil.copy2(source, support / name)
            files.append(support / name)
        write(raw / 'trick-native-analysis-request.json', {'root': str(ROOT), 'commit': a.commit,
              'files': {str(f): digest(f) for f in files}, 'raw': str(raw), 'output': str(out), 'matching_study':a.matching_study, 'pickup_study':a.pickup_study, 'short_study':a.short_study})
        command = ['/usr/bin/python3', str(own), '--execute', '--raw', str(raw), '--output', str(out), '--commit', a.commit]
        if a.matching_study: command.append('--matching-study')
        if a.pickup_study: command.append('--pickup-study')
        if a.short_study: command.append('--short-study')
        job = raw / 'trick-native-analysis.sh'
        job.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        submit = ['qsub', '-h', '-terse', '-w', 'n', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=false,h_rt=00:30:00,h_vmem=8G', '-m', 'n',
                  '-N', 'trick_native_metric', '-j', 'y', '-o', str(raw / 'trick-native-analysis.log'), '-S', '/bin/bash']
        if a.hold_job:
            submit += ['-hold_jid', a.hold_job]
        submit.append(str(job))
        result = subprocess.run(submit, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        receipt = dict(command=submit, returncode=result.returncode, response=result.stdout)
        write(raw / 'trick-native-analysis-submission.json', receipt)
        write(raw / 'motion-analysis-submission.json', receipt)
        print(result.stdout, end='')
        if result.returncode:
            return result.returncode
        assert re.fullmatch(r'\d+\s*', result.stdout), result.stdout
        subprocess.run(['qrls', result.stdout.strip()], check=True)
        return 0

    request = read(raw / 'trick-native-analysis-request.json')
    assert request['commit'] == a.commit and Path(request['root']) == ROOT
    assert request.get('matching_study',False)==a.matching_study
    assert request.get('pickup_study',False)==a.pickup_study
    assert request.get('short_study',False)==a.short_study
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
    assert horizon in (200, 5000)
    profile = next(iter(read(support / 'reference-profile.json').values()))
    cases = spec['cases']
    seeds = sorted({c['seed'] for c in cases})
    assert (seeds == [0] if horizon == 200 else seeds in ([0, 2], [1, 3, 4, 5], [0, 1, 2, 3, 4, 5]))
    assert len(cases) == (6 if a.pickup_study else 4) * len(seeds)
    by_seed = {seed: {} for seed in seeds}
    receipts, intervals = {}, []
    allocation = read(raw / 'allocation.json')
    for case in cases:
        name, env = case['name'], case['environment']
        remaining=int(env[KEY]);native=int(env['CGAR_TRICK_NATIVE_METRIC']);bands=int(env['CGAR_TRICK_NATIVE_BANDS'])
        mode=2+bands if native else remaining
        assert native in (0,1) and bands in (0,1) and (not bands or native) and (not native or remaining==1)
        assert int(env['CGAR_FLOW_COST_SCALE'])==(20 if native else 4)
        short = int(env['CGAR_TRICK_SHORT_TASKS'])
        matching_enabled=int(env['CGAR_TRICK_UNOPENED_MATCH']);quota=int(env.get('CGAR_REASSIGN_MATCH_GROUPS','4'))
        assert matching_enabled in (0,1) and quota==(64 if matching_enabled else 4)
        assert (native==remaining==bands==matching_enabled==1 if a.short_study else (native==remaining==1 if a.matching_study else matching_enabled==0))
        weight=int(env['CGAR_PICKUP_WEIGHT'])
        assert weight in ((1,5) if a.short_study or (a.pickup_study and matching_enabled) else (1,))
        arms_per_field=3 if a.pickup_study else 2
        arm = arms_per_field*bands+(2 if weight==5 else matching_enabled) if a.matching_study else mode
        if a.short_study: arm=2*int(weight==5)+short
        assert env['CGAR_TRICK_LANES'] == '1' and short in ((0,1) if a.short_study else (0,))
        assert mode in (0, 1, 2, 3) and arm not in by_seed[case['seed']]
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
        assert not any(line.startswith(('[cgar-chain-pricing]', '[cgar-flow]')) for line in log)
        score_receipts = [line for line in log if line.startswith('[cgar-temporal-score]')]
        assert score_receipts == (['[cgar-temporal-score] remaining_flow=1 paid_forward_extra=0'] if mode else [])
        scheduler=[fields(line) for line in log if line.startswith('[cgar-scheduler] ')]
        assert scheduler and all(row['pickup_weight']==weight for row in scheduler)
        components = [line for line in log if line.startswith('[CGAR_TRICK_COMPONENTS] ')]
        assert len(components) == 1
        actual = dict(token.split('=',1) for token in components[0].split()[1:])
        expected = dict(instance='WAREHOUSE',lanes='1',short_tasks=str(short),matching=str(matching_enabled),remaining_flow=str(remaining),native_metric=str(native),native_bands=str(bands),hrrn=str(1-short),oldest_admission=str(1-short),started_tasks='protected')
        assert actual == expected
        assert meta['expected_trick_components'] == {k:int(v) for k,v in expected.items() if k not in ('instance','started_tasks')}
        field_receipts = [line for line in log if line.startswith('[CGAR_TRICK] ')]
        assert len(field_receipts) == 1
        field = dict(token.split('=',1) for token in field_receipts[0].split()[1:])
        assert field['instance']=='WAREHOUSE' and field['field_sha256']==meta['expected_trick_field_sha256']
        assert field['learned_publications']=='disabled'
        if native:
            assert field['provider']=='nms-native-metric' and field['forward_base']=='20' and field['opposing']=='200' and field['turn']=='1'
            assert field['band']==str(bands) and field['score']=='pure_potential' and field['tie']=='raw'
            if a.matching_study or a.short_study or 'installed_fnv1a64' in field:
                assert int(field['installed_fnv1a64'])==(18323636419238823753 if bands else 11595014239903881941)
        else:
            assert field['provider']=='nms-lane-directions' and field['forward_base']=='4' and field['opposing']=='16' and field['turn']=='4'
        native_rows=[fields(line) for line in log if line.startswith('[cgar-native-metric]')]
        assert bool(native_rows)==bool(native)
        if native:
            assert [x['t'] for x in native_rows]==list(range(0,horizon,200)) if summary['valid'] else True
            assert all(x['forward_base']==20 and x['turn']==1 and x['cost_limit']==200+bands and x['wide_fallback_tables']>=0 for x in native_rows)
            assert [x['wide_fallback_tables'] for x in native_rows]==sorted(x['wide_fallback_tables'] for x in native_rows)


        matching = [fields(line) for line in log if line.startswith('[cgar-unopened-match] t=')]
        assert matching and [x['t'] for x in matching] == list(range(0, horizon, 200)) if summary['valid'] else True
        for x in matching:
            assert x['enabled']==matching_enabled
            if not matching_enabled:
                assert all(v==0 for k,v in x.items() if k!='t')
                continue
            assert x['passes']==x['t']//10+1
            assert x['groups']<=x['anchors']<=quota*x['passes']
            assert 2*x['groups']<=x['selected']<=32*x['groups']
            assert x['full_groups']<=x['groups'] and x['nodes']<=2048*x['anchors']
            assert x['matrix_entries']<=32*x['selected'] and x['moved']<=x['selected']
            assert x['accepted_cycles']<=x['cycles'] and x['eligible']>=x['resident']+x['missing']
        if matching_enabled:assert any('local_pool=all_resident' in line for line in log if line.startswith('[cgar-unopened-match] enabled=1'))
        receipts[name] = {'remaining_flow':remaining,'native_metric':native,'native_bands':bands,'mode':mode,'field_sha256':field['field_sha256'],'native_diagnostics':native_rows,
                          'matching_disabled':not matching_enabled,'matching_group_quota':quota,'pickup_weight':weight,'short_preference':short,'sampled_cumulative_matching':matching, 'explicit_trick_track_verified':True}

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
                assert x['unit_cost']==(20 if native else 4) and x['turn_cost']==(1 if native else 4) and x['turn_surcharge']==0
            for x in regions:
                assert x['regions'] == x['threads'] == 4 and x['rounds'] == 2 and x['temperature_ppm'] == 1000
                assert x['repairs'] == (x['kept'] + x['reverted']) * 25000 and x['score_after'] + 1e-6 >= x['score_before']

    assert all(set(arms)==set(range(6 if a.pickup_study else 4)) for arms in by_seed.values())

    helpers = support / 'experiments/construction-20260918'
    if horizon == 200:
        subprocess.run(['/usr/bin/python3', str(helpers / 'collect_cold.py'), '--input', str(raw), '--output', str(out)], check=True)
        summaries = read(out / 'run-summaries.json')
        all_valid = all(v[0]['valid'] for v in summaries.values())
        if all_valid:
            fingerprints = read(out / 'trajectory-fingerprints.json')
            reference_screen = read(support / 'reference-screen.json')
            for seed in seeds:
                if a.short_study:
                    for w in (1,5):
                        assert fingerprints[by_seed[seed][2*int(w==5)]]==reference_screen['trick_native_band1_match1_pickup%d-s%d-r0'%(w,seed)]
                elif a.matching_study:
                    assert fingerprints[by_seed[seed][0]]==reference_screen['trick_native_no_bands-s%d-r0'%seed]
                    assert fingerprints[by_seed[seed][arms_per_field]]==reference_screen['trick_native_bands-s%d-r0'%seed]
                    if a.pickup_study:
                        previous=read(support/'matching-screen.json')
                        for band in (0,1):
                            assert fingerprints[by_seed[seed][3*band+1]]==previous['trick_native_band%d_match1-s%d-r0'%(band,seed)]
                else:
                    assert fingerprints[by_seed[seed][0]] == read(support/'legacy-paid-screen.json')['trick_lanes-s%d-r0' % seed]
                    assert fingerprints[by_seed[seed][1]] == reference_screen['trick_lanes_potential1-s%d-r0' % seed]
        result = dict(all_valid=all_valid, eligible_for_full_comparison=all_valid, full_run=False,
                      exact_control=all_valid, all_sampled_work_bounds_checked=all_valid)
    else:
        subprocess.run(['/usr/bin/python3', str(support / 'experiments/sequences-20260918/analyze.py'),
                        '--input', str(raw), '--output', str(out), '--workers', '1'], check=True)
        sys.path.insert(0, str(helpers))
        verifier = importlib.import_module('verify_full')
        verifier.ROOT = ROOT
        result = verifier.verify(raw, out, a.commit, allow_failed=True, decision_limit_ms=spec['time_limit_ms'])
        write(out / 'verification.json', result)
        group_key='CGAR_PICKUP_WEIGHT' if a.short_study else ('CGAR_TRICK_NATIVE_BANDS' if a.matching_study else KEY)
        reference={(r['seed'],int(r['environment'][group_key])):r for r in read(support/'reference.json')['rows']
                   if ((r['environment']['CGAR_TRICK_NATIVE_BANDS']==r['environment']['CGAR_TRICK_UNOPENED_MATCH']=='1') if a.short_study else (not a.matching_study or r['environment']['CGAR_TRICK_NATIVE_METRIC']=='1'))}
        control_key='CGAR_TRICK_SHORT_TASKS' if a.short_study else ('CGAR_TRICK_UNOPENED_MATCH' if a.matching_study else 'CGAR_TRICK_NATIVE_METRIC')
        controls={(r['seed'],int(r['environment'][group_key])):r for r in result['rows'] if r['environment'][control_key]=='0'}
        for key,row in controls.items():
            assert row['tasks']==reference[key]['tasks'] and row['trajectory_sha256']==reference[key]['trajectory_sha256']
        metrics={row['case']:row for row in read(out/'metrics.json')};pairs=[]
        for row in result['rows']:
            if row['environment'][control_key]!='1':continue
            bands=int(row['environment']['CGAR_TRICK_NATIVE_BANDS']);key=(row['seed'],int(row['environment']['CGAR_PICKUP_WEIGHT']) if a.short_study else (bands if a.matching_study else 1))
            if key not in controls:continue
            control=controls[key]
            pairs.append(dict(seed=row['seed'],bands=bands,pickup_weight=int(row['environment']['CGAR_PICKUP_WEIGHT']),tasks=row['tasks'],control_tasks=control['tasks'],
                 task_difference=row['tasks']-control['tasks'],final1000_difference=row['final1000']-control['final1000'],
                 age_p90_difference=row['outstanding_age_p90']-control['outstanding_age_p90'],
                 empty_robot_step_difference=row['empty_robot_steps']-control['empty_robot_steps'],
                 empty_steps_per_completed_task=row['empty_robot_steps']/row['tasks'],
                 control_empty_steps_per_completed_task=control['empty_robot_steps']/control['tasks'],
                 loaded_turn_difference=row['loaded_turns']-control['loaded_turns'],loaded_wait_difference=row['loaded_waits']-control['loaded_waits'],
                 candidate_per1000=metrics[row['case']]['completed_per_1000'],control_per1000=metrics[control['case']]['completed_per_1000']))
        complete=not result['failures'] and len(pairs)==(4 if a.pickup_study else 2)*len(seeds)
        effect_key='pickup_weight' if a.short_study else 'bands'
        effect_values=(1,5) if a.short_study else (0,1)
        result.update(full_run=True,exact_control_arms=[list(k) for k in sorted(controls)],pairs=pairs,
             mean_effect_percent={str(b):(statistics.mean(r['tasks'] for r in pairs if r[effect_key]==b)/statistics.mean(r['control_tasks'] for r in pairs if r[effect_key]==b)-1)*100 for b in effect_values} if complete else None,
             complete_all_pairs=complete,promoted=False)
        if a.pickup_study:
            result['mean_effect_percent']={f'band{b}_weight{w}':(statistics.mean(r['tasks'] for r in pairs if r['bands']==b and r['pickup_weight']==w)/statistics.mean(r['control_tasks'] for r in pairs if r['bands']==b and r['pickup_weight']==w)-1)*100 for b in (0,1) for w in (1,5)} if complete else None
            matched={(r['seed'],int(r['environment']['CGAR_TRICK_NATIVE_BANDS'])):r for r in result['rows'] if r['environment']['CGAR_TRICK_UNOPENED_MATCH']=='1' and r['environment']['CGAR_PICKUP_WEIGHT']=='1'}
            increments=[]
            for row in result['rows']:
                if row['environment']['CGAR_PICKUP_WEIGHT']!='5':continue
                key=(row['seed'],int(row['environment']['CGAR_TRICK_NATIVE_BANDS']))
                if key not in matched:continue
                control=matched[key]
                increments.append(dict(seed=key[0],bands=key[1],tasks=row['tasks'],control_tasks=control['tasks'],task_difference=row['tasks']-control['tasks'],final1000_difference=row['final1000']-control['final1000'],age_p90_difference=row['outstanding_age_p90']-control['outstanding_age_p90'],empty_robot_step_difference=row['empty_robot_steps']-control['empty_robot_steps']))
            result['pickup_increments']=increments
            result['mean_pickup_increment_percent']={str(b):(statistics.mean(r['tasks'] for r in increments if r['bands']==b)/statistics.mean(r['control_tasks'] for r in increments if r['bands']==b)-1)*100 for b in (0,1)} if complete else None

    result.update(decision_limit_ms=spec['time_limit_ms'], benchmark_mode=spec.get('benchmark_mode', 'competition_budget'),
                  competition_budget_confirmed=spec['time_limit_ms'] == 1000 and spec.get('exclusive_host', True) and result.get('all_valid', result.get('all_valid_within_deadline_and_memory', False)))
    result.update(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), source_commit=a.commit,
                  source_files_verified=len(sources), binary_sha256=build['binary_sha256'], receipts=receipts,
                  scope='TRICK: explicit --trick WAREHOUSE native metric20/200(+1bands),turn1,raw scalar units against exact4/16/4 paid/remaining controls. Scheduler pickup/chain prices are coherently repriced with base20. Matching/short preference OFF. This is a compound fidelity experiment, not a causal cheap-turn effect or an NMS reproduction. Prefixes only establish feasibility; failures have no partial score.')
    result['matching_study']=a.matching_study
    result['pickup_study']=a.pickup_study
    result['short_study']=a.short_study
    if a.matching_study:
        result['scope']='TRICK: explicit --trick WAREHOUSE. Native no-band/band fields crossed with bounded matchingOFF/64. Remaining potential, raw scalar, scale20, turn1 and fair admissions fixed; short preference OFF. Native controls must exactly reproduce V73. Check installed FNV, complete work, bounds, timings and memory; no partial quality score or NMS policy equivalence.'
    if a.pickup_study:
        result['scope']='TRICK: explicit --trick WAREHOUSE. Each native field has three arms: matchingOFF/weight1, matching64/weight1, matching64/weight5. Remaining potential, raw scalar, scale20, turn1 and fair admissions fixed; short preference OFF. Four full OFF controls must exactly reproduce V73; startup matched/weight1 controls must exactly reproduce V75. Report matching and conditional pickup increments separately; no missing weight5/unmatched interaction is inferred. Installed FNV, source, complete work, timings and memory are checked. No partial quality score.'
    if a.short_study:
        result['scope']='TRICK: explicit --trick WAREHOUSE. Native band field, bounded matching64, remaining potential, raw scalar, scale20 and turn1 fixed. Pickup weights1/5 crossed with short preferenceOFF/ON. Short preference removes age-based HRRN and oldest-task admission only; started tasks remain protected. Four full fair controls must exactly reproduce V78; both startup fair controls must exactly reproduce V78. Report short-preference effects separately by pickup weight, plus age and travel diagnostics. No partial quality score, causal share of NMS lead, or strict1s certification.'
    for name in ['trick-native-analysis-request.json', 'trick-native-analysis-submission.json']:
        shutil.copy2(raw / name, out / name)
    write(out / 'comparison.json', result)
    print('TRICK_NATIVE_METRIC_ANALYSIS_COMPLETE', json.dumps({k: result.get(k) for k in ['full_run', 'all_valid', 'pairs', 'mean_effect_percent', 'eligible_for_full_comparison']}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
