#!/usr/bin/env python3
"""Report the frozen ten-seed wrap-up without pooling profiles or duplicate seeds."""
import argparse
import datetime
from decimal import Decimal, InvalidOperation
import hashlib
import json
from pathlib import Path
import re
import statistics

ROOT = Path(__file__).resolve().parents[2]


def read(path):
    return json.loads(path.read_text())


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def normalized(value):
    try:
        return str(Decimal(value).normalize())
    except InvalidOperation:
        return value


def settings(case):
    return {key: normalized(value) for key, value in case['env'].items() if key != 'R05_SEED'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--protocol', type=Path, required=True)
    parser.add_argument('--audit', type=Path, required=True)
    args = parser.parse_args()
    protocol = read(args.protocol)
    audit = read(args.audit)
    assert audit['complete'], 'Finish every declared attempt and its independent audit'
    rows = {}
    source_map = read(args.protocol.parent / protocol['source_map'])
    new_cases = read(args.protocol.parent / protocol['cases'])
    expected_names = {case['name'] for case in new_cases}
    assert len(new_cases) == protocol['new_distinct_seed_runs']
    assert {row['name'] for row in audit['rows']} == expected_names

    def accept(row, spec_path, historical):
        case = read(spec_path)['cases'][0]
        instance = row['instance']
        seed = int(case['env']['R05_SEED'])
        key = (instance, seed)
        assert key not in rows, 'Duplicate planner seed is not another observation'
        reference = protocol['selected_profiles'][instance]['case']
        assert settings(case) == settings(reference), 'Mixed solver settings'
        assert case['binary_sha256'] == reference['binary_sha256'], 'Mixed binary versions'
        assert case['input_hashes'] == reference['input_hashes'], 'Mixed task/start input'
        assert all(case[k] == reference[k] for k in ('steps', 'limit_ms', 'preprocess_ms', 'cores', 'smt', 'trick'))
        assert case['limit_ms'] == 1000 and case['preprocess_ms'] == 30000
        assert seed in protocol['planner_seeds']
        source = source_map[case['binary_sha256']]['source']
        assert source.startswith(row['source_commit']) or row['source_commit'].startswith(source)
        result = dict(instance=instance, seed=seed, historical=historical,
                      name=row['name'], valid=row['valid'], source_commit=source,
                      binary_sha256=case['binary_sha256'], evidence=row['evidence'])
        if row['valid']:
            assert row['replay']['replay_valid'] and row['waiting']['event_accounting_valid']
            assert row['latency_seconds']['max'] <= 1 and row['peak_rss_bytes'] <= 32000000000
            assert row['replay']['steps'] == reference['steps']
            result.update(tasks=row['tasks'], latency_seconds=row['latency_seconds'],
                          peak_rss_bytes=row['peak_rss_bytes'], finished_utc=row['finished_utc'],
                          result_sha256=row['replay']['result_sha256'])
        else:
            result['tasks'] = None
            summary = read(ROOT / row['evidence'])[0]
            result['exit'] = summary['exit']
            # Failure files have no valid full score; preserve the measured
            # reason separately instead of turning it into a zero observation.
            log = ROOT / 'runs/random05' / Path(row['evidence']).relative_to('random05/results').parent / row['name'] / 'native.log'
            if log.exists():
                matches = re.findall(r'R05_TIMEOUT phase=(\w+) timestep=(-?\d+) elapsed_ms=([\d.]+)', log.read_text())
                result['timeouts'] = [dict(phase=p, timestep=int(t), elapsed_ms=float(ms)) for p, t, ms in matches]
                result['native_log_sha256'] = sha(log)
            result['failure_kind'] = summary.get('failure_kind', 'entry_timeout' if result.get('timeouts') else 'execution_failure')
        rows[key] = result

    for reused in protocol['reused']:
        old_audit = ROOT / reused['audit']
        spec = ROOT / reused['spec']
        assert sha(old_audit) == reused['audit_sha256'] and sha(spec) == reused['spec_sha256']
        row = next(row for row in read(old_audit)['rows'] if row['name'] == reused['case'])
        assert row['valid'] and row['tasks'] == reused['tasks']
        assert row['replay']['result_sha256'] == reused['result_sha256']
        accept(row, spec, True)
    for row in audit['rows']:
        accept(row, args.audit.parent / row['name'] / 'spec.json', False)
    expected = {(instance, seed) for instance in protocol['selected_profiles'] for seed in protocol['planner_seeds']}
    assert set(rows) == expected, 'Incomplete predeclared seed set'
    references = read(ROOT / 'random05/references/matched-nms-kk-combined.json')['archived']
    output = {}
    lines = ['# Selected PILOT throughput variability', '',
             'Frozen planner seeds0–9 on each archived instance; one observation per seed.',
             'All successful full runs pass independent replay, strict1,000ms entries,',
             '30-second preparation and32GB. Original binaries/settings are fixed per',
             'instance. Historical matching observations are reused; exact repeats are',
             'not extra samples. No algorithm or parameter search was performed.', '',
             '**Mean ± SD uses successful full runs only.** SD is the sample standard',
             'deviation with denominator n−1. Failed attempts remain failures and have no',
             'throughput observation. Where failures occur, these statistics describe the',
             'successful subset, not the unconditional ten-seed distribution.', '',
             '| Instance | Valid / attempted | Mean ± sample SD | Range | Best | max(NMS, KK) | Mean lead | Best lead |',
             '|---|---:|---:|---:|---:|---:|---:|---:|']
    for instance in sorted(protocol['selected_profiles']):
        attempts = [rows[(instance, seed)] for seed in protocol['planner_seeds']]
        valid = [row for row in attempts if row['valid']]
        scores = [row['tasks'] for row in valid]
        assert len(scores) >= 2, 'Sample SD needs at least two completed seeds'
        target = references[instance]['target']
        mean, sd = float(statistics.mean(scores)), statistics.stdev(scores)
        record = dict(valid_seeds=len(scores), attempted_seeds=len(attempts), failed_seeds=[r['seed'] for r in attempts if not r['valid']],
                      mean=mean, sample_standard_deviation=sd, minimum=min(scores), maximum=max(scores),
                      matched_max_nms_kk=target, mean_gain_percent=100*(mean/target-1), best_gain_percent=100*(max(scores)/target-1),
                      worst_valid_entry_ms=1000*max(r['latency_seconds']['max'] for r in valid), attempts=attempts)
        output[instance] = record
        lines.append('| {} | {}/{} | {:.1f} ± {:.1f} | {}–{} | {} | {} | {:+.2f}% | {:+.2f}% |'.format(
            instance, len(scores), len(attempts), mean, sd, min(scores), max(scores), max(scores), target,
            record['mean_gain_percent'], record['best_gain_percent']))
    lines += ['', '## Individual seeds', '',
              '| Instance | ' + ' | '.join(str(s) for s in protocol['planner_seeds']) + ' |',
              '|---|' + '---:|' * len(protocol['planner_seeds'])]
    for instance, record in output.items():
        lines.append('| ' + instance + ' | ' + ' | '.join(str(r['tasks']) if r['valid'] else 'FAIL' for r in record['attempts']) + ' |')
    failures = [row for row in rows.values() if not row['valid']]
    lines += ['', '## Timing failures', '']
    if not failures:
        lines += ['Every declared seed completes within the strict limit.']
    else:
        for row in failures:
            details = ', '.join('{} step{}: {:.3f}ms'.format(t['phase'], t['timestep'], t['elapsed_ms']) for t in row.get('timeouts', []))
            lines.append('- {} seed{}: {}. [Original attempt]({}).'.format(row['instance'], row['seed'], details or row['failure_kind'], row['evidence'].replace('random05/results/'+args.audit.parent.name+'/', '', 1)))
    lines += ['', 'These are descriptive statistics for development-selected profiles. Some',
              'reused seeds helped select those profiles. They do not establish unseen-map',
              'or fresh-task-stream variability, nor a statistical confidence claim against',
              'the maximum of the NMS/KK observations. General solvers were not re-evaluated.', '',
              'This completes the user-authorized statistical wrap-up; development is to',
              'remain paused until an explicit request to resume.', '',
              '[Frozen protocol](../../experiments/'+args.protocol.name+'), [independent audit](audit.json), [full numeric statistics](seed-statistics.json).', '']
    report = dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), protocol=str(args.protocol),
                  protocol_sha256=sha(args.protocol), new_audit_sha256=sha(args.audit),
                  estimator=protocol['estimator'], selection_limit=protocol['selection_limit'], instances=output)
    (args.audit.parent / 'seed-statistics.json').write_text(json.dumps(report, indent=2) + '\n')
    (args.audit.parent / 'REPORT.md').write_text('\n'.join(lines))
    for instance, record in output.items():
        print(instance, '{:.1f} ± {:.1f}, n={}/{}, best={}'.format(record['mean'], record['sample_standard_deviation'], record['valid_seeds'], record['attempted_seeds'], record['maximum']))


if __name__ == '__main__':
    main()
