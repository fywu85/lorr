#!/usr/bin/env python3
"""Read-only lower-bound feasibility of assigned visible chains near the end."""
import argparse
from collections import Counter, defaultdict
import datetime
import hashlib
import json
from pathlib import Path
import numpy as np
from result_horizon import executed_steps


def analyze(case, distances, index, cols, span):
    path = Path(case['result'])
    raw = path.read_bytes()
    data = json.loads(raw)
    horizon = executed_steps(data)
    assert all(data[k] == 0 for k in ('numPlannerErrors', 'numScheduleErrors', 'numEntryTimeouts'))
    goals = {t[0]: [index[t[2][k]*cols+t[2][k+1]] for k in range(0, len(t[2]), 2)] for t in data['tasks']}
    tails = {}
    for task, chain in goals.items():
        tail = np.zeros((len(chain), 4), dtype=np.int32)
        for k in range(len(chain)-2, -1, -1):
            cost = distances[chain[k]*4:chain[k]*4+4, chain[k+1]*4:chain[k+1]*4+4].astype(np.int32)
            assert np.all(cost < 65535)
            tail[k] = np.min(cost + tail[k+1][None, :], axis=1)
        tails[task] = tail
    changes, events = defaultdict(list), defaultdict(list)
    for agent, row in enumerate(data['actualSchedule']):
        for item in row.split(',') if row else []:
            time, task = map(int, item.split(':'))
            changes[time].append((agent, task))
    for time, agent, task, stage in data['events']:
        events[time].append((agent, task, stage))
    actions = [row.split(',') for row in data['actualPaths']]
    assert len(actions) == data['teamSize'] and all(len(row) == horizon for row in actions)
    loc = [row*cols+col for row, col, _ in data['start']]
    direction = ['ESWN'.index(heading) for _, _, heading in data['start']]
    assignment = [-1]*data['teamSize']
    stage = defaultdict(int)
    counts = defaultdict(Counter)
    completed = 0
    start = max(1, horizon-span+1)
    for time in range(1, horizon+1):
        changed = dict(changes[time])
        for agent, task in changed.items():
            assignment[agent] = task
        for agent, path_actions in enumerate(actions):
            task = assignment[agent]
            action = path_actions[time-1]
            if time >= start:
                counts['all'][action] += 1
                if task >= 0 and stage[task] < len(goals[task]):
                    k = stage[task]
                    goal = goals[task][k]
                    pose = index[loc[agent]]*4+direction[agent]
                    bound = int(np.min(distances[pose, goal*4:goal*4+4].astype(np.int32)+tails[task][k]))
                    # Arriving at a currently occupied goal still consumes an
                    # action. Later repeated waypoints remain a relaxation.
                    bound = max(1, bound)
                    category = 'opened' if k else 'unopened'
                    counts[category][action] += 1
                    if bound > horizon-time+1:
                        counts['provably_late_'+category][action] += 1
                        if agent in changed:
                            counts['assignment_changes']['provably_late_'+category] += 1
                    if agent in changed:
                        counts['assignment_changes'][category] += 1
                else:
                    counts['idle'][action] += 1
            if action == 'F':
                previous = loc[agent]
                loc[agent] += (1, cols, -1, -cols)[direction[agent]]
                assert loc[agent] in index and abs(previous//cols-loc[agent]//cols)+abs(previous%cols-loc[agent]%cols) == 1
            elif action == 'R':
                direction[agent] = (direction[agent]+1)%4
            elif action == 'C':
                direction[agent] = (direction[agent]+3)%4
            else:
                assert action == 'W'
        for agent, task, next_stage in events[time]:
            assert assignment[agent] == task and next_stage == stage[task]+1
            assert index[loc[agent]] == goals[task][stage[task]]
            stage[task] = next_stage
            completed += next_stage == len(goals[task])
    assert completed == data['numTaskFinished']
    total = (horizon-start+1)*data['teamSize']
    assert sum(counts['all'].values()) == total
    assert sum(sum(counts[k].values()) for k in ('opened', 'unopened', 'idle')) == total
    return dict(case, tasks=completed, result_sha256=hashlib.sha256(raw).hexdigest(),
                first_step=start, last_step=horizon, robot_steps=total, counts=dict(counts))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--distances', required=True, type=Path)
    parser.add_argument('--manifest', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--span', type=int, default=200)
    args = parser.parse_args()
    assert args.span > 0
    with args.distances.open('rb') as stream:
        magic, rows, cols, count, states = map(int, np.fromfile(stream, dtype='<u4', count=5))
        assert magic == 0x4f524431 and states == count*4
        cells = np.fromfile(stream, dtype='<u4', count=count)
        distances = np.fromfile(stream, dtype='<u2').reshape(states, states)
    index = {int(cell): i for i, cell in enumerate(cells)}
    records = []
    for case in json.loads(args.manifest.read_text()):
        row = analyze(case, distances, index, cols, args.span)
        records.append(row)
        late = sum(sum(row['counts'].get(k, {}).values()) for k in ('provably_late_opened', 'provably_late_unopened'))
        print(case['name'], 'provably-late assigned robot steps', late, '/', row['robot_steps'], flush=True)
    output = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  source_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                  distance_sha256=hashlib.sha256(args.distances.read_bytes()).hexdigest(),
                  manifest_sha256=hashlib.sha256(args.manifest.read_bytes()).hexdigest(), rows=records,
                  caveat='Uses only each currently assigned visible chain, current pose and declared end. Directed unit-action distances ignore collisions and later repeated-waypoint service ticks, so late classifications are conservative impossibility certificates. Robot motion can help others even when its own task cannot finish; these are not wasted-work estimates or recoverable throughput. Planner suppression state is unavailable from traces. All traces were previously independently replayed; no solver or benchmark is modified.')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2)+'\n')


if __name__ == '__main__':
    main()
