#!/usr/bin/env python3
"""Verify a full regional-reference experiment, controls and cumulative diagnostics."""
import argparse
import json
from pathlib import Path
import re
import statistics
from verify_full import verify, read

HERE = Path(__file__).resolve().parent


def fields(line):
    return {k: float(v) if '.' in v else int(v)
            for k, v in re.findall(r'(\w+)=([-\d.]+)', line)}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--input', required=True, type=Path)
    p.add_argument('--output', required=True, type=Path)
    p.add_argument('--commit', required=True)
    p.add_argument('--variable', required=True, choices=[
        'CGAR_TEMPORAL_NEXT_ERRAND', 'CGAR_TEMPORAL_REGION_TEMPERATURE_PPM', 'CGAR_TEMPORAL_REGIONS', 'CGAR_TEMPORAL_DISTANCE_SCALE', 'CGAR_TEMPORAL_CANDIDATE_LIMIT'])
    p.add_argument('--control', required=True, type=int)
    args = p.parse_args()
    report = verify(args.input, args.output, args.commit)
    reference = {r['seed']: r for r in read(HERE / 'results/pickup-full-regions-six-seed-v44.json')['rows']
                 if r['environment']['CGAR_TEMPORAL_REGIONS'] == '4'}
    control = {r['seed']: r for r in report['rows'] if int(r['environment'][args.variable]) == args.control}
    arm_values = {int(r['environment'][args.variable]) for r in report['rows']}
    assert len(arm_values) >= 2 and len(control) * len(arm_values) == len(report['rows']) and sorted(control) == [0, 2]
    for seed, r in control.items():
        assert r['tasks'] == reference[seed]['tasks']
        assert r['trajectory_sha256'] == reference[seed]['trajectory_sha256'], ('control', r['case'])
    changed = {args.variable}
    if args.variable == 'CGAR_TEMPORAL_REGIONS':
        changed.add('CGAR_TEMPORAL_REGION_THREADS')
    if args.variable == 'CGAR_TEMPORAL_CANDIDATE_LIMIT':
        changed.add('CGAR_TEMPORAL_REGION_ROUNDS')
    diagnostics = {}
    for r in report['rows']:
        env = r['environment']
        base = control[r['seed']]['environment']
        assert {k: v for k, v in env.items() if k not in changed} == {k: v for k, v in base.items() if k not in changed}
        lines = (Path(r['raw_case']) / 'WAREHOUSE.log').read_text().splitlines()
        next_rows = [fields(x) for x in lines if x.startswith('[cgar-temporal-next-errand]')]
        peaks = [fields(x) for x in lines if x.startswith('[cgar-regional-peaks]')]
        region_rows = [fields(x) for x in lines if x.startswith('[cgar-temporal-regions]')]
        global_rows = [fields(x) for x in lines if x.startswith('[cgar-temporal]')]
        assert [x['step'] for x in global_rows] == list(range(200, 5001, 200))
        for x in global_rows:
            assert x['candidate_limit'] == int(env['CGAR_TEMPORAL_CANDIDATE_LIMIT'])
            assert x['workers'] == int(env['CGAR_TEMPORAL_WORKERS'])
            assert x['threads'] == int(env['CGAR_TEMPORAL_THREADS'])
            assert 0 <= x['selected_worker'] < x['workers']
        assert [x['step'] for x in region_rows] == list(range(200, 5001, 200))
        parts, rounds, steps = (int(env[k]) for k in ['CGAR_TEMPORAL_REGIONS', 'CGAR_TEMPORAL_REGION_ROUNDS', 'CGAR_TEMPORAL_REGION_STEPS'])
        for x in region_rows:
            assert x['regions'] == parts and x['rounds'] == rounds and x['threads'] == int(env['CGAR_TEMPORAL_REGION_THREADS'])
            assert x['temperature_ppm'] == int(env['CGAR_TEMPORAL_REGION_TEMPERATURE_PPM'])
            assert x['repairs'] == (x['kept'] + x['reverted']) * steps
            assert 0 < x['kept'] + x['reverted'] <= parts * rounds
            assert x['score_after'] + 1e-6 >= x['score_before']
        if env.get('CGAR_TEMPORAL_NEXT_ERRAND', '0') == '1':
            assert [x['step'] for x in next_rows] == list(range(200, 5001, 200))
            prior = None
            for x in next_rows:
                assert x['enabled'] == 1 and x['known'] == x['eligible'] + x['unavailable']
                assert 0 < x['changed_choices'] <= x['arriving_choices'] and x['eligible'] > 0
                if prior:
                    for k in ['known', 'eligible', 'unavailable', 'arriving_choices', 'changed_choices']:
                        assert x[k] >= prior[k]
                prior = x
        else:
            assert not next_rows
        if env.get('CGAR_TEMPORAL_REGION_PEAK_AUDIT', '0') == '1':
            assert [x['step'] for x in peaks] == list(range(200, 5001, 200))
            prior = None
            for x in peaks:
                assert 0 < x['batches'] <= x['step'] * parts * rounds and x['attempts'] == x['batches'] * steps
                assert 0 <= x['lost_improvements'] <= x['lost_peaks'] <= x['batches']
                assert x['peak_gain'] >= x['final_gain'] - 1e-5 and 0 <= x['discarded_gain'] <= x['peak_gain'] + 1e-5
                assert 0 <= x['max_peak_attempt'] <= steps and 0 <= x['peak_attempt_sum'] <= x['batches'] * steps
                assert 0 <= x['peak_updates'] <= x['attempts']
                if prior:
                    for k in ['batches', 'attempts', 'peak_updates', 'lost_peaks', 'lost_improvements',
                              'peak_attempt_sum', 'max_peak_attempt', 'peak_gain', 'discarded_gain']:
                        assert x[k] >= prior[k]
                prior = x
        else:
            assert not peaks
        diagnostics[r['case']] = {'next_errand': next_rows, 'cumulative_peaks': peaks, 'regional_samples': region_rows, 'global_samples': global_rows}
    values = sorted({int(r['environment'][args.variable]) for r in report['rows']} - {args.control})
    groups = []
    for value in values:
        candidates = sorted((r for r in report['rows'] if int(r['environment'][args.variable]) == value), key=lambda r: r['seed'])
        assert [r['seed'] for r in candidates] == sorted(control)
        pairs = []
        for r in candidates:
            c = control[r['seed']]
            pair = {'seed': r['seed'], 'tasks': r['tasks'], 'control_tasks': c['tasks'],
                    'task_difference': r['tasks'] - c['tasks'], 'final1000_difference': r['final1000'] - c['final1000'],
                    'age_p90_difference': r['outstanding_age_p90'] - c['outstanding_age_p90']}
            for k in ['empty_robot_steps', 'loaded_turns', 'loaded_waits']:
                pair[k + '_percent'] = (r[k] / c[k] - 1) * 100
            pairs.append(pair)
        mean = statistics.mean(r['tasks'] for r in candidates)
        baseline = statistics.mean(r['tasks'] for r in control.values())
        groups.append({'value': value, 'mean_tasks': mean, 'control_mean_tasks': baseline,
                       'mean_effect_percent': (mean / baseline - 1) * 100, 'pairs': pairs,
                       'all_tested_full_totals_improve': all(x['task_difference'] > 0 for x in pairs),
                       'status': 'first_pair_complete_not_promoted'})
    report.update(scope='Full warehouse seeds0/2 with exact confirmed regional controls. Every complete entry, source, binary, RSS and physical CPU allocation verified. Short screens are not acceptance evidence.',
                  variable=args.variable, control_value=args.control, exact_control_seeds=sorted(control),
                  groups=groups, diagnostics=diagnostics, promoted_as_benchmark_reference=False,
                  throughput_goal_complete=False)
    (args.output / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({'cases': report['full_cases'], 'groups': groups,
                      'max_entry_seconds': max(r['max_entry_seconds'] for r in report['rows']),
                      'peak_rss_bytes': max(r['peak_rss_bytes'] for r in report['rows'])}, indent=2))


if __name__ == '__main__':
    main()
