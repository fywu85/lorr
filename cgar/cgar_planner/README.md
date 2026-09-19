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

`CGAR_GLOBAL_SAMPLES` (default 0, clamped to 0–512) adds a deterministic
sample of free tasks to each idle robot's initial shortlist, even when its local
search found candidates. Samples use a coprime stride through the complete
eligible task list, exclude local duplicates, and fit within `CGAR_MAX_PAIRS`.
The oldest-task admission is still independent. This is an experimental option;
its full warehouse comparisons are in the [construction study](../../experiments/construction-20260918/RESULTS.md).

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
  one robot uses the same cost basis. By default the cache holds 512 MiB of distance tables
  after trimming and builds at most 32 complete new tables per decision. This
  original LRU experiment can churn; new tables may temporarily exceed the
  retained cache limit until the next decision.
- `CGAR_TURN_TABLE_MB` sets the orientation-table cache capacity in MiB
  (default 512, clamped to 16–32768). It changes storage capacity, not the
  fixed limit of 32 complete table builds per decision. On full WAREHOUSE,
  `CGAR_ORIENTATION_GUIDANCE=1 CGAR_TURN_FIRST=1 CGAR_TURN_TABLE_MB=8192`
  outperformed the 512 MiB demand-cache profile on six planner seeds, with
  total measured peak memory around 14.3 GiB. Larger capacities preserved
  the same trajectories on seeds 0–2. See the [warehouse study](../../experiments/sequences-20260918/README.md).
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

## Experimental temporal construction and repair

`CGAR_TEMPORAL=1` uses a generic five-step operation pool, displacement constructor,
and fixed-work repair adapted from NMS; see `LICENSE_NMS`. It requires orientation
guidance and keeps CGAR's primary, recovery witnesses, pocket locks, parking and
capacity rules. Ordinary spatial commitments are replaced by a temporal joint
plan. Supporting forward moves required by a protected first action are pinned,
and unfinished protected destination cells cannot be newly occupied. All five
reservation layers are independently checked before publishing the first action.

The tested warehouse research profile is:

```sh
CGAR_ORIENTATION_GUIDANCE=1 CGAR_TURN_FIRST=1 CGAR_TURN_TABLE_MB=8192 \
CGAR_TEMPORAL=1 CGAR_TEMPORAL_STEPS=50000 CGAR_TEMPORAL_EQUAL_WEIGHT=1 \
./cgar/build/lifelong -i mr24/warehouse.domain/WAREHOUSE.json -o result.json -s 5000 -t 1000 -d 2
```

Use a reserved core rather than the shared interactive CPU quota. This profile
averages 107,413 tasks across three full seeds, versus 43,407 for the paired
previous cache profile. Measured total peak RSS is about 15.2 GiB. The precise
seed-0 repeat has maximum complete entry time 0.5051 seconds. It has not yet
matched the local leader or earned six-seed confirmation. The switch defaults
off, and these results do not imply performance on other maps.

- `CGAR_TEMPORAL_STEPS` prescribes repair attempts per worker (0–1,000,000).
  Construction always processes the complete order. Zero means construction only.
- `CGAR_TEMPORAL_WARM_START=1` reuses the unexecuted suffix of the previous
  complete plan (default off). Every suffix already exists in the generic action
  pool. Changed goals, protected actions and unavailable candidates return to
  their normal seeds; a deterministic closure resets every conflicting suffix.
  State/time mismatches discard history. Construction and fixed repair still
  finish completely, and the final plan still passes all collision checks.
  This is experimental; full throughput validation is pending.
- `CGAR_TEMPORAL_BUDGET` bounds displacements per construction root (default 8192);
  repair roots also have the native cap of 1000. A failed bounded attempt rolls
  back according to the algorithm, independently of elapsed time.
- `CGAR_TEMPORAL_ORDER` selects ticket (0), current-goal distance (1, default),
  or estimated remaining-task-chain distance (2) construction order.
- `CGAR_TEMPORAL_EQUAL_WEIGHT=1` gives every active ordinary robot equal score
  weight; the default retains rank weights. Protected actions remain fixed.
- `CGAR_TEMPORAL_WORKERS` (1–32) runs a deterministic portfolio.
  `CGAR_TEMPORAL_THREADS` controls concurrent execution, defaulting to the worker
  count. All prescribed workers finish or the entire decision fails. Seeds and
  tie-breaking do not depend on thread completion order. The GRID matrix harness
  accepts `--cpus-per-instance`; affinity size alone is not proof of an allocation.
