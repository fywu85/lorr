# Order latency at the current RANDOM-05 throughput frontiers

Audited 2026-09-22T08:18:22.650468+00:00. Throughput selects these runs. Durations are simulation steps; completion latency includes queueing and service, from release to the final waypoint.

| Allocation / solver | Tasks | Completed mean | Completed p95 | Completed max | Oldest unfinished | Initial unfinished / unopened |
|---|---:|---:|---:|---:|---:|---|
| Four cores / ours, 5f81613 | 3770 | 421.8 | 1065 | 1941 | 2000 | 137 / 96 |
| Four cores / NMS | 2914 | 418.0 | 1296 | 1997 | 2000 | 219 / 102 |
| 32 workers / ours, 2d893f96e78d3fcc092c37a03a7b1888880b306e | 4302 | 352.3 | 957 | 1986 | 2000 | 146 / 110 |
| 32 workers / NMS | 3172 | 389.6 | 1273 | 1976 | 2000 | 206 / 91 |

Both solvers have step-zero orders unfinished at step 2,000. Their eventual maximum latency is unknown and at least 2,000 steps. Completed-only statistics omit that censored tail and cannot establish a fairness bound. Our records use explicit known-horizon triage.

All runs start with the same 1,200 orders. Unopened means no first-waypoint event, even if the task has been assigned. Completing orders reveals replacements, so later release times depend on throughput and every final backlog contains 1,200 orders. Initial-cohort counts avoid that release-time comparison problem.

Four cores / ours, 5f81613: 41 initial orders started but unfinished. The longest completed order is 279 (release 0, finish 1941): 1941 steps.
32 workers / ours, 2d893f96e78d3fcc092c37a03a7b1888880b306e: 36 initial orders started but unfinished. The longest completed order is 1016 (release 0, finish 1986): 1986 steps.

Earlier frontiers remain in [the full audit](audit.json) and [per-frontier latency history](../../WAITING_PROGRESS.md). Raw traces stay in ignored `runs/`; the manifest and audit retain exact paths, raw-result hashes and event-accounting checks.

Reproduce from the repository root:

```sh
python3 random05/tools/audit_task_waits.py \
  --manifest random05/results/task-waiting-frontiers-20260920T1612/manifest.json \
  --output random05/results/task-waiting-frontiers-20260920T1612/audit.json
python3 random05/tools/render_waiting_report.py
```
