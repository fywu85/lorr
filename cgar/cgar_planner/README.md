# CGAR migration

This is a synchronous grid adaptation of the KNAPP ground-progress method. The
planner shares its certificate and distance cache with the task scheduler through
`Cgar::instance()`. The executable defaults to both CGAR components.

## Recovery and task epochs

A pending destination retains the oldest primary ticket until it is reached or
its task/stop epoch changes. A repeated goal cell in a new task receives a new
ticket. A stalled primary is never dropped because of its age.

Contested pockets are evacuated into the core before an exclusive service visit.
The planner builds the complete evacuation on a private copy of the placement,
using core vacancies to clear each exit, then independently replays every shift.
The stored witness is executed in waves of distinct robots. Its full cell set
is reserved against other traffic until completion. There is no transaction age
expiry; only an unexpected observed placement invalidates the witness.

The static graph certificate and the runtime placement conditions are distinct.
`valid` in the initialization log describes the whole-fleet graph certificate.
`active_certified` reports whether the current operating mode fits that graph's
capacity. These diagnostics are not a substitute for an end-to-end liveness proof.

## Fleets larger than the core

When the whole fleet does not fit, the CGAR scheduler admits only tasks whose
entire remaining chain lies in the retained core. Robots outside it park and are
excluded from assignment. If the core is full, a checked vacancy shift parks an
additional robot before admitting work. The resulting active fleet must fit the
retained promotion graph's capacity and leave a core vacancy.

This is an explicit throughput tradeoff: outside tasks are excluded, and parked
robots are not promised service. This mode relies on the CGAR scheduler; using
an external/default scheduler can assign work outside its supported task set.

## Scheduling

For every idle robot, a bounded local search supplies nearby task candidates.
Candidates are ranked using whole-chain HRRN estimates, and unmatched robots
search the remaining pool again. A cheap fallback covers robots not reached
before the search deadline. Every fleet-width of regular admissions includes an
oldest-task slot evaluated independently of candidate pruning. This is a sparse
approximation to global HRRN, rather than an exhaustive assignment matrix.

`CGAR_MAX_PAIRS` bounds stored candidates, not which task IDs are eligible.
`CGAR_PRIMARY_PATIENCE` and `CGAR_TXN_MAX_AGE` are no longer used. Other existing
switches, including `CGAR_PLANNER=default`, `CGAR_SCHEDULER=default`, `CGAR_CERT`,
`CGAR_TXN`, `CGAR_HRRN`, cache budgets, and seed retain their experimental roles.
Turning off certificates or transactions also turns off the corresponding
progress mechanism.

## Planning deadlines

Distance tables use compact indices for traversable cells. BFS construction checks
its deadline and caches only complete tables. The primary receives the first exact
distance lookup; other missing tables use Manhattan estimates once the distance
budget expires. Planning reserves time for action validation, and recursive PIBT
also checks the deadline.

## Build and verification

From the workspace root, using the configured environment:

```sh
env/bin/cmake --build cgar/build --parallel 4
env/bin/ctest --test-dir cgar/build --output-on-failure
```

The regression executable checks certificate reachability, short and long pocket
evacuations, persistent primaries, task-epoch replacement at the same cell,
capacity bootstrap, full 10,000-robot assignment coverage, and fair admission
under a one-pair candidate cap, and bounded distance work under a short deadline. Every simulated fixture action is checked for
obstacles, vertex collisions, and edge swaps.

Run the ten main-round instances into a new directory with:

```sh
python3 tools/benchmark_cgar.py --output runs/cgar-new-run --jobs 1
```

The runner copies the executable and records source hashes, time limits,
configuration overrides, output JSON, logs, and CSV/Markdown comparisons with
the original draft's outputs (or their committed summary when raw outputs are absent). Each instance uses the archived simulation length,
1000 ms per decision, and 30000 ms preprocessing. Results are single runs; task
throughput and simulator error counts do not establish starvation freedom.

Recorded full-suite results: [sequential execution](../../benchmarks/mr24-20260917/summary.md) and [ten concurrent instances](../../benchmarks/mr24-parallel-20260918/summary.md). Both use the same production executable. The parallel run shares one CPU across the jobs; all ten horizons completed with zero errors or timeouts, with throughput differences documented in the report.
