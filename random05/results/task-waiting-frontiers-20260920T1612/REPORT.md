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
| Four cores / new best seed 2, source `6ce9312` | 3,648 | 1,890 | 2,000 | 135 | 98 |
| 32 workers / new best, source `05559b7` | 3,705 | 1,922 | 2,000 | 135 | 94 |
| Four cores / new best, source `05559b7` | 3,655 | 1,916 | 2,000 | 141 | 98 |
| 32 workers / new best, source `d6a3e0f` | 3,724 | 1,941 | 2,000 | 130 | 96 |
| 32 workers / new best, source `05559b7` | 3,741 | 1,927 | 2,000 | 135 | 94 |
| 32 workers / new best seed0, source `d6a3e0f` | 3,743 | 1,939 | 2,000 | 142 | 95 |

The 3,689-task run finished at 16:15:24 UTC after the initial answer. Its
longest completed order is task 921 (0 to 1,929); 45 initial orders had started
but remained unfinished. Higher throughput did not reduce every tail metric.

The subsequent four-core seed2 record completes3,648 tasks. Its longest
completed order is task1,086 (0 to1,890);135 initial orders remain unfinished,
including98 never opened and37 already started.

The 3,705-task persistent-candidate record has maximum completed latency1,922
(task768, released0),135 initial orders unfinished and94 still unopened.

The latest 3,655-task four-core record has maximum completed latency 1,916
(task 419, released 0). Of the initial orders, 141 remain unfinished: 98 unopened
and 43 already started. Throughput remains the selection criterion.

The3,724-task record has maximum completed latency1,941 (task1020, released0),
130 initial orders unfinished:96 unopened and34 already started.

The3,741-task run has maximum completed latency1,927, with135 initial orders
unfinished (94 unopened). The3,743-task seed0 record has maximum1,939 (task654,
released0), with142 initial orders unfinished:95 unopened and47 started.

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

The faster four-core implementation, source60c5f9b with cached kinematic
eligibility, reproduces the entire3,655 trajectory. Its waiting-time metrics
therefore equal the original3,655 row. The manifest includes this exact new
raw result as `ours-four-cores-3655-optimized`; throughput still selects the run.
