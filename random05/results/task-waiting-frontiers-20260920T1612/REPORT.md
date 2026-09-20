# Order latency at the current RANDOM-05 throughput frontiers

Audited 2026-09-20T19:26:45.443966+00:00. Throughput selects these runs. Durations are simulation steps; completion latency is final-waypoint time minus release time, including queueing and service.

| Allocation / solver | Tasks | Completed mean | Completed p95 | Completed max | Oldest unfinished | Initial unfinished / unopened |
|---|---:|---:|---:|---:|---:|---|
| Four cores / ours, f81b760 | 3709 | 424.2 | 1032 | 1947 | 2000 | 135 / 98 |
| Four cores / NMS, strongest matched repeat | 2914 | 418.0 | 1296 | 1997 | 2000 | 219 / 102 |
| 32 workers / ours, 5f81613 | 3794 | 416.0 | 1057 | 1950 | 2000 | 133 / 98 |
| 32 workers / NMS | 3172 | 389.6 | 1273 | 1976 | 2000 | 206 / 91 |

Both solvers have step-zero orders unfinished at step2,000. Their eventual maximum latency is unknown and at least2,000 steps. Completed-only statistics omit that censored tail and cannot establish a fairness bound. Our records use explicit known-horizon triage.

All runs start with the same1,200 orders. Unopened means no first-waypoint event, even if the task has been assigned. Completing orders reveals replacements, so later release times depend on throughput and every final backlog contains1,200 orders. Initial-cohort counts avoid that release-time comparison problem.

The four-core3,709 record has37 initial orders started but unfinished, versus117 forNMS. The32-worker3,794 record has35, versus115 forNMS. These are descriptive counts, not isolated causal effects.

The four-core record reproduces the entire earlier relaxed3,709 trajectory, now under strict1s. Its longest completed order is674 (release0, finish1,947). The32-worker3,794 record's longest is1223 (release34, finish1,984):1,950 steps. Throughput remains primary; the completed maximum increased from1,919 in the earlier3,755 run.

Earlier frontiers remain in [the full audit](audit.json) and [per-frontier latency history](../../WAITING_PROGRESS.md). Raw traces stay in ignored `runs/`; the manifest and audit include exact paths, raw-result hashes and event-accounting checks.

Reproduce from repository root:

```sh
python3 random05/tools/audit_task_waits.py \
  --manifest random05/results/task-waiting-frontiers-20260920T1612/manifest.json \
  --output random05/results/task-waiting-frontiers-20260920T1612/audit.json
```
