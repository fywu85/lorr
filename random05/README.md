# PILOT: independent combined planner and scheduler

[Active five-instance goal](ACTIVE_GOAL.md), [competition dashboard](../PILOT_PROGRESS.md),
[RANDOM-05 results](RESULTS.md), [all five RANDOM cases](RANDOM_PROGRESS.md),
and the [completed 4,000-task audit](results/completion-audit-4000/audit.json).

A new implementation guided by the colleague's development log supplied by the
user. Their code and tuned traffic field are unavailable. The campaign now covers all five archived RANDOM densities. RANDOM-05 has
passed the 4,000-task target and frozen fresh-input validation. Absolute scores are not directly
comparable to the colleague's private synthetic instances. Large maps are
outside the current development scope.

[Throughput history](../RANDOM05_PROGRESS.md) records each verified best with a
UTC timestamp, source commit, settings, and linked benchmark evidence. Published
scores, matched local max(NMS, Kitty Knight), single-seed bests, and replicated results are
identified separately. Exact frontier cases are saved in `best.json`,
`best-four-cores.json`, and `best-32-workers.json`. The history audit is run with
`python3 random05/tools/audit_progress.py`; it requires the frozen local builds
and traces. It also generates [order-latency history](WAITING_PROGRESS.md),
including unfinished-order ages. Throughput remains the primary objective.

The solver combines exact oriented task-chain costs, reassignment of unopened
tasks, a two-step PIBT pipeline, idle pre-rotation, and parallel look-ahead over
persistent randomized priorities. Experiments cover greedy or exact assignment,
static traffic fields, local search, cycle coordination, and pocket evacuation.
Started tasks remain assigned to their original robot.

Map-tuned guidance, capped priority aging, and known-horizon abandonment require
the matching `--trick RANDOM-01` through `--trick RANDOM-05`. Known-horizon abandonment suppresses a robot's planning goal;
it does not illegally unassign its started task. These throughput-oriented choices
are explicit experiments and carry no fairness claim. Explicit task-progress
rank weighting (`R05_SCORE_RANK_POWER`) applies to reactive look-ahead and
windowed LNS costs. It favors shorter remaining chains and requires a trick flag;
a declared `R05_SCORE_RANK_STEPS` can limit it to startup.

Optional `R05_MATCH_FEASIBLE=1` also requires a known `R05_HORIZON` and the
matching trick flag. It rejects unopened task/robot pairs whose physical
collision-free completion lower bound exceeds the remaining steps, using an
independent unit-action metric. Opened tasks remain locked; no task leaves the
visible pool. Idle alternatives preserve admission caps and configured idle
prices. Without an idle price, joint matching prefers maximum feasible
cardinality before the existing cost objective. The option is experimental and
disabled by default; see [the diagnosis](results/all-random-bottlenecks-v2/REPORT.md).

Development uses a fixed number of complete futures (`R05_K`) of depth
`R05_DEPTH`. An optional `R05_FIRST_K` declares a smaller fixed count for
timestep 0 to leave room for startup work. `R05_THREADS` controls parallel workers. A deadline overrun raises an
error; a partial portfolio is not silently returned. Actual and promised spatial
transitions are collision-certified, and the simulator independently validates
complete 2,000-step runs. Regression tests cover chained costs, task locks,
collision rejection, dense turnover, deterministic worker counts, and eviction.

Optional staged search uses `R05_SCREEN_BRANCHES=s` and
`R05_SCREEN_KEEP=q`. Every candidate receives `s` complete futures; the best
`1/q` then receive all `R05_CONTINUATIONS=B` futures before final selection.
The incumbent anchor occupies one of those survivor slots. Each generation
must allow at least two survivors so the screened leader also survives.
`R05_K` remains the exact total of branch evaluations, with work `q*s+B-s`
per group of `q` roots. Only fully evaluated candidates become parents or
persist between real steps. For example, K5120/B14/s2/q4 over four generations
tests1,024 roots and fully evaluates256. Screening is disabled by default.

`R05_LOCAL` adds a fixed rollout allowance for sequential regional refinements
of the best priority vector. One proposal evaluates all `B` continuations, so
five proposals require `R05_LOCAL=5*B`; an incomplete remainder is unused.
Optional `R05_PARALLEL_CONTINUATIONS=1` evaluates those independent branches
concurrently when no outer search team is active. It preserves the original
reduction and proposal order, avoids nested active teams and completes all
branches. Defaults are local work0 and parallel continuations off. This is an
experimental exact runtime optimization, with throughput depending separately
on the chosen local-search budget.

`R05_BRANCH_DIAGNOSTICS=N` separately logs already-computed branch scores every
N steps without altering search decisions. Use `tools/audit_branch_ranking.py`
on completed diagnostic runs to measure screening recall. The diagnostic and
staged modes are mutually exclusive. [Measured screening evidence](results/branch-ranking-20260920/REPORT.md)
precedes the staged throughput experiments; recall alone is not a throughput gain.

