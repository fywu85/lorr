#!/usr/bin/env python3
"""Audit observed directional evidence; never export a field for planner input."""
import argparse
from array import array
import hashlib
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--input', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    out = a.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    resources = cpu_resources()
    assert resources['effective_cpu_quota'] is None or resources['effective_cpu_quota'] >= 1
    (out / 'allocation.json').write_text(json.dumps(resources, indent=2) + '\n')
    data = json.loads(a.input.read_text())
    assert data['makespan'] == 5000 and data['numPlannerErrors'] == data['numEntryTimeouts'] == 0
    lines = (ROOT / 'mr24/warehouse.domain/maps/warehouse_large.map').read_text().splitlines()
    rows, cols = int(lines[1].split()[1]), int(lines[2].split()[1])
    free = [c not in '@T' for line in lines[4:] for c in line]
    prefixes = [128, 256, 512, 1024]
    counts = [array('I', [0]) * (rows * cols * 4) for _ in prefixes]
    for robot, (row, col, orientation) in enumerate(data['start']):
        u, direction, period = row * cols + col, 'ESWN'.index(orientation), 0
        for step, action in enumerate(data['actualPaths'][robot][:2 * prefixes[-1]:2]):
            if step == prefixes[period]:
                period += 1
            if action == 'R':
                direction = (direction + 1) % 4
            elif action == 'C':
                direction = (direction + 3) % 4
            elif action == 'F':
                v = u + [1, cols, -1, -cols][direction]
                assert 0 <= v < len(free) and free[v] and abs(u // cols - v // cols) + abs(u % cols - v % cols) == 1
                counts[period][u * 4 + direction] += 1
                u = v
            else:
                assert action == 'W', action
    cumulative = array('I', [0]) * len(counts[0])
    evidence, summaries = {}, {}
    for cutoff, window in zip(prefixes, counts):
        for k, value in enumerate(window):
            cumulative[k] += value
        edges, penalized = {}, {margin: 0 for margin in [0, 10, 25, 50, 75]}
        margins = [0] * 11
        for u in range(len(free)):
            if not free[u]:
                continue
            for direction, v in [(0, u + 1), (1, u + cols)]:
                if v >= len(free) or not free[v] or (direction == 0 and u // cols != v // cols):
                    continue
                forward, reverse = cumulative[4 * u + direction], cumulative[4 * v + direction + 2]
                total = forward + reverse
                if total < 8:
                    continue
                delta = forward - reverse
                edges[4 * u + direction] = (delta, total)
                margins[min(10, abs(delta) * 10 // total)] += 1
                for margin in penalized:
                    penalized[margin] += abs(delta) * 100 > margin * total
        evidence[cutoff] = edges
        summaries[cutoff] = {'executed_moves': sum(cumulative),
            'cardinal_moves_ESWN': [sum(cumulative[d::4]) for d in range(4)],
            'edges_with_at_least_8_samples': len(edges),
            'imbalance_deciles': margins,
            'penalized_directed_edges_above_margin_percent': penalized}
    assert summaries[128]['executed_moves'] == 998691
    assert summaries[128]['penalized_directed_edges_above_margin_percent'][0] == 37474
    stability = {}
    for margin in [0, 10, 25, 50, 75]:
        initial = {k: value for k, value in evidence[128].items() if abs(value[0]) * 100 > margin * value[1]}
        comparable = [(delta, evidence[1024][k][0]) for k, (delta, _) in initial.items() if k in evidence[1024]]
        stability[margin] = {'initially_biased_edges': len(initial), 'comparable_at_1024': len(comparable),
            'opposite_cumulative_direction_at_1024': sum(a * b < 0 for a, b in comparable),
            'balanced_at_1024': sum(b == 0 for _, b in comparable)}
    result = {'purpose': 'Post-hoc audit of control movements, not a planner guidance field or throughput experiment.',
              'input': str(a.input), 'input_sha256': hashlib.sha256(a.input.read_bytes()).hexdigest(),
              'prefixes': summaries, 'early_direction_stability': stability}
    (out / 'audit.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
