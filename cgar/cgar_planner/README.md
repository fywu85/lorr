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

## Experimental assignment changes

`CGAR_PICKUP_WEIGHT` multiplies pickup distance in the scheduler objective before
adding the remaining task-chain cost. The default is 1; accepted values are
clamped to 1–16. With HRRN enabled, waiting age is divided by this weighted cost.
`CGAR_HRRN=0` instead ranks directly by cost. The independent oldest-task admission
rule remains active in either mode. One weight applies to every map.

`CGAR_REASSIGN=1` enables a bounded pass that swaps two already assigned tasks
before their first pickup. It defaults to 0. Every ten steps, the pass considers
at most 256 eligible source robots, with at most 16 nearby partners from a
2,048-node core search and 16 deterministic global samples per source. A swap
must save at least four estimated pickup moves and 10% of the pair's pickup cost.
Both tasks stay assigned; their chain costs cancel in this comparison.

A task may change robot at most once before pickup. Each robot has a 20-step
cooldown after a swap. Started tasks, the current and next pending primary,
recovery participants and cells, pocket service, unfinished movement commitments,
and tasks admitted through the oldest-task rule are protected. Both robots and
pickups must be in the core. Existing capacity restrictions apply to the complete
remaining task chains. The pass never changes simulator-owned task metadata;
normal task-epoch synchronization creates tickets for the accepted new goals.
Finite retargeting and primary protection preserve the existing progress rules;
these checks do not constitute a new end-to-end liveness proof.

Each speculative pair uses cached complete distance tables for both goals if
available, otherwise Manhattan distance for all four comparisons. The pass builds
no tables and does not promote routing-cache entries. Its fixed work must finish
before the shared deadline; otherwise it raises `Timeout`. There is no return of
partly completed reassignment work. `[cgar-reassignment]` logs work, accepted swaps,
estimated savings and protected assignments.

See the [assignment experiment](../../experiments/assignment-20260918/README.md)
for full-horizon comparisons of weights, reassignment and the direct-cost ranking
used by NMS. These switches are experimental until a robust gain is established.

## Experimental distance consistency switches

Three independent switches are available for matched experiments; all default to
`0` until full-horizon results justify promotion:

- `CGAR_REFINE_CHAIN_COSTS=1` replaces permanent approximate task-chain scalars with
  per-leg estimates. A cached complete BFS table can refine a previously Manhattan
  leg; the refined scalar survives table eviction. A changed task stop or route
  invalidates the entry. Revisiting an approximate entry does not build more tables.
- `CGAR_SCHEDULER_CACHE_PEEK=1` reads cached tables without promoting their LRU
  positions. Routing still promotes tables normally, and new table construction
  retains the existing fixed quota.
- `CGAR_STABLE_STALL_BASIS=1` restarts a robot's progress observation window when its
  potential changes between Manhattan, table-derived distance and pocket exit.
  It preserves the robot's ticket. A primary receives its mandatory complete table
  before observation, so secondary-cache eviction does not continually reset its
  recovery timer.

`[cgar-estimates]` logs refinements, invalidations and observation resets. These
changes contain no map-name or map-category policies. See the
[matched experiment](../../experiments/throughput-20260918-next/README.md) for
validation, the promotion decision and NMS transfer priorities.

## Experimental motion guidance and diagnostics

All new motion switches default to `0`. They apply globally, without map-name or
map-category rules:

- `CGAR_TURN_FIRST=1` orders equal-distance candidates by required turns before
  preferring an empty cell. The remaining candidate ordering is unchanged.
- `CGAR_ORIENTATION_GUIDANCE=1` uses exact unit-action distances over
  `(cell, orientation)`, including forward moves and both turn directions.
  Complete turn-and-forward costs compete with a unit wait. Every candidate for
  one robot uses the same cost basis. The cache holds 512 MiB of distance tables
  after trimming and builds at most 32 complete new tables per decision. This
  original LRU experiment can churn; new tables may temporarily exceed the
  retained cache limit until the next decision.
- `CGAR_ORIENTATION_GUIDANCE=2` restricts those builds to goals admitted by a
  demand policy. Every 32 steps it ranks active goals by request count, retains
  resident entries on ties, then breaks ties by goal ID. Only as many goals as
  fit in the same cache are admitted. Goals without a table use the existing
  spatial guidance; prescribed counts determine this fallback, never elapsed time.
- `CGAR_DIAGNOSTICS=1` logs cumulative final actions and disjoint wait causes every
  200 completed decisions. Counts distinguish idle, before-pickup and after-pickup
  robots, identify turn dependencies, and record moves relative to cached spatial
  distance. Recovery, primary, commitment and pocket counts are overlapping
  contexts. Diagnostics read cached distances without building or promoting
  entries or consuming randomness.

Orientation costs guide ordinary PIBT choices. They do not replace the certified
spatial progress potential, primary tickets, recovery witnesses, pocket locks or
existing commitments. Robots evacuating a draining pocket retain exit guidance.
All prescribed computation must complete before the shared deadline, including
complete table construction; otherwise the entry raises `Timeout`.

The optional combination `CGAR_ORIENTATION_GUIDANCE=2 CGAR_TURN_FIRST=1` improves
several instances over six full-horizon seeds, but SORTATION remains mixed and
has a negative mean. It is not the default. The [movement study](../../experiments/motion-20260918/README.md)
contains per-seed throughput, runtime and memory, identical-goal NMS comparisons,
and unsuccessful four-proposal and rotation-reservation experiments. Those two
rejected mechanisms are archived as patches and absent from the active planner.

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
candidates, explicit scheduler/distance timeouts, weighted pickup choices, bounded
unopened-task swaps, and protection of started tasks, primary tickets, recovery
witnesses, ongoing turns and fair admissions. The turn-distance oracle is compared with independent forward action searches;
cache eviction/retention, explicit timeout failure, and turn-dependency attribution
are also checked. Every simulated fixture action is checked for obstacles, vertex
collisions, and edge swaps.

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
