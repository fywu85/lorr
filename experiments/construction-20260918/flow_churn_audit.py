#!/usr/bin/env python3
"""Reconstruct cumulative published traffic costs and measure consecutive-field churn.

Offline observation only; no output is supplied to the planner. Uses input map
adjacency and executed actions, without map-specific layout assumptions.
"""
import argparse
from array import array
import collections
import datetime
import hashlib
import json
from pathlib import Path
from flow_field_audit import read_map, neighbors, fields, counters


def analyze(case):
    metadata = json.loads((case / 'metadata.json').read_text())
    summary = json.loads((case / 'summary.json').read_text())[0]
    assert summary['valid'] and summary['makespan'] == 5000
    instance = summary['instance']
    input_file = Path(metadata['instances'][instance]['input'])
    inp = json.loads(input_file.read_text())
    map_file = input_file.parent / inp['mapFile']
    rows, cols, free = read_map(map_file)
    adj = neighbors(rows, cols, free)
    raw = case / (instance + '.json')
    flow, motion = counters(raw.with_suffix('.log'))
    env = metadata['environment']
    warmup = int(env['CGAR_FLOW_WARMUP'])
    refresh = int(env['CGAR_FLOW_REFRESH_INTERVAL'])
    assert refresh > 0 and not int(env.get('CGAR_FLOW_CACHE_ONLY_REFRESH', '0'))
    steps = [f['step'] for f in flow]
    assert steps == list(range(warmup, 5000, refresh))
    assert all(f['samples'] == step for step, f in zip(steps, flow))
    counts = [array('Q', [0]) * len(adj) for _ in steps]
    with raw.open() as source:
        data = json.load(source)
    assert data['makespan'] == 5000 and len(data['start']) == len(data['actualPaths']) == data['teamSize']
    totals = collections.Counter()
    for start, text in zip(data['start'], data['actualPaths']):
        u = start[0] * cols + start[1]
        direction = {'E': 0, 'S': 1, 'W': 2, 'N': 3}[start[2]]
        assert free[u] and len(text) == 9999
        interval = 0
        for t, action in enumerate(text[::2], 1):
            totals[action] += 1
            if action == 'F':
                if interval < len(steps):
                    counts[interval][4 * u + direction] += 1
                u = adj[4 * u + direction]
                assert u >= 0
            elif action == 'R':
                direction = (direction + 1) % 4
            elif action == 'C':
                direction = (direction + 3) % 4
            else:
                assert action == 'W'
            if interval < len(steps) and t == steps[interval]:
                interval += 1
        assert interval == len(steps)
    assert all(m['steps'] == 5000 for m in motion.values())
    for action, key in [('F', 'fw'), ('R', 'cr'), ('C', 'ccr'), ('W', 'wait')]:
        assert totals[action] == sum(m[key] for m in motion.values())
    minimum = int(env['CGAR_FLOW_MIN_SAMPLES'])
    margin = int(env['CGAR_FLOW_MIN_MARGIN_PERCENT'])
    strength = int(env['CGAR_FLOW_STRENGTH'])
    base = int(env.get('CGAR_FLOW_COST_SCALE', '1'))
    pairs = [(edge, 4 * v + (edge % 4 + 2) % 4)
             for edge, v in enumerate(adj) if v >= 0 and edge // 4 < v]
    published = []
    previous = None
    for i, (step, logged) in enumerate(zip(steps, flow)):
        count = counts[i]
        if i:
            for edge in range(len(adj)):
                count[edge] += counts[i - 1][edge]
        cost = fields(count, adj, minimum, margin, strength, base)
        penalized = sum(c > base for c in cost)
        assert sum(count) == logged['moves'] and penalized == logged['penalized_edges']
        entry = dict(step=step, moves=sum(count), penalized_directed_edges=penalized,
                     field_sha256=hashlib.sha256(cost).hexdigest())
        if previous is not None:
            tally = collections.Counter()
            buckets = {str(bound): collections.Counter() for bound in [25, 50, 75, 100]}
            for a, b in pairs:
                old = int(previous[a] > base) - int(previous[b] > base)
                new = int(cost[a] > base) - int(cost[b] > base)
                if not old and not new:
                    continue
                total = count[a] + count[b]
                dominance = 100 * abs(int(count[a]) - int(count[b])) / total if total else 0
                bucket = buckets[str(next(bound for bound in [25, 50, 75, 100] if dominance <= bound))]
                for out in [tally, bucket]:
                    out['union_penalized_pairs'] += 1
                    out['direction_changed_pairs'] += old != new
                    out['sign_flips'] += old * new < 0
                    out['appeared'] += old == 0 and new != 0
                    out['vanished'] += old != 0 and new == 0
                    out['same_sign_weight_changed'] += old == new and (previous[a] != cost[a] or previous[b] != cost[b])
            directed_union = sum(a > base or b > base for a, b in zip(previous, cost))
            directed_changes = sum((a > base) != (b > base) for a, b in zip(previous, cost))
            entry['churn'] = dict(tally)
            entry['churn']['pair_fraction'] = tally['direction_changed_pairs'] / tally['union_penalized_pairs'] if tally['union_penalized_pairs'] else 0
            entry['churn']['directed_membership_fraction'] = directed_changes / directed_union if directed_union else 0
            entry['churn']['dominance_buckets_upper_inclusive_percent'] = {key: dict(value) for key, value in buckets.items()}
        published.append(entry)
        previous = cost
    return dict(case=case.name, raw_case=str(case), tasks=data['numTaskFinished'],
                seed=int(env['CGAR_SEED']), margin=margin, strength=strength, base=base,
                binary_sha256=metadata['binary_sha256'], map_sha256=hashlib.sha256(map_file.read_bytes()).hexdigest(),
                all_publication_move_and_penalized_counts_match=True,
                full_replayed_actions_match_movement_counters=True, publications=published)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases', type=Path, nargs='+', required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    assert not args.output.exists()
    result = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  script_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                  limitation='Churn is observational and does not establish cause. Reconstructed fields match production move and penalized-edge counts at every publication; production does not log full field hashes. Pair churn counts flips once; directed membership churn counts both toggled directions in a flip. Dominance buckets use current cumulative observed counts.',
                  cases=[])
    for case in args.cases:
        row = analyze(case.resolve())
        result['cases'].append(row)
        print(row['case'], row['tasks'], 'publications', len(row['publications']), flush=True)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n')


if __name__ == '__main__':
    main()
