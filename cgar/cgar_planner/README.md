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
Candidates are ranked using whole-chain HRRN estimates. Unmatched robots replenish
candidates that other robots took; an unchanged search that already found nothing
is not repeated. When the fixed local search finds nothing, a deterministic sample
of unused tasks competes with the oldest task under the same cost objective.
Every fleet-width of regular admissions includes an
oldest-task slot evaluated independently of candidate pruning. This is a sparse
approximation to global HRRN, rather than an exhaustive assignment matrix.

`CGAR_MAX_PAIRS` bounds stored candidates, not which task IDs are eligible.
`CGAR_FALLBACK_REPAIR=0` selects the old fallback for a matched control;
`CGAR_FALLBACK_SAMPLES` defaults to 64 (clamped to 0–4096). Sampling uses a separate
deterministic sequence and does not consume PIBT's random stream. Cumulative
scheduler counters are logged every 200 steps; the last log can precede the horizon.
`CGAR_PRIMARY_PATIENCE` and `CGAR_TXN_MAX_AGE` are no longer used. Other existing
switches, including `CGAR_PLANNER=default`, `CGAR_SCHEDULER=default`, `CGAR_CERT`,
`CGAR_TXN`, `CGAR_HRRN`, cache budgets, and seed retain their experimental roles.
Turning off certificates or transactions also turns off the corresponding
progress mechanism.

## Planning deadlines

Scheduling, distance construction, recovery and PIBT share the entry's absolute
wall-clock deadline. If required computation does not finish, CGAR throws `Timeout`;
the competition entry logs `CGAR_TIMEOUT` and exits with code 124. The runner records
a failed timeout with unavailable task/error counts, rather than a successful
partial schedule or a budget-induced waiting plan. The simulator is unchanged.

Distance tables use compact traversable-cell indices and publish only complete
BFS results. The primary receives the first exact lookup. Fixed table-count,
candidate-count and sampling limits remain part of the algorithm: missing secondary
tables use Manhattan estimates after the prescribed count is exhausted. Elapsed
time no longer decides how many candidates or samples to consider. A successful
call completes this fixed policy; this is not an anytime algorithm.

## Build and verification

From the workspace root, using the configured environment:

```sh
env/bin/cmake --build cgar/build --parallel 4
env/bin/ctest --test-dir cgar/build --output-on-failure
```

The regression executable checks certificate reachability, short and long pocket
evacuations, persistent primaries, task-epoch replacement at the same cell,
capacity bootstrap, full 10,000-robot assignment coverage, and fair admission
under a one-pair candidate cap, sparse fallback quality, replenishment after taken
candidates, and explicit scheduler/distance timeouts. Every simulated fixture action is checked for
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

CGAR currently computes on one core. On this GRID cluster, use a reserved allocation
with one physical core per instance to avoid the interactive account's shared
one-CPU quota:

```sh
python3 tools/benchmark_gridengine.py --output runs/cgar-grid-new --jobs 10
```

The launcher requests 8 GiB per slot and uses `debian.q`, the `threaded` parallel environment, physical-core
binding and an exclusive allocation. Each native run is pinned to a different
physical core. Affinity, cgroup quotas, CPU model, CPU time and peak RSS are recorded.
The [GRID resource report](../../benchmarks/gridengine-20260918/README.md) explains
the allocation and its differences from the competition's larger allowance.

Recorded historical full-suite results: [sequential execution](../../benchmarks/mr24-20260917/summary.md) and [ten concurrent instances](../../benchmarks/mr24-parallel-20260918/summary.md). Both use the same production executable. The parallel run shares one CPU across the jobs; all ten horizons completed with zero errors or timeouts, with throughput differences documented in the report.

The [sequential repeat](../../benchmarks/mr24-sequential-repeat-20260918/summary.md) documents remaining account contention. The [strict-deadline fallback study](../../experiments/throughput-20260918-strict/README.md) contains matched policy comparisons and shorter-horizon calibration.
