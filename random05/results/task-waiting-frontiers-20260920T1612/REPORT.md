# Order latency at the current RANDOM-05 throughput frontiers

Audited 2026-09-20T19:05:16.590571+00:00. Throughput selects these runs. Durations are simulation steps; completion latency is final-waypoint time minus release time, including queueing and service.

| Allocation / solver | Tasks | Completed mean | Completed p95 | Completed max | Oldest unfinished | Initial unfinished / unopened |
|---|---:|---:|---:|---:|---:|---|
| Four cores / ours, f81b760 | 3709 | 424.2 | 1032 | 1947 | 2000 | 135 / 98 |
| Four cores / NMS, strongest matched repeat | 2914 | 418.0 | 1296 | 1997 | 2000 | 219 / 102 |
| 32 workers / ours, 5f81613 | 3755 | 424.3 | 1076 | 1919 | 2000 | 130 / 93 |
| 32 workers / NMS | 3172 | 389.6 | 1273 | 1976 | 2000 | 206 / 91 |

Both solvers have step-zero orders unfinished at step2,000. Their eventual maximum latency is therefore unknown and at least2,000 steps. Completed-only statistics omit that censored tail and cannot establish a fairness bound. Our records use the explicit known-horizon triage trick.

All runs start with the same1,200 orders. “Unopened” means no first-waypoint event, even if the task has been assigned. Completing orders reveals replacements, so later release times depend on throughput and every final backlog contains1,200 orders. Initial-cohort counts avoid that release-time comparison problem.

The four-core3,709 record has37 initial orders started but unfinished, versus117 forNMS. The32-worker3,755 record has37, versus115 forNMS. These are descriptive counts, not isolated causal effects of any feature.

The four-core implementation reproduces the entire earlier relaxed-deadline3,709 trajectory, now under a strict1s deadline. Its longest completed order is674 (release0, finish1,947). The32-worker3,755 run's longest is995 (release0, finish1,919).

Earlier frontiers remain in [the full audit](audit.json) and [per-frontier latency history](../../WAITING_PROGRESS.md). Raw traces stay in ignored `runs/`; the manifest includes exact paths and the audit includes raw-result hashes and event-accounting checks.

Reproduce from repository root:

```sh
python3 random05/tools/audit_task_waits.py \
  --manifest random05/results/task-waiting-frontiers-20260920T1612/manifest.json \
  --output random05/results/task-waiting-frontiers-20260920T1612/audit.json
```
