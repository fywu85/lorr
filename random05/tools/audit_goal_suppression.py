#!/usr/bin/env python3
"""Separate traffic actions from horizon-suppressed goals in validated traces."""
import argparse
from collections import Counter, defaultdict, deque
import datetime
import hashlib
import json
from pathlib import Path
from result_horizon import executed_steps
import re


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def audit(case, certified):
    path = Path(case['result'])
    assert sha(path) == certified['result_sha256'] and certified['replay_valid']
    data = json.loads(path.read_text())
    map_path = Path(case['map'])
    assert sha(map_path) == certified['map_sha256']
    lines = map_path.read_text().splitlines()
    rows, cols = int(lines[1].split()[-1]), int(lines[2].split()[-1])
    free = {y * cols + x for y, row in enumerate(lines[4:])
            for x, cell in enumerate(row) if cell not in '@T'}
    adjacent = {}
    for cell in free:
        y, x = divmod(cell, cols)
        adjacent[cell] = [(y + dy) * cols + x + dx
                          if 0 <= x + dx < cols and 0 <= y + dy < rows
                          and (y + dy) * cols + x + dx in free else -1
                          for dx, dy in ((1, 0), (0, 1), (-1, 0), (0, -1))]
    distances = {}
    def distance(a, b):
        if a not in distances:
            found, todo = {a: 0}, deque([a])
            while todo:
                cell = todo.popleft()
                for nxt in adjacent[cell]:
                    if nxt >= 0 and nxt not in found:
                        found[nxt] = found[cell] + 1
                        todo.append(nxt)
            distances[a] = found
        return distances[a][b]

    goals = {t[0]: [t[2][k] * cols + t[2][k + 1]
                    for k in range(0, len(t[2]), 2)] for t in data['tasks']}
    changes, events = defaultdict(list), defaultdict(list)
    for agent, row in enumerate(data['actualSchedule']):
        for entry in row.split(',') if row else []:
            time, task = map(int, entry.split(':'))
            changes[time].append((agent, task))
    for time, agent, task, stage in data['events']:
        events[time].append((agent, task, stage))
    paths = [row.split(',') for row in data['actualPaths']]
    loc = [row[0] * cols + row[1] for row in data['start']]
    direction = ['ESWN'.index(row[2]) for row in data['start']]
    n, horizon = data['teamSize'], executed_steps(data)
    schedule, stages = [-1] * n, defaultdict(int)
    total_forward = 0
    bins, phases, sampled = {}, defaultdict(Counter), {}
    scale = case.get('triage_scale')
    logged = {}
    if scale is not None:
        for line in Path(case['native_log']).read_text().splitlines():
            match = re.match(r'R05_STEP t=(\d+) moves=(\d+) .* triaged=(\d+)$', line)
            if match:
                time, moves, dropped = map(int, match.groups())
                logged[time] = (moves, dropped)
        assert logged, 'missing native suppression counts'
    for step in range(horizon):
        time = step + 1
        for agent, task in changes[time]:
            schedule[agent] = task
        bucket = bins.setdefault(str(step // 200 * 200),
                                 dict(first_step=step // 200 * 200,
                                      actions=defaultdict(Counter), finished=0,
                                      suppressed_agent_steps=0))
        per_cell = n * step / total_forward if total_forward else 2
        suppressed = moves = 0
        for agent in range(n):
            task = schedule[agent]
            phase = 'idle' if task < 0 else 'loaded' if stages[task] else 'unopened'
            dropped = False
            if task >= 0 and scale is not None:
                remaining, previous = 0, loc[agent]
                for goal in goals[task][stages[task]:]:
                    remaining += distance(previous, goal)
                    previous = goal
                dropped = remaining * per_cell * scale > horizon - step
            suppressed += dropped
            state = phase + ('_suppressed' if dropped else '_active')
            action = paths[agent][step]
            bucket['actions'][state][action] += 1
            phases[state][action] += 1
            if action == 'F':
                loc[agent] = adjacent[loc[agent]][direction[agent]]
                assert loc[agent] >= 0
                moves += 1
            elif action == 'R':
                direction[agent] = (direction[agent] + 1) % 4
            elif action == 'C':
                direction[agent] = (direction[agent] + 3) % 4
        total_forward += moves
        bucket['suppressed_agent_steps'] += suppressed
        if step in logged:
            assert logged[step] == (moves, suppressed), (step, logged[step], moves, suppressed)
            sampled[str(step)] = dict(moves=moves, suppressed=suppressed)
        for agent, task, stage in events[time]:
            assert schedule[agent] == task and stage == stages[task] + 1
            assert loc[agent] == goals[task][stage - 1]
            stages[task] = stage
            bucket['finished'] += stage == len(goals[task])
    assert sum(b['finished'] for b in bins.values()) == data['numTaskFinished']
    for phase in ('loaded', 'unopened', 'idle'):
        reconstructed = Counter()
        reconstructed.update(phases[phase + '_active'])
        reconstructed.update(phases[phase + '_suppressed'])
        assert reconstructed == Counter(certified['phases'][phase]), phase
    assert sum(sum(row.values()) for row in phases.values()) == n * horizon
    return dict(result=str(path), result_sha256=sha(path), map_sha256=sha(map_path),
                triage_scale=scale, tasks=data['numTaskFinished'], bins=bins,
                phases=phases, native_samples_checked=sampled,
                native_log_sha256=sha(Path(case['native_log'])) if scale is not None else None,
                reconstructed_accounting_valid=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, required=True)
    parser.add_argument('--certified-audit', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    certified = json.loads(args.certified_audit.read_text())
    results = {}
    for case in json.loads(args.manifest.read_text()):
        record = audit(case, certified['runs'][case['name']])
        results[case['name']] = record
        active = record['phases']['loaded_active'].get('W', 0)
        suppressed = record['phases']['loaded_suppressed'].get('W', 0)
        print(case['name'], 'loaded waits active/suppressed', active, suppressed, flush=True)
    args.output.write_text(json.dumps(dict(
        created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        source_sha256=sha(Path(__file__)), manifest_sha256=sha(args.manifest),
        certified_audit_sha256=sha(args.certified_audit), runs=results,
        caveat='Reconstructed current-step goal suppression, not a causal estimate of throughput lost to waits.'), indent=2) + '\n')


if __name__ == '__main__':
    main()
