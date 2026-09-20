#!/usr/bin/env python3
"""Render the current throughput-selected waiting report from audited records."""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DIRECTORY = ROOT / 'random05/results/task-waiting-frontiers-20260920T1612'


def read(path):
    return json.loads(path.read_text())


def main():
    audit = read(DIRECTORY / 'audit.json')
    assert audit['same_starts_horizon_and_task_stream']
    assert audit['manifest_sha256'] == hashlib.sha256((DIRECTORY / 'manifest.json').read_bytes()).hexdigest()
    rows = []
    for allocation, filename, reference in [
            ('Four cores', 'best-four-cores.json', 'nms-four-cores'),
            ('32 workers', 'best-32-workers.json', 'nms-32-workers')]:
        best = read(ROOT / 'random05' / filename)
        name = best['case']['name']
        evidence = Path(best['evidence'])
        summary = next(r for r in read(ROOT / evidence) if r['name'] == name)
        raw = ROOT / 'runs/random05' / evidence.relative_to('random05/results').parent / name / 'result.json'
        matches = [r for r in audit['runs'].values() if (ROOT / r['result']).resolve() == raw.resolve()]
        assert len(matches) == 1, 'current throughput frontier is missing or duplicated in the waiting audit'
        record = matches[0]
        assert record['tasks_finished'] == summary['result']['numTaskFinished']
        assert record['result_sha256'] == hashlib.sha256(raw.read_bytes()).hexdigest()
        rows.append((allocation + ' / ours, ' + best['source_commit'], record))
        rows.append((allocation + ' / NMS', audit['runs'][reference]))
    lines = [
        '# Order latency at the current RANDOM-05 throughput frontiers', '',
        'Audited ' + audit['created_utc'] + '. Throughput selects these runs. Durations are simulation steps; completion latency includes queueing and service, from release to the final waypoint.', '',
        '| Allocation / solver | Tasks | Completed mean | Completed p95 | Completed max | Oldest unfinished | Initial unfinished / unopened |',
        '|---|---:|---:|---:|---:|---:|---|']
    for label, row in rows:
        completed = row['completed_latency']
        lines.append('| {} | {} | {:.1f} | {} | {} | {} | {} / {} |'.format(
            label, row['tasks_finished'], completed['mean_steps'], completed['p95_steps'],
            row['maximum_release_to_completion']['steps'], row['oldest_unfinished']['steps'],
            row['initial_still_unfinished'], row['initial_still_unopened']))
    lines += ['',
        'Both solvers have step-zero orders unfinished at step 2,000. Their eventual maximum latency is unknown and at least 2,000 steps. Completed-only statistics omit that censored tail and cannot establish a fairness bound. Our records use explicit known-horizon triage.', '',
        'All runs start with the same 1,200 orders. Unopened means no first-waypoint event, even if the task has been assigned. Completing orders reveals replacements, so later release times depend on throughput and every final backlog contains 1,200 orders. Initial-cohort counts avoid that release-time comparison problem.', '']
    for label, row in rows[::2]:
        longest = row['maximum_release_to_completion']
        lines.append('{}: {} initial orders started but unfinished. The longest completed order is {} (release {}, finish {}): {} steps.'.format(
            label, row['initial_still_unfinished'] - row['initial_still_unopened'],
            longest['task_id'], longest['released_at'], longest['observed_at'], longest['steps']))
    lines += ['',
        'Earlier frontiers remain in [the full audit](audit.json) and [per-frontier latency history](../../WAITING_PROGRESS.md). Raw traces stay in ignored `runs/`; the manifest and audit retain exact paths, raw-result hashes and event-accounting checks.', '',
        'Reproduce from the repository root:', '', '```sh',
        'python3 random05/tools/audit_task_waits.py \\',
        '  --manifest random05/results/task-waiting-frontiers-20260920T1612/manifest.json \\',
        '  --output random05/results/task-waiting-frontiers-20260920T1612/audit.json',
        'python3 random05/tools/render_waiting_report.py', '```']
    (DIRECTORY / 'REPORT.md').write_text('\n'.join(lines) + '\n')
    print('Rendered current throughput-selected waiting report: ' + ', '.join(str(row['tasks_finished']) for _, row in rows))


if __name__ == '__main__':
    main()
