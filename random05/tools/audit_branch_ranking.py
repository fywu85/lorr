#!/usr/bin/env python3
"""Measure continuation screening quality from complete observational runs."""
import argparse
from collections import defaultdict
import datetime
import hashlib
import json
import math
from pathlib import Path
from result_horizon import summary_steps


def screening(rows, branches, retained, accept_equal=True):
    """Keep the generation's first anchor and a fixed number of prefix leaders."""
    assert rows and 1 <= retained <= len(rows)
    assert all(1 <= branches < len(row['scores']) for row in rows)
    tie = lambda row: -row['candidate'] if accept_equal else row['candidate']
    full = sorted(rows, key=lambda row: (-row['value'], tie(row)))
    prefix = sorted(rows, key=lambda row: (-sum(row['scores'][:branches]) / branches, tie(row)))
    kept = {row['candidate'] for row in prefix[:retained]}
    anchor = min(row['candidate'] for row in rows)
    if anchor not in kept:
        kept.remove(prefix[retained - 1]['candidate'])
        kept.add(anchor)
    winner = full[0]['candidate']
    top = full[:min(8, len(full))]
    survivor_best = max(row['value'] for row in rows if row['candidate'] in kept)
    return dict(winner_survives=winner in kept,
                winner_prefix_rank=next(i + 1 for i, row in enumerate(prefix) if row['candidate'] == winner),
                top8_recall=sum(row['candidate'] in kept for row in top) / len(top),
                full_value_loss=max(0.0, full[0]['value'] - survivor_best))


def aggregate(rows):
    return dict(generations=len(rows),
                winner_survival=sum(row['winner_survives'] for row in rows) / len(rows),
                top8_recall=sum(row['top8_recall'] for row in rows) / len(rows),
                mean_full_value_loss=sum(row['full_value_loss'] for row in rows) / len(rows),
                maximum_full_value_loss=max(row['full_value_loss'] for row in rows),
                mean_winner_prefix_rank=sum(row['winner_prefix_rank'] for row in rows) / len(rows))


def audit(log):
    log = log.resolve()
    summary = json.loads(log.with_name('summary.json').read_text())
    case = json.loads(log.with_name('case.json').read_text())
    assert summary['valid'] and summary_steps(summary) == 2000, 'requires a complete valid run'
    env = case['env']
    interval = int(env['R05_BRANCH_DIAGNOSTICS'])
    assert interval > 0
    risk = float(env.get('R05_CONTINUATION_RISK', 0))
    branches = int(env['R05_CONTINUATIONS'])
    generations = int(env.get('R05_GENERATIONS', 1))
    accept_equal = bool(int(env.get('R05_EQUAL', 0)))
    grouped = defaultdict(list)
    for line in log.read_text().splitlines():
        if not line.startswith('R05_BRANCH_SCORE '):
            continue
        row = json.loads(line[len('R05_BRANCH_SCORE '):])
        assert len(row['scores']) == branches and all(math.isfinite(x) for x in row['scores'])
        mean = sum(row['scores']) / branches
        value = mean - risk * math.sqrt(sum((x - mean) ** 2 for x in row['scores']) / branches)
        assert abs(row['value'] - value) < 1e-7, 'recorded branches do not reproduce the evaluated score'
        grouped[row['t'], row['generation']].append(row)
    expected = {(step, generation) for step in range(0, 2000, interval) for generation in range(generations)}
    assert set(grouped) == expected, 'missing or unexpected diagnostic generations'
    for (step, generation), rows in grouped.items():
        work = int(env.get('R05_FIRST_K', 0)) if step == 0 else 0
        work = work or int(env['R05_K'])
        roots = work // branches
        expected_ids = list(range(generation * roots // generations, (generation + 1) * roots // generations))
        assert [row['candidate'] for row in rows] == expected_ids, 'missing, reordered or duplicated candidates'
    measurements = []
    for prefix in [1, 2, 4, 8]:
        if prefix >= branches:
            continue
        for divisor in [4, 2]:
            observations = []
            phases = defaultdict(list)
            for (step, generation), rows in sorted(grouped.items()):
                retained = max(1, (len(rows) + divisor - 1) // divisor)
                row = screening(rows, prefix, retained, accept_equal)
                observations.append(row)
                phases['early_0_499' if step < 500 else 'middle_500_1499' if step < 1500 else 'late_1500_1999'].append(row)
            measurements.append(dict(prefix_branches=prefix, retained_fraction=1 / divisor,
                                     overall=aggregate(observations),
                                     phases={name: aggregate(rows) for name, rows in phases.items()}))
    return dict(name=case['name'], log=str(log),
                log_sha256=hashlib.sha256(log.read_bytes()).hexdigest(),
                raw_result_sha256=hashlib.sha256(log.with_name('result.json').read_bytes()).hexdigest(),
                binary_sha256=summary['binary_sha256'], tasks=summary['result']['numTaskFinished'],
                full_branches=branches, sampled_steps=len(expected) // generations,
                anchor_rule='Always retain the first candidate in each generation; replace the weakest prefix survivor if necessary.',
                measurements=measurements)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--logs', nargs='+', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    report = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  interpretation='Observational screening only; no staged-search throughput or statistical guarantee is inferred.',
                  runs=[audit(path) for path in args.logs])
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    for run in report['runs']:
        print(run['name'], 'tasks', run['tasks'])
        for row in run['measurements']:
            print('  branches={} retain={} survival={:.1%} top8={:.1%} lost_value={:.3f}'.format(
                row['prefix_branches'], row['retained_fraction'], row['overall']['winner_survival'],
                row['overall']['top8_recall'], row['overall']['mean_full_value_loss']))


if __name__ == '__main__':
    main()
