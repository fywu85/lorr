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
CHANGED = {KEY, 'CGAR_TRICK_UNOPENED_MATCH', 'CGAR_TRICK_LANES', 'CGAR_TRICK_SHORT_TASKS', 'CGAR_TRICK_NATIVE_METRIC', 'CGAR_TRICK_NATIVE_BANDS', 'CGAR_FLOW_COST_SCALE'}


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
        for name, source in [('reference.json', BASE / 'results/trick-potential-full-v68/verification.json'),
                             ('legacy-paid-screen.json', BASE / 'results/trick-short-tasks-screen-v58/trajectory-fingerprints.json'),
                             ('reference-profile.json', BASE / 'warehouse-regional-reference-variants.json'),
                             ('reference-screen.json', BASE / 'results/trick-potential-screen-v68/trajectory-fingerprints.json')]:
            shutil.copy2(source, support / name)
            files.append(support / name)
        write(raw / 'trick-native-analysis-request.json', {'root': str(ROOT), 'commit': a.commit,
              'files': {str(f): digest(f) for f in files}, 'raw': str(raw), 'output': str(out)})
        command = ['/usr/bin/python3', str(own), '--execute', '--raw', str(raw), '--output', str(out), '--commit', a.commit]
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
    assert len(cases) == 4 * len(seeds)
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
        arm = mode
        assert env['CGAR_TRICK_LANES'] == '1' and short == 0 and env['CGAR_TRICK_UNOPENED_MATCH'] == '0'
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
        components = [line for line in log if line.startswith('[CGAR_TRICK_COMPONENTS] ')]
        assert len(components) == 1
        actual = dict(token.split('=',1) for token in components[0].split()[1:])
        expected = dict(instance='WAREHOUSE',lanes='1',short_tasks='0',matching='0',remaining_flow=str(remaining),native_metric=str(native),native_bands=str(bands),hrrn='1',oldest_admission='1',started_tasks='protected')
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
            assert all(v == 0 for k,v in x.items() if k != 't')
        receipts[name] = {'remaining_flow':remaining,'native_metric':native,'native_bands':bands,'mode':mode,'field_sha256':field['field_sha256'],'native_diagnostics':native_rows,
                          'matching_disabled':True, 'explicit_trick_track_verified':True}

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

    assert all(set(arms) == {0,1,2,3} for arms in by_seed.values())

    helpers = support / 'experiments/construction-20260918'
    if horizon == 200:
        subprocess.run(['/usr/bin/python3', str(helpers / 'collect_cold.py'), '--input', str(raw), '--output', str(out)], check=True)
        summaries = read(out / 'run-summaries.json')
        all_valid = all(v[0]['valid'] for v in summaries.values())
        if all_valid:
            fingerprints = read(out / 'trajectory-fingerprints.json')
            reference_screen = read(support / 'reference-screen.json')
            for seed in seeds:
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
        reference={(r['seed'],int(r['environment'][KEY])):r for r in read(support/'reference.json')['rows']}
        controls={(r['seed'],int(r['environment'][KEY])):r for r in result['rows'] if r['environment']['CGAR_TRICK_NATIVE_METRIC']=='0'}
        for key,row in controls.items():
            assert row['tasks']==reference[key]['tasks'] and row['trajectory_sha256']==reference[key]['trajectory_sha256']
        metrics={row['case']:row for row in read(out/'metrics.json')};pairs=[]
        for row in result['rows']:
            if row['environment']['CGAR_TRICK_NATIVE_METRIC']!='1':continue
            bands=int(row['environment']['CGAR_TRICK_NATIVE_BANDS']);key=(row['seed'],1)
            if key not in controls:continue
            control=controls[key]
            pairs.append(dict(seed=row['seed'],bands=bands,tasks=row['tasks'],control_tasks=control['tasks'],
                 task_difference=row['tasks']-control['tasks'],final1000_difference=row['final1000']-control['final1000'],
                 age_p90_difference=row['outstanding_age_p90']-control['outstanding_age_p90'],
                 empty_robot_step_difference=row['empty_robot_steps']-control['empty_robot_steps'],
                 empty_steps_per_completed_task=row['empty_robot_steps']/row['tasks'],
                 control_empty_steps_per_completed_task=control['empty_robot_steps']/control['tasks'],
                 loaded_turn_difference=row['loaded_turns']-control['loaded_turns'],loaded_wait_difference=row['loaded_waits']-control['loaded_waits'],
                 candidate_per1000=metrics[row['case']]['completed_per_1000'],control_per1000=metrics[control['case']]['completed_per_1000']))
        complete=not result['failures'] and len(pairs)==2*len(seeds)
        result.update(full_run=True,exact_control_arms=[list(k) for k in sorted(controls)],pairs=pairs,
             mean_effect_percent={str(b):(statistics.mean(r['tasks'] for r in pairs if r['bands']==b)/statistics.mean(r['control_tasks'] for r in pairs if r['bands']==b)-1)*100 for b in (0,1)} if complete else None,
             complete_all_pairs=complete,promoted=False)
    result.update(decision_limit_ms=spec['time_limit_ms'], benchmark_mode=spec.get('benchmark_mode', 'competition_budget'),
                  competition_budget_confirmed=spec['time_limit_ms'] == 1000 and spec.get('exclusive_host', True) and result.get('all_valid', result.get('all_valid_within_deadline_and_memory', False)))
    result.update(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), source_commit=a.commit,
                  source_files_verified=len(sources), binary_sha256=build['binary_sha256'], receipts=receipts,
                  scope='TRICK: explicit --trick WAREHOUSE native metric20/200(+1bands),turn1,raw scalar units against exact4/16/4 paid/remaining controls. Scheduler pickup/chain prices are coherently repriced with base20. Matching/short preference OFF. This is a compound fidelity experiment, not a causal cheap-turn effect or an NMS reproduction. Prefixes only establish feasibility; failures have no partial score.')
    for name in ['trick-native-analysis-request.json', 'trick-native-analysis-submission.json']:
        shutil.copy2(raw / name, out / name)
    write(out / 'comparison.json', result)
    print('TRICK_NATIVE_METRIC_ANALYSIS_COMPLETE', json.dumps({k: result.get(k) for k in ['full_run', 'all_valid', 'pairs', 'mean_effect_percent', 'eligible_for_full_comparison']}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
