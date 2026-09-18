#!/usr/bin/env python3
"""Summarize paired CGAR matrices and check whether short prefixes predict full effects."""
import argparse
import concurrent.futures
import json
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'throughput-20260918-strict'))
from analyze import extract, effect, category


def get_metrics(item):
    result = extract(item)
    lines = item[1].with_suffix('.log').read_text().splitlines()
    for tag, key in [('[cgar-estimates]', 'last_estimate_counters'), ('[cgar]', 'last_planner_counters')]:
        matches = [s for s in lines if s.startswith(tag + ' t=')]
        result[key] = dict(field.split('=', 1) for field in matches[-1].split()[1:] if '=' in field) if matches else {}
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--control', default='baseline')
    args = parser.parse_args()
    root, out = args.input, args.output
    spec = json.loads((root / 'spec.json').read_text())
    summaries = {c['name']: json.loads((root / c['name'] / 'summary.json').read_text()) for c in spec['cases']}
    assert all(len(rows) == len(spec['instances']) for rows in summaries.values()), 'incomplete matrix'
    valid = [(c, r) for c in spec['cases'] for r in summaries[c['name']] if r['valid']]
    with concurrent.futures.ProcessPoolExecutor(max_workers=4) as pool:
        metrics = list(pool.map(get_metrics, [(c['name'], root / c['name'] / (r['instance'] + '.json')) for c, r in valid]))
    keyed = {(r['case'], r['instance']): r for r in metrics}
    controls = {(c['seed'], c['repeat']): c for c in spec['cases'] if c['variant'] == args.control}
    pairs = []
    for c in spec['cases']:
        if c['variant'] == args.control or (c['seed'], c['repeat']) not in controls:
            continue
        control = controls[c['seed'], c['repeat']]
        for name in spec['instances']:
            if (c['name'], name) not in keyed or (control['name'], name) not in keyed:
                continue
            a, b = keyed[control['name'], name], keyed[c['name'], name]
            full = effect(a['tasks'], b['tasks'])
            grid = list(map(int, a['curve']))
            def stable(magnitude):
                flags = []
                for step in grid:
                    av, bv = a['curve'][str(step)], b['curve'][str(step)]
                    delta = effect(av, bv)
                    flags.append(min(av, bv) >= 100 and category(delta) == category(full)
                                 and (not magnitude or abs(delta - full) <= 5))
                bad = max((i for i, good in enumerate(flags) if not good), default=-1)
                return grid[bad + 1] if bad + 1 < len(grid) else None
            pairs.append({'instance': name, 'variant': c['variant'], 'seed': c['seed'], 'repeat': c['repeat'],
                          'control': a['tasks'], 'treatment': b['tasks'], 'effect_percent': full,
                          'identical_trajectories': a['trajectory_sha256'] == b['trajectory_sha256'],
                          'directional_steps': stable(False) if abs(full) >= 1 else None,
                          'within_5pp_steps': stable(True) if abs(full) >= 1 else None,
                          'effect_at_1000': effect(a['curve']['1000'], b['curve']['1000']) if '1000' in a['curve'] else None})
    out.mkdir(parents=True, exist_ok=True)
    for filename, value in [('metrics.json', metrics), ('paired-results.json', pairs), ('run-summaries.json', summaries),
                            ('spec.json', spec), ('allocation.json', json.loads((root / 'allocation.json').read_text())),
                            ('run-metadata.json', {c['name']: json.loads((root / c['name'] / 'metadata.json').read_text()) for c in spec['cases']})]:
        (out / filename).write_text(json.dumps(value, indent=2) + '\n')
    print('ANALYSIS_COMPLETE', len(metrics), 'valid cases; failures', sum(not r['valid'] for rows in summaries.values() for r in rows))


if __name__ == '__main__':
    main()