Build and test on GRID:

```sh
python3 random05/tools/grid.py submit --kind build --output runs/random05/my-build
```

Add `--ipo` to the build command to test interprocedural optimization
(`-DPILOT_IPO=ON` in CMake). The frozen build specification records this choice.
It keeps the ordinary instruction-set and floating-point options; full trace
comparisons check that a compiler optimization preserves decisions.

For a benchmark, copy a case JSON from `experiments/`, set its binary to the
completed build, then submit:

```sh
python3 random05/tools/grid.py submit --kind benchmark \
  --output runs/random05/my-benchmark --cases path/to/cases.json
```

The runner freezes source/binaries, hashes inputs and guidance files, reserves
physical cores through GRID, verifies binding, and places concurrent cases on
disjoint cores. Resource rejections produce terminal invalid summaries before
any solver starts; they are distinguished from running jobs. Each solver process has a 32 GB address-space cap. `results/`
contains compact evidence; `runs/` contains ignored binaries and full traces.
Experimental settings stay off unless explicitly enabled; use the configuration
in the frontier's evidence to reproduce it rather than assuming defaults are best.

For matched-hardware validation, add `--hosts 'research33*|research39*'` and
`--cpu-model 'AMD EPYC 9354 32-Core Processor'` to benchmark submission. The
runner selects queue instances on those known hosts and verifies the CPU model
before starting. Exploratory runs may use other CPU models; record that
explicitly and validate promising results on the reference hardware.

For mixed-duration batches, use `python3 random05/tools/split_grid.py submit`
with the same output, cases, hosts, and CPU-model arguments. Each case gets an
independent GRID allocation. Run `split_grid.py collect --output <batch>` to
assemble completed summaries; each child directory retains its own provenance.


Fresh task/start validation is documented separately in
[FRESH_VALIDATION.md](FRESH_VALIDATION.md), with frozen settings and two NMS
repetitions per input. Audit the completed comparison with:

```sh
python3 random05/tools/audit_fresh.py \
  --batch random05/results/fresh-validation-split-full-v31 \
  --output random05/results/fresh-validation-v1/audit.json
```

This checks the predeclared manifest at commit22e7cd1, candidate/reference binary
hashes, generated input hashes, matched physical cores/CPU model, complete valid
runs, latency and peak RAM. It needs the original generated inputs (recreatable
with the documented generator). Do not tune on that validation set.


An earlier completed fresh-input comparison is [validation V3](FRESH_VALIDATION_V3.md):
a frozen four-core configuration beats the stronger of two NMS repetitions by
26.38% and24.54% on two newly generated task/start inputs,25.46% combined.
All six runs pass the full2,000-step, strict1s and32GB checks. These inputs remain
excluded from tuning. The original protocol is pinned at81bdfbd; later selected
records must not replace its candidate.

```sh
python3 random05/tools/audit_fresh.py \
  --batch random05/results/fresh-validation-v3-split-full-v52 \
  --output random05/results/fresh-validation-v3/audit.json \
  --protocol-commit 81bdfbd \
  --protocol-json random05/experiments/fresh-validation-v3-protocol.json
```


The latest completed comparison is [validation V4](FRESH_VALIDATION_V4.md), declared
at commit a7bad0c before generating seeds 50007/50008. It uses the 3,770-task
four-core staged-search configuration and two NMS4 repetitions per input.
It scores 3,680 and 3,641 against stronger NMS repetitions 2,907 and 2,930:
**+25.42% combined**, with all six original attempts valid. Later development
records do not replace this frozen candidate.
All task/start inputs 50001–50008 remain excluded from tuning.

The current archived-input frontiers also pass an independent
[full movement and task-work replay](results/action-audit-frontiers-v65/REPORT.md).
This checks waypoint visits and locked-task assignments as well as collisions.
A current waiting report can be regenerated after its event audit with
`python3 random05/tools/render_waiting_report.py`; it selects the runs in the
best manifests and validates their raw-result hashes.


## General windowed planner

`R05_WINDOW=15` or20 enables fixed-work rolling-window LNS using the existing
combined scheduler and exact oriented task-chain costs. A short prefix of the
previous plan and pipelined tail supply a complete collision-free starting plan.
Independent parallel searches repair small groups with time-space A*. Every
configured iteration finishes; deadline overruns still fail the entry.

`R05_WINDOW_KEEP`, `R05_WINDOW_ISLANDS`, `R05_WINDOW_ITERS`,
`R05_WINDOW_NEIGHBORHOOD` and `R05_WINDOW_EXPANSIONS` declare the search budget.
The windowed mode uses these counts instead of the reactive `R05_K` portfolio.
`R05_WINDOW_ITERS` accepts 0–8,192 complete repair attempts per island; larger
explicit budgets still have to pass the strict per-step deadline.
`R05_WINDOW_BLOCKERS=1` selects repair groups from route/reservation conflicts;
`R05_WINDOW_EQUAL=1` permits changed paths with equal objective values. Both
options are experimental and default off. All instance-specific guidance and
horizon rules retain their explicit trick flag.

