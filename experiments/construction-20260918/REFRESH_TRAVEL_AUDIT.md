# Travel and delay accounting for refresh512

GRID jobs **8898658** and **8898659** analyze completed seed0/2 refresh trajectories.
The existing C++ shortest-path decomposition passes its independent assignment/
reassignment fixture. The new cell audit separately hand-checks visits, departures,
waits, turns and nearest-rank latency quantiles, then exactly reconciles every
replayed action with full production diagnostics and all5000 timing samples.

| Measure | Refresh seed0 | Refresh seed2 | Local KittyKnight |
|---|---:|---:|---:|
| Completed tasks | 134,511 | 134,519 | 152,981 |
| Robot-steps per completed task | 371.72 | 371.69 | 326.84 |
| Mean completed-task shortest chain | 242.32 | 242.32 | 233.84 |
| Mean completed-task loaded elapsed steps | 284.62 | 284.60 | 252.52 |
| Loaded elapsed / unit shortest chain | 1.17455 | 1.17448 | 1.07990 |
| All empty robot-steps, including reassignments | 10,298,959 | 10,302,897 | 10,049,537 |

Loaded elapsed includes forward motion, turns and waits; its ratio is not a pure
route-detour ratio. Completed-task cohorts differ, and unfinished tasks are censored.
These comparisons locate remaining differences; they are not a causal allocation
of the throughput gap. KittyKnight also exceeds our32GB memory requirement.
Its final-assignment pickup distances/delays are strongly affected by reassignments;
whole-run empty-step accounting is the safer comparison here.

In the final1000steps, refresh seeds0/2 respectively have9,057,998/9,069,116
forward moves,499,223/486,474 waits and442,779/444,410 turns out of10million
robot-steps. Under the cached unit spatial metric,231,459/224,432 forward moves
increase remaining distance; none are unclassified. Weighted-metric progress is
not measured by this audit.

The top5% of free cells account for26.39%/24.46% of waits, but only22.32%/20.39%
of stationary actions including turns. Counts are affected by traffic volume;
required turns are not queue delays, and cell visits are censored at window ends.
These numbers do not establish a strong, concentrated congestion target or justify
adding per-cell delay tolls. A routing counterfactual has not been evaluated.

The same replay gives mean entry237.3/256.8ms, p95=303.1/322.8ms,
p99=323.8/340.1ms and maximum765.3/762.0ms, using nearest-rank quantiles.

[Travel results and provenance](results/refresh-travel-v1/),
[dwell/latency audit](results/refresh-dwell-v1.json),
[audit provenance](results/refresh-dwell-v1-provenance.json).
