# Order waiting time at the recorded RANDOM-05 frontiers

Audited 2026-09-20, using the full 2,000-step archived-input runs. Durations are
simulation steps. Completion latency includes both queueing and service:
`final waypoint time - order release time`. These are the throughput-selected
runs, not runs selected for fairness.

| Allocation / solver | Tasks completed | Longest completed latency | Oldest unfinished age | Initial orders unfinished | Initial orders never opened |
|---|---:|---:|---:|---:|---:|
| Four cores / ours, source `6ce9312` | 3,637 | 1,947 | 2,000 | 133 | 101 |
| Four cores / NMS, strongest matched repeat | 2,914 | 1,997 | 2,000 | 219 | 102 |
| 32 workers / ours, source `d933023` | 3,657 | 1,954 | 2,000 | 134 | 96 |
| 32 workers / NMS | 3,172 | 1,976 | 2,000 | 206 | 91 |
| 32 workers / new best, source `8eb59d3` | 3,689 | 1,929 | 2,000 | 141 | 96 |

The 3,689-task run finished at 16:15:24 UTC after the initial answer. Its
longest completed order is task 921 (0 to 1,929); 45 initial orders had started
but remained unfinished. Higher throughput did not reduce every tail metric.

Each run starts with the same 1,200 revealed orders. “Never opened” means no
first-waypoint event; the order may have been assigned or reassigned. The
32-worker allocation uses 16 physical cores and SMT. Both pairs use matching
allocations, starting configurations, task streams and run horizons.

Both solvers leave some step-zero orders unfinished at the horizon. Their
maximum eventual completion latency is therefore unknown and at least 2,000
steps. Reporting only the longest completed order would omit this tail. This
finite experiment does not prove either an eventual fairness guarantee or
infinite starvation. Our recorded runs use the declared known-horizon triage
trick, which also affects late-run service decisions.

The longest completed order is task 401 for our four-core run (released at 0,
completed at 1,947), versus NMS task 1,193 (0 to 1,997). For 32 workers these are
our task 921 (0 to 1,954) and NMS task 439 (0 to 1,976).

The initial cohort has 32 started-but-unfinished orders for our four-core run
versus 117 for NMS; the corresponding 32-worker counts are 38 and 115. The
initial unopened tail is similar. This is descriptive accounting, not an
isolated causal estimate of any planner or scheduler feature.

The simulator keeps 1,200 outstanding orders by revealing replacements after
completion. Thus the total final backlog is 1,200 in all runs, and later
orders have different release times. Initial-cohort counts avoid that particular
comparison problem.

Reproduce from repository root:

```sh
python3 random05/tools/audit_task_waits.py \
  --manifest random05/results/task-waiting-frontiers-20260920T1612/manifest.json \
  --output random05/results/task-waiting-frontiers-20260920T1612/audit.json
```

[Machine-readable audit](audit.json) includes raw-result hashes, exact task IDs,
first-stop delays and event-accounting checks. The audit verifies consecutive
waypoint events, release times, horizon, valid completion counts, identical starts
and consistent task definitions. Raw results remain under the ignored `runs/`
directory; the compact report and manifest are committed.