- `CGAR_TEMPORAL_CANDIDATE_LIMIT` (default 0) experiments with a deterministic
  total candidate threshold per worker, including construction. It is checked
  between complete repair attempts; the final attempt may overshoot. The attempt
  cap also applies, defaulting to 1,000,000 when this option is positive. It has
  only deadline-screening evidence so far, not a full-run performance claim.

Additional optional experiments are undergoing full-run evaluation:

- `CGAR_TURN_COST` sets an integer rotation guidance cost (1–16, default 1).
  Forward cost stays one. Complete reverse Dijkstra tables and temporal path
  scores charge the same extra turn cost, including rotations represented by
  terminal waits. The certificate's spatial potential is unchanged.
- `CGAR_TEMPORAL_REGIONS` enables repair of disjoint portions of one complete
  plan (1–32 regions; unset or 0 disables it). Partitions use only dimensions;
  paths crossing a boundary and all protected paths stay fixed for that round.
  Other candidates remain wholly inside their assigned region.
  `CGAR_TEMPORAL_REGION_STEPS` sets attempts per region (default 25,000),
  `CGAR_TEMPORAL_REGION_ROUNDS` sets complete rounds with shifted boundaries
  (default 2, range 1–16), and `CGAR_TEMPORAL_REGION_THREADS` sets concurrent
  execution (default the region count). `CGAR_TEMPORAL_REGION_TEMPERATURE_PPM`
  sets the initial annealing temperature in millionths (default 1000; range
  0–10,000); zero permits only nondecreasing-score repair attempts. This setting
  changes only regional repair, and is an unvalidated tuning experiment.
  Every worker finishes before merging; the complete merged reservations are
  checked for collisions. Thread scheduling does not change seeds, prescribed
  work, or the selected result.

Full seed-0 runs reach 111,411 tasks with one regional round and 112,164 with two,
against a 107,457 control. Both fit the deadline and memory limits; multiple-seed
confirmation remains outstanding. Corrected turn cost 2 reduces throughput and
cost 4 fails explicitly at step 902 despite passing its short screen. See the
[full experiment record](../../experiments/construction-20260918/RESULTS.md).

A deadline overrun propagates as `Timeout`; no partial worker portfolio, regional
merge, or clock-truncated search is returned successfully. The [study](../../experiments/construction-20260918/README.md)
retains native conformance, full trajectories' fingerprints, failed settings,
resource allocations and the independent Fable review. These finite checks do
not constitute an end-to-end liveness proof.

## Experimental PIBT reference policy

`CGAR_PIBT_REFERENCE=1` selects native `Kei18/pibt2` spatial candidate ordering and
priorities inside the shared destination-assignment recursion. It uses exact
spatial distance tables, initial-distance and elapsed-step priorities, and the
upstream random tie-breaking sequence. It defaults `CGAR_CERT` and `CGAR_TXN` to
`0`, disables ordinary movement commitments, and includes every traversable
component without capacity pruning. These settings are experimental; the regular
CGAR policy and its defaults remain unchanged.

For layer comparisons, `CGAR_PIBT_TICKETS=1` restores CGAR's priority ordering and
`CGAR_PIBT_COMMITMENTS=1` restores ordinary destination persistence. Explicit
`CGAR_CERT=1` restores certificate routing, pocket locks and capacity handling;
`CGAR_TXN=1` restores optional recovery when its certificate conditions hold.
Transaction commitments remain protected even when ordinary commitments are off.
The PIBT-specific ticket/commitment switches have no effect unless reference mode
is enabled. `CGAR_TURN_FIRST` and `CGAR_ORIENTATION_GUIDANCE` must both be zero in
reference mode; incompatible combinations are rejected at initialization.

Native PIBT has no rotations. The competition entry still applies LoRR turns,
forward-dependency cancellation and final collision checks. The conformance
harness separately tests native spatial moves and an independent rotation
adapter. Lifelong scheduling stays with CGAR; changing or retiring a target cell
restarts its native priority episode. This extension is not upstream's MAPD
scheduler. The compatibility profile does not inherit the disabled certificate
and recovery progress guarantees.

Exact distances in this profile do not fall back to Manhattan after a table-count
limit. Required tables complete or raise `Timeout`. Large cold workloads can
therefore need more than the usual one-second decision budget. The diagnostic
study uses an explicit ten-second cap, with all one-second failures retained.
See the [PIBT study](../../experiments/pibt-equivalence-20260918/README.md) for the
pinned source, 308 equivalence cases, 300 full runs, runtime and per-layer effects.

## Planning deadlines