[Per-case configurations and evidence](random-frontiers.json) distinguish
geometry-independent variants from declared tricks. They are selected records,
not an automatic instance-to-configuration dispatcher. Selecting configurations
by instance name must be exposed as a trick. Verify new batches with
`tools/audit_random_cases.py`; promote audited RANDOM-01..04 improvements with
`tools/promote_random_records.py`. RANDOM-05 also requires its main frontier
and waiting-history audits.

`R05_WINDOW_STARTS` (default1) adds a fixed portfolio of complete pipeline seeds before LNS. Extra starts use `R05_NOISE`; they are ranked by predicted remaining task cost across the window. The one-start default preserves the previous initializer. No wall-clock cutoff shortens this portfolio.

`R05_FUSE_CACHE_HITS=1` is an experimental exact optimization: it copies a
cached move ranking during its first lookup, avoiding a second read pass over
the sparse cache. It defaults off and changes no candidate score, priority,
collision rule, or work budget. Full trajectory equivalence and runtime
comparisons decide whether it is useful.

`R05_WINDOW_REUSE=1` retains each worker's time-space A* storage between repairs
and simulation steps. Epoch tags isolate searches; the option must preserve
complete trajectories. `R05_WINDOW_ROUNDS=r` adds deterministic plan sharing:
all islands start the next round from the best complete plan in the previous
round. The fixed total iterations per island must be divisible by r (1–32).
Defaults remain reuse off and one round; no wall-clock cutoff changes the work.


`R05_WINDOW_REPAIR_ORDERS=3` adds a third complete repair attempt after the
sampled order and its reverse. It plans the last blocked member first; if both
orders succeeded, it leads with the route having the largest excess cost.
Every attempt uses the same outside reservations and fixed search limit. The
best complete group is retained. Defaults remain one order; three orders are
experimental and need throughput/runtime comparisons at a declared budget.

`R05_WINDOW_COMPONENT_REPAIR=1` accepts compatible improving components of
completed window repairs; `2` also salvages components after a bounded repair
failure. Unplanned dependencies retain their old paths. Default0 preserves the
previous whole-group acceptance. This optional general mechanism requires
`R05_WINDOW>0` and still completes every declared repair order.


`R05_MATCH_AUCTION=epsilon` selects bounded capacitated auction matching when the
eligible-agent count is at most`R05_HUNGARIAN`. Default0 uses Hungarian assignment.
`R05_AUCTION_BIDS` sets the fixed bid limit per eligible agent (default128).
Completed proposals certify price tolerance and caps; budget exhaustion falls
back to a complete Hungarian assignment. There is no timed partial schedule.
Nonzero tolerance can change throughput.

## Exact runtime controls

`R05_MATCH_SIMD=1` uses runtime-dispatched AVX2 Hungarian relaxations, with the
same double-precision operations, stable column tie breaking and complete
augmentations. Unsupported processors and the alternative free-column tie
policy retain the scalar scan. `R05_MATCH_SKIP_ZERO=1` avoids no-op dual updates;
`R05_COMPACT_IDLE=1` avoids redundant identical free optional columns.

`R05_PERSISTENT_WORKER=1` runs initialization and each complete planning call
synchronously on one solver-owned thread. The competition driver otherwise
creates a new caller thread each step, losing its OpenMP team and thread-local
workspace. The worker does no planning between calls, uses only currently
revealed tasks, and joins before solver/environment destruction. The original
simulator clock includes dispatch and waiting. These are general implementation
controls, independent of the explicitly gated map-specific presets.

`R05_DYNAMIC_WORK=1` distributes independent fixed rollout items and shared
ranking builds dynamically. Random streams, candidate indices, complete work
and final ordered selection are unchanged. It also schedules whole independent
LNS islands dynamically; each island retains its own seeded search.

Portable interprocedural optimization is available with `-DPILOT_IPO=ON`. GRID
measurements also record `OMP_PROC_BIND=spread` and `OMP_PLACES=threads` inside
the verified16physical-core/32worker allocation. Exact regression and prefix
checks and full-run qualification are complete: ten valid full planner seeds per
instance, 50,000 planning calls total, worst885.879ms and peak500.015MB RSS.
RANDOM-05 additionally sets `OMP_WAIT_POLICY=PASSIVE` and `GOMP_SPINCOUNT=0`.
See [the frozen statistics and runtime report](results/random45-reliability-dynamic-split-full-v174/REPORT.md).
Selected runtime flags and exact binaries are in `random-frontiers.json`;
source173 is retained for RANDOM-03 and source174 for RANDOM-04/05.
All presets keep their original fixed budgets and explicit trick flags.
