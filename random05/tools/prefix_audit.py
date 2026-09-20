#!/usr/bin/env python3
"""Measure whether prefixes of full fixed-work runs preserve their final ranking."""
import argparse
import datetime
import json
import math
from pathlib import Path


def ranks(values):
    order = sorted(range(len(values)), key=values.__getitem__)
    output = [0.0] * len(values)
    start = 0
    while start < len(order):
        end = start + 1
        while end < len(order) and values[order[end]] == values[order[start]]:
            end += 1
        for i in order[start:end]:
            output[i] = (start + end - 1) / 2.0
        start = end
    return output


def correlation(a, b):
    a, b = ranks(a), ranks(b)
    am, bm = sum(a) / len(a), sum(b) / len(b)
    numerator = sum((x-am)*(y-bm) for x, y in zip(a, b))
    denominator = math.sqrt(sum((x-am)**2 for x in a)*sum((y-bm)**2 for y in b))
    return numerator / denominator if denominator else None


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runs', type=Path, default=Path('runs/random05'))
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--minimum-tasks', type=int, default=2800)
    args = parser.parse_args()
    cuts = [250, 500, 750, 1000, 1250, 1500, 1750, 2000]
    records, seen = [], set()
    for batch in sorted(args.runs.iterdir()):
        if not batch.is_dir() or batch.name.startswith('build'):
            continue
        for path in sorted(batch.rglob('case.json')):
            case = json.loads(path.read_text())
            env = case.get('env', {})
            if env.get('R05_K') != '1024' or case.get('cores') != 4 or env.get('R05_HORIZON') != '2000':
                continue
            summary_path = path.with_name('summary.json')
            if not summary_path.exists():
                continue
            summary = json.loads(summary_path.read_text())
            score = summary.get('result', {}).get('numTaskFinished', 0)
            if not summary.get('valid') or summary.get('steps') != 2000 or score < args.minimum_tasks:
                continue
            signature = summary['trajectory_sha256']
            if signature in seen:
                continue
            seen.add(signature)
            result = json.loads(path.with_name('result.json').read_text())
            lengths = {t[0]: len(t[2])//2 for t in result['tasks']}
            completions = [e[0] for e in result['events'] if e[3] == lengths[e[2]]]
            assert len(completions) == score, (str(path), len(completions), score)
            counts = {str(c): sum(t <= c for t in completions) for c in cuts}
            assert counts['2000'] == score
            records.append(dict(case=str(path), final=score, trajectory_sha256=signature,
                                environment=env, prefixes=counts))
    assert len(records) >= 4, 'need at least four independent trajectories'
    best = max(r['final'] for r in records)
    near_best = [i for i, r in enumerate(records) if r['final'] >= best * 0.98]
    report = []
    for cut in cuts:
        values = [r['prefixes'][str(cut)] for r in records]
        threshold = sorted(values, reverse=True)[max(1, math.ceil(len(values)/3))-1]
        survivors = [i for i, value in enumerate(values) if value >= threshold]
        selected = max(range(len(values)), key=values.__getitem__)
        row = dict(steps=cut, spearman=correlation(values, [r['final'] for r in records]),
                   retained_in_top_third=len(survivors), near_best_total=len(near_best),
                   near_best_retained=sum(i in survivors for i in near_best),
                   prefix_winner_final_tasks=records[selected]['final'])
        report.append(row)
        print(row)
    output = dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  population='Unique valid K1024, four-core, known-horizon 2000 trajectories; final tasks >= {}'.format(args.minimum_tasks),
                  caveat='Retrospective, selected population; correlations do not establish prospective screening reliability.',
                  trajectories=len(records), best=best, metrics=report, runs=records)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2)+'\n')


if __name__ == '__main__':
    main()