Scheduling, distance construction, recovery and PIBT share the entry's absolute
wall-clock deadline. If required computation does not finish, CGAR throws `Timeout`;
the competition entry logs `CGAR_TIMEOUT` and exits with code 124. The runner records
a failed timeout with unavailable task/error counts, rather than a successful
partial schedule or a budget-induced waiting plan. Initialization failures exit
125 (invalid configuration or exception) or 124 (timeout), and readiness is set
only after complete initialization. Local simulator instrumentation propagates
initialization exceptions and adds `entryComputeTimes` at the final entry deadline
check. The original `plannerTimes` also includes subsequent simulator logging;
both series are retained. Movement rules and task accounting are unchanged.

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

CGAR uses one core by default; optional temporal portfolio and regional repair
can use explicitly reserved additional cores. On this GRID cluster, use a reserved
allocation to avoid the interactive account's shared one-CPU quota:

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

## Experimental orientation storage

`CGAR_TURN_COMPACT=1` optionally packs complete orientation tables losslessly into
16 bits, with a 32-bit fallback whenever a finite distance reaches 65,535. Logical
cache capacity stays unchanged. Regression and complete 5,000-step warehouse
trajectory checks pass. The seed-0 control uses 11.883 GB RSS versus 16.208 GB
with wide tables and preserves every path, schedule, event and task. It also
passes the one-second complete-entry limit during a concurrent two-instance run.

`CGAR_TURN_PREFETCH_THREADS=1..32` speculatively builds complete tables using
reserved threads before demand admission. It defaults to zero. Repeated cold
checks found no timing benefit, so leave it disabled. See the
[routing study](../../experiments/construction-20260918/ROUTING.md) for all outcomes.

## Experimental learned traffic and objective scaling

`CGAR_FLOW_STRENGTH=1..8` enables a one-time frozen guidance field learned only
from executed movement. `CGAR_FLOW_WARMUP` defaults to 128 observed steps and
`CGAR_FLOW_MIN_SAMPLES` to 8 observations per edge. It defaults off. Exact-table
path costs include the same forward penalties, while the certified spatial
potential remains unchanged. Strengths 1/2/4 pass regression and deadline screens;
full performance is pending. [Traffic study](../../experiments/construction-20260918/FLOW.md).

`CGAR_FLOW_MIN_MARGIN_PERCENT` optionally requires stronger directional evidence
before assigning a learned penalty (0–100, default 0). The opposite-minus-forward
count must exceed that percentage of their total. This is a count margin, not a
confidence interval. Higher-margin and later-freeze profiles are experimental;
see [the controlled follow-up](../../experiments/construction-20260918/FLOW_MARGIN.md).

`CGAR_TEMPORAL_DISTANCE_SCALE` changes goal-distance emphasis relative to the
native operation-index bonus (default 50, allowed 1–4096). Scales 256/1024 pass
regression/deadline screens; full performance is pending. This is tested
separately from learned traffic. [Scoring study](../../experiments/construction-20260918/SCALE.md).

Optional intended-route experiment: `CGAR_GUIDE_ROUTES=1` uses fixed-work,
current-goal congestion-aware routes to guide ordinary temporal candidates.
It requires temporal planning, unit turns and `CGAR_FLOW_STRENGTH=0`.
`CGAR_GUIDE_BATCH=128`, `CGAR_GUIDE_EXPANSIONS=4096`,
`CGAR_GUIDE_LOOKAHEAD=8`, `CGAR_GUIDE_BASE_COST=16`,
`CGAR_GUIDE_OPPOSITE_COST=1`, and `CGAR_GUIDE_LOAD_COST=0` are the defaults.
Limited route searches publish no guide; the complete temporal solve and
one-second failure contract still apply. This is unpromoted experimental code;
see [the design and evidence](../../experiments/construction-20260918/GUIDE_PATHS.md).

`CGAR_GUIDE_HEURISTIC_WEIGHT=1` controls the fixed guide-search heuristic weight
(range 1–8). Larger values can select different, nonoptimal routing-cost paths;
only complete routes are admitted. Cached unit-orientation distances provide
lower bounds when available. This changes guide search ordering and requires
separate full-horizon evaluation even at weight 1.

`CGAR_GUIDE_RECONNECT_STEPS=0` optionally allows a fixed number (1–128) of local
connector actions after a nearby route deviation. It preserves a complete
current-goal suffix and updates intended-edge counts exactly. Goal/protection
changes invalidate the route; failed connectors use ordinary route admission.
The experiment uses 16 actions. No action plan is exposed before complete
search and collision validation.
