#!/usr/bin/env python3
"""Measure order latency from serialized release times and waypoint events."""
import argparse
import datetime
import hashlib
import json
import math
from pathlib import Path
from result_horizon import executed_steps


def audit(path):
    raw = path.read_bytes()
    data = json.loads(raw)
    horizon = executed_steps(data)
    tasks = {task[0]: task for task in data['tasks']}
    assert len(tasks) == len(data['tasks']), 'duplicate task ID'
    for task, release, locations in tasks.values():
        assert 0 <= release <= horizon and locations and len(locations) % 2 == 0
    stages, first, completed = {}, {}, {}
    previous_time = 0
    for time, robot, task, stage in data['events']:
        assert previous_time <= time <= horizon and 0 <= robot < data['teamSize']
        previous_time = time
        assert task in tasks and tasks[task][1] <= time
        assert stage == stages.get(task, 0) + 1
        assert stage <= len(tasks[task][2]) // 2
        stages[task] = stage
        first.setdefault(task, time)
        if stage == len(tasks[task][2]) // 2:
            assert task not in completed
            completed[task] = time
    assert len(completed) == data['numTaskFinished'], 'completion accounting mismatch'
    for key in ('numPlannerErrors', 'numScheduleErrors', 'numEntryTimeouts'):
        assert data[key] == 0, (key, data[key])

    def longest(times):
        if not times:
            return None
        duration, task = max((time - tasks[task][1], task) for task, time in times.items())
        return dict(steps=duration, task_id=task, released_at=tasks[task][1],
                    observed_at=times[task])

    latencies = sorted(time - tasks[task][1] for task, time in completed.items())
    completed_stats = dict(
        count=len(latencies),
        mean_steps=sum(latencies) / len(latencies) if latencies else None,
        p50_steps=latencies[math.ceil(0.50 * len(latencies)) - 1] if latencies else None,
        p95_steps=latencies[math.ceil(0.95 * len(latencies)) - 1] if latencies else None,
        p99_steps=latencies[math.ceil(0.99 * len(latencies)) - 1] if latencies else None)
    unfinished = {task: horizon for task in tasks if task not in completed}
    unopened = {task: horizon for task in unfinished if task not in first}
    initial = [task for task in tasks if tasks[task][1] == 0]
    record = dict(result=str(path), result_sha256=hashlib.sha256(raw).hexdigest(),
                  horizon_steps=horizon, tasks_finished=len(completed), tasks_revealed=len(tasks),
                  completed_latency=completed_stats,
                  maximum_release_to_completion=longest(completed),
                  maximum_release_to_first_waypoint=longest(first),
                  oldest_unfinished=longest(unfinished), oldest_unopened=longest(unopened),
                  unfinished_count=len(unfinished), unopened_count=len(unopened),
                  initial_cohort_count=len(initial),
                  initial_restricted_mean_steps=(sum(completed.get(task, horizon) for task in initial)
                                                 / len(initial) if initial else None),
                  initial_still_unfinished=sum(task in unfinished for task in initial),
                  initial_still_unopened=sum(task in unopened for task in initial),
                  initial_started_unfinished=sum(task in unfinished and task in first for task in initial),
                  event_accounting_valid=True)
    return record, data


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    manifest = json.loads(args.manifest.read_text())
    runs, reference, definitions = {}, None, {}
    for case in manifest:
        record, data = audit(Path(case['result']))
        if reference is None:
            reference = data
        assert data['start'] == reference['start'], 'different starting configurations'
        assert executed_steps(data) == executed_steps(reference), 'different horizons'
        for task, release, locations in data['tasks']:
            assert task not in definitions or definitions[task] == locations, 'different task streams'
            definitions[task] = locations
        runs[case['name']] = record
        print(case['name'], 'finished', record['tasks_finished'], 'max completed',
              record['maximum_release_to_completion']['steps'], 'oldest unfinished',
              record['oldest_unfinished']['steps'], flush=True)
    report = dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  units='simulation steps, not wall-clock seconds',
                  completed_latency='final-waypoint event time minus task release time',
                  unfinished_age='run horizon minus task release time; eventual completion latency is censored',
                  unopened='no first-waypoint event; assignment alone does not open a task',
                  initial_restricted_mean='mean of completion time or horizon for step-zero orders; not uncensored eventual latency',
                  caveat='Completed-only maxima exclude unfinished orders. Task release times after the initial cohort depend on throughput.',
                  source_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                  manifest_sha256=hashlib.sha256(args.manifest.read_bytes()).hexdigest(),
                  same_starts_horizon_and_task_stream=True, runs=runs)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')


if __name__ == '__main__':
    main()
