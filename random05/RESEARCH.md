# Follow-up directions for RANDOM-05

Updated 2026-09-20. These are research hypotheses, not benchmark gains.

## Reuse and repair multi-action operations

The 2024 winner's authors describe EPIBT with short action sequences, bounded
revisits, and inheritance of unexecuted operations. Their tests include800
robots on this map, but use fixed per-agent task sequences and a1000-step
horizon, so their scores are not combined-track comparisons.
[Paper](https://arxiv.org/abs/2511.09193).

Read-only source inspected at
[`Straple/EPIBT@3808e832`](https://github.com/Straple/EPIBT/tree/3808e832593c9fb46b3e119cbb36035cccd830e1),
MIT license. The local checkout is `/tmp/r05-epibt-reference`; an optional operation policy now adapts its reservation/revisit structure.
The MIT notice is retained under `licenses/EPIBT-MIT.txt`.

A possible controlled adaptation would add an optional three-step operation
kernel beneath our existing rollout search and scheduler. It must preserve an
entire valid inherited plan as fallback, reserve both vertices and edges at
all three times, bound revisits, and carry future operations through the
selected rollout into the next real step. This is more flexible than our
failed all-or-nothing intent-component policy. Start with collision and
inheritance regressions, then compare fixed work and full2000-step results.

## Guidance directions versus cost scale

Mixed Guidance Graph Optimization separates edge-direction choices from their
weights and enforces strong connectivity. Its direction mutation operators
include edges, neighborhoods and cycles. This suggests local changes around
our successful generated field, rather than only drawing unrelated fields.
[Paper](https://arxiv.org/abs/2602.23468).

Our fixed-normalization-reference experiment isolates counterflow price from
preferred-edge scale, but its K128 gain disappeared at K1024. It does not yet
justify a new field or hard one-way restrictions.

## Current bounded tests

- Penalize adjacent opposite rotations in rollout evaluation; track the previous
  executed action across real steps so the boundary is scored correctly.
- Increase local priority-refinement trials while keeping the global search fixed.
- Mutate priorities inside one spatial neighborhood per candidate, retaining a
  quarter of fully fresh global candidates. Radius0 preserves the existing
  portfolio. This tests whether global mutations disrupt too many independent
  movements for the look-ahead score to retain useful local changes.

Promote only full valid runs. Record a single-seed maximum separately from
replicated planner-seed means and from fresh-instance validation.

Full K1024 tests now reject all listed rotation-penalty, extra-local-search and
regional-mutation settings on the control seed. Startup length preferences also
lose. These optional paths remain off; see RANDOM05_PROGRESS.md for exact scores.

## Three-step prototype

`R05_OPERATIONS=3` uses precomputed action sequences grouped by identical vertex
reservation footprints. It ranks their headings against our exact task-chain
costs, reserves vertices and opposite-edge exclusions for all three times,
and allows bounded lower-priority revisits with rollback to inherited plans.
The remaining valid suffix is carried into the next simulated and real step.
The existing pipeline remains the default. Unlike the upstream timer-driven
search, this integration completes a fixed rollout count and reports a timeout
if the entry deadline is exceeded. Full performance evidence is pending.

A one-visit ablation stalled in the dense toy simulation after redundant turns
were removed. It is rejected as a performance candidate. The selected four-visit
policy and its no-inheritance ablation retain the full mobility/collision
regressions; no liveness guarantee is claimed for arbitrary revisit settings.

The first full-map operation prototype is weak (K1=152, K8=1,018, K32=1,019,
K128=1,246). Inspection found a concrete difference from the reference: it
excludes the all-wait operation from active candidates, whereas our grouped
stationary footprint could stop search before considering temporary detours.
`R05_OPERATION_MOVING=1` tests moving footprints only, while retaining the
entire inherited plan as the safe fallback when all repairs fail. The default
remains0; full-run results, not this rationale, will decide whether it helps.

## Terminal task completion reward

Our rollout keeps each robot's current task chain fixed for its short horizon.
After completion it predicts an idle robot, without assigning a new task. The
sum of distance decreases may therefore undervalue finishing a chain relative
to making equal progress on a longer chain. `R05_COMPLETE_BONUS` adds an optional
reward for each whole chain completed within the simulated horizon; default0
preserves the existing objective. This is distinct from urgency-weighted distance
or an assignment length preference. A two-robot contested-goal regression checks
that the reward can select the completing task. Full benchmarks must establish
whether this hypothesis translates to throughput; none is assumed.

`R05_ROLLOUT_MATCH=1` is the second test of that diagnosis. After a simulated
completion, a greedy joint matcher assigns a replacement from tasks that were
visible and unassigned at the real step's start. Each rollout has its own mask,
so a task cannot be duplicated. Existing assignments stay fixed, and no hidden
future tasks or real schedules are modified. Progress is accumulated between
assignments; the cost of newly assigned work is rebased instead of counted as
negative progress. The forecast is still approximate: it does not continuously
rematch unopened tasks or predict newly revealed tasks. Default0 preserves the
prior behavior. Dense regressions check no-spare-task equivalence, task locks,
collision freedom, and worker-count determinism with spare tasks.

## Waiting-time diagnosis and early forward augmentation

The full replay audit in `results/action-audit-v39/REPORT.md` controls for task
IDs completed by both solvers. It finds longer loaded waits in our trajectories,
including35.26 extra steps per common completed task against NMS32. This is an
observed difference, not a causal savings estimate.

`R05_EARLY_FILL=1` tests whether the pipeline delays safe motion. It preserves
all previously promised forward moves, then considers only currently idle robots
that can move forward in their present heading. A chain must end at an available
cell after existing promises; a cycle must be at least3 agents (on this grid,
valid cycles are at least4). Incoming tails of cycles stay put. Aggregate cost-to-go
gain must exceed `R05_EARLY_FILL_GAIN` and is measured against the best idle
quarter-turn, to account for the turn opportunity lost. The resulting first move
and the following promise are certified normally. This is a generic motion
hypothesis, not a map-coordinate rule. Defaultoff; full validation pending.

Full early-fill comparisons now reject this implementation: thresholds 0/1/4
score 3,369/3,355/3,381 versus 3,395. Without the horizon cutoff, it scores
3,101 versus 3,197. Every run is valid; the unchanged control reproduces its
complete trajectory. Safe additional moves do not automatically improve the
sequence of future decisions. Leave the feature off.


## Confidence in generated traffic directions

The generated field gives every opposing direction the same contrast, even
when aggregate forward and reverse demand are almost equal. NMS's Random05
field instead contains many unmarked cells with equal directional costs.
Its imported field previously lost at K64 (2,446 versus 2,729); that does not
establish that a softer field helps our current solver.

`R05_FLOW_CONFIDENCE_POWER` tests a narrower hypothesis on our existing field.
At exponent p>0, multiply the opposing-direction penalty by
`(|forward - reverse| / max(1, forward + reverse))^p`. Preserve the preferred
edge, turn costs and physical connectivity; retain global normalization.
Exponent0 exactly preserves the existing field. This operates on aggregate
traffic demand, with no map-coordinate rules, but remains a declared guidance
trick under `--trick RANDOM-05`. Full-run evidence is required before use.


## Remaining differences from the public operation policy

Read-only inspection of the pinned EPIBT source confirms two separable differences
from our experimental three-step kernel. Upstream leaves successful robots marked
for the remainder of a search pass; ours clears that protection on success. It
also excludes operations with rotations after the final forward step; ours keeps
those rotations to improve the terminal oriented cost. Our moving-only ablation
excluded stationary footprints but still permitted rotations after a forward step.

Neither difference is established as a bottleneck. If returning to that kernel,
test successful-path protection and terminal-rotation removal separately, preserving
the full inherited valid fallback and deterministic work budget. Its current best
2,106 is well below the pipeline, so do not present it as a faithful replication of
the reference's measured performance. References are the pinned checkout's
`src/planner/epibt/epibt.cpp` (`build_impl`) and `operations.cpp` (`verify_operation`),
at the public source commit linked above.


The confidence experiment is now complete: exponents0/.25/.5/1/2 score
3,395/3,295/3,200/3,153/2,899. All are valid, and the default trajectory is
unchanged. Keep exponent0. This rejects the tested softening rule, not all
possible changes to guidance.

## Future continuation uncertainty

More global futures have not reliably improved throughput even when computation
fits the deadline. A possible next test is to keep the first simulated decision
fixed and evaluate it under several independently perturbed future priorities,
using their mean score. The current search judges each priority vector by one
future that keeps those offsets throughout the horizon. Comparing several
continuations could reduce preference for a good predicted tail that is unlikely
to survive replanning. This is an untested hypothesis.

A controlled version would keep total simulated futures fixed, require an integer
number of continuations per root, and preserve exact existing behavior at one
continuation. All branches must share the root's first actions and pending moves;
only later decisions may change. Use common deterministic continuation draws for
comparing roots and preserve worker-count determinism. Benchmark the full horizon;
short screens and promising model scores have repeatedly misranked candidates.


`R05_CONTINUATIONS=B` now implements the continuation test (default1).
`R05_K` remains the total number of global rollouts, divided among `K/B` root
priority vectors. Branch0 keeps the root's offsets throughout; the other B-1
branches mutate offsets after `R05_CONTINUATION_START` steps (default1), with
per-agent probability `R05_FUTURE_MUTATION` (default0.3) at each later step.
All roots use common deterministic mutation draws from an independent stream.
The first actions, positions, headings, stages and pending moves must match
across branches; the evaluator checks this invariant before averaging scores.

Local refinement spends complete B-rollout groups out of `R05_LOCAL`, leaving
any remainder unused. The primary full comparisons set local refinement to0,
so every variant executes exactly its specified K global rollouts per step.
Separate controls retain the full frontier and match the smaller root portfolio.
Dense task-turnover regressions check worker-count determinism and that duplicate
unchanged continuations preserve root selection. Full validation is pending;
no time-based early return or partial search result is introduced.


`R05_OPERATION_PROTECT=1` now retains successful recursion markers for the
remainder of the operation search pass. `R05_OPERATION_FINISH_MOVE=1` excludes
new candidate sequences containing a turn after their last forward action;
the inherited valid plan remains available. Both default to0 and are separate
from the earlier moving-footprint filter. Dense regressions cover each rule,
the combination, and worker determinism. A five-case full run compares them
against the existing K32/revisit64 prototype (1,973 tasks), with the rest of
its configuration fixed. These changes do not alter the default pipeline.


`R05_SHARE_PREFIX=1` optionally reuses the exact simulated state before the first
priority perturbation. The root's constant-priority rollout saves that state,
first actions/promises, task-turnover state and all accumulated score terms.
Other continuations resume there. Branch order and floating-point score operations
are preserved; only repeated policy steps are removed. With depth8, B4/start1
removes9.4% of policy advances; B8/start2 removes21.9%. These are operation counts,
not measured runtime savings. Dense checks exercise task turnover, completion
bonuses, discounting, reverse-turn costs, local refinement and two worker counts.
Full trajectory equivalence and timing still require measurement.

## Exact evaluation optimizations

`R05_PACKED_ORDER=1` sorts the priority and agent ID as one integer key. The
priority uses an order-preserving IEEE float transform; signed zero is normalized,
and nonfinite values retain the original sorting path. Equal priorities preserve
ascending agent ID exactly. This removes indirect priority-table reads from the
sort comparator without changing the selected order.

`R05_FAST_DISPERSION=1` precomputes the geometric proximity graph used by the
existing 5-by-5 dispersion penalty. At high occupancy it counts occupied pairs
from all free-cell pairs minus the degrees of empty cells, adding back empty-empty
pairs once. At low occupancy it counts occupied neighbors directly. Both branches
compute the same integer pair count, with no new objective or map-specific rule.

Build-v45 passes ordering tests with random values, ties, signed zeros and
nonfinite fallback, independent geometric pair counts across every occupancy
of an obstacle map, and full dense task-turnover equivalence across worker counts.
The features default off. Full-map trajectory and latency comparisons are required
before claiming performance improvements.

`R05_SCRATCH_REUSE=1` additionally retains each worker's temporary policy arrays
between simulated steps. Ownership, reservations and heading constraints are
reset explicitly; overwritten arrays retain capacity. Frame positions and
promises exchange their vector storage with the scratch arrays after each step.
The spatial/kinematic policy no longer needs a temporary copy of the chosen
assignment when filling its intent buffer. Scratch state is thread-local.
Build-v46 passes dense turnover, component-policy, cycle-preparation and worker
identity regressions. Full trajectories and timings remain required.

## Continuation score variance

Averaging several future priorities improves the development input, but selecting
from hundreds of roots may still favor decisions that scored well because of a
few favorable sampled tails. `R05_CONTINUATION_RISK` tests a penalty equal to the
population standard deviation of continuation scores times the configured weight.
It leaves the same roots, future draws, first decision and fixed work budget.
Weight zero preserves the existing mean and its floating-point summation order.
Identical continuations have zero penalty. Dense worker-identity tests pass in
build-v47. This is a hypothesis; full throughput comparisons decide whether to use it.

The same new mean-score regime also warrants rechecking two/four/eight search
generations at fixed total work. Those settings lost under single-continuation
scoring; the changed evaluation landscape provides a specific reason to retest.

## First-step compute diagnosis

The large four-core portfolios miss the deadline at the first step. Optional
`R05_PROFILE=1` reports wall time for assignment, task-chain cost preparation,
candidate preparation, parallel look-ahead and final work. It does not change
candidate counts or introduce early termination. Diagnostic prefixes are only
for timing; full 2,000-step runs remain the throughput selection criterion.
The simulator reveals the initial task pool after initialization, so moving its
assignment into preprocessing would require information the interface has not
yet exposed. Investigate actual measured costs before further optimization.

## Recheck assignment from the committed next position

The pipeline's first forward/wait move is already promised before the scheduler
runs. Task completion is checked after executing the action. The existing
`R05_SCHED_PREDICT=1` option therefore prices the approach from that committed
next position; it retains the current orientation, which is exact for committed
forward moves. It avoids assigning a zero-approach task at a cell the robot must
leave immediately. Started tasks remain locked.

This option was last tested in the v8 greedy-matching configuration (2,669 tasks),
well before full guided Hungarian matching and averaged continuation search.
That changed scheduler/search coupling is a concrete reason to recheck it,
without assuming the old small gain will persist. Test full K2048 andK5120 with
four generations against their completed current-position controls3520/3637.
No hidden tasks, uncommitted future moves, or map-coordinate rules are used.
The separately frozen fresh-validation V3 candidate remains unchanged.

## Current search conclusions

Averaging several continuations now has positive full-run and planner-seed
evidence; it is no longer merely an untested hypothesis. The exact CPU changes
make K5120/B8/four generations feasible on four cores, reaching3,637 tasks.
Positive variance penalties failed; a negative coefficient's small single-seed
gain also failed across five planner seeds. Keep the mean score.

Retaining four distinct elite parents between four search generations is the
latest promising general change:3,618 versus3,520 at K2048/B8. Larger elite
counts are not uniformly better. Full seed and compute-scaling comparisons are
running. The third fresh-input validation tests the separately frozen3,637
configuration and must not be relabeled as validation of these newer variants.


The completed K2048/four-parent seed check is effectively neutral on average:
3,531.8 versus3,531.4 across planner seeds0–4, with only two positive pairs.
At K8192, eight parents give a selected3,689 on seed3, compared with3,582 for
one parent/four generations and3,596 for one generation. Preserve the maximum,
but do not call this a replicated average gain.

Next hypothesis: carry several evaluated priority vectors across real steps.
The current solver carries only the winner, discarding alternative coordination
patterns after each step. Reuse only vectors as candidate parents; every score
must be recomputed from current poses, assignments, ages and committed moves.
Keep the same fixed rollout budget, the selected incumbent as the first anchor,
and globally randomized candidates. A carry count of one must preserve the
existing full trajectory. No map-specific rule or fairness objective is added.


`R05_PERSIST_ELITES` implements this hypothesis (default1). It retains distinct
priority vectors ranked on the completed current search, with the chosen winner
first. At the next real step, non-global first-generation slots try each saved
vector unchanged once and then mutate parents in rotation. Fully global slots
remain unchanged. Scores, plans and reservations are recomputed from current
state; only vectors persist. Fixed global rollout work is unchanged. Dense
regressions cover task replacement, future-task reassignment, duplicate vectors
and worker determinism. Full default-control trajectory checks and throughput
measurements are required before promotion.


A next general hypothesis is to reduce mutation probability in later search
generations. Constant0.3 mutation replaces roughly240 priority offsets per root
on800robots, even after a useful coordinated pattern has been found. A smaller
late mutation may refine that pattern while retaining the existing fully random
candidates for exploration. `R05_MUTATION_DECAY` defaults to1; generationg uses
`mutation * decay^g`. It changes neither the total rollout budget nor global
restart amplitude/frequency. Values0.25/0.5/0.75 are experimental, with unchanged
controls. No performance gain is claimed before full runs and seed checks.


The first-step deadline constrains the current four-core fixed budget: across
all five K5120 development planner seeds, the maximum924–928ms is always step0.
After the first five steps, maxima are839–855ms. Initial matching and task-cost
setup account for this extra work. `R05_FIRST_K` optionally declares a smaller
rollout budget only at timestep0; zero disables it. Regular K applies thereafter.
Both counts are fixed before execution, divisible by the continuation count,
and must supply enough roots for generations/elites. No elapsed-time cutoff or
partial search is introduced. This general startup-work allowance may let more
search fit in later steps; strict full-run timing checks are still required.


The first-K5120/regular-K6144 four-core attempt exits124 at timestep2 with
1000.533ms compute. Preserve this failed strict1s attempt; do not count a partial
score or silently retry. Smaller5632/5888 full runs remain underway.

A bounded implementation experiment increases candidate-ranking cache capacity.
The original64 slots per agent/worker were a fixed memory choice, while current
RSS is far below32GB. `R05_CACHE_SLOTS` accepts powers of two8–1024 (default64).
Epoch, chain identity, stage, pose and moving-state keys and the neighbor-dependent
push-cost bypass stay unchanged. Larger tables may reduce repeated cache misses,
though their memory footprint could also hurt locality. Dense tests stress
frequent eviction and compare128/256 slots against uncached exact trajectories.
Full trajectory and timing evidence is required before any performance claim.


At the main budgets, lowering late mutation loses: K5120 drops from3637 to
3462–3530; K8192/E8/carry8 from3705 to3627–3702. Test the complementary
hypothesis: broader constant ROOT mutations0.5/1 instead of0.3. Continuation
mutation remains0.3; the rollout count, generations, noise amplitude and scoring
remain unchanged. Use source05559b7 against its existing controls. Separately,
K5120/E8/carry8 now scores3655 onfourcores (vs3637 E1/carry1,3610 E8/carry1);
five planner seeds will test whether this selected gain persists. Neither
experiment uses fresh-validation inputs.


Completed controls reject stronger root mutation: at K5120/E1, mutation0.5/1
scores3545/3460 versus3637; at K8192/E8/P8,3537/3472 versus3705. Retain0.3.
K5120/E8/P8 improves the selected four-core record to3655, but five seeds average
3589.8 versus3606.0 forE1/P1, with only one positive pair. Do not claim a mean gain.
FirstK5120/regular5632and5888 are valid onfourcores,3650/3591;6144 cache64 fails.
The corresponding fast32worker runs score3650/3591/3545. Increasing K is not
monotonically beneficial. Larger cache512 preserves3637 and lowers mean four-core
latency791.0->772.0ms (2.4%), max920.2->901.0ms, RSS308->446MB.

The current action replay finds extra loaded waits on common completed orders:
about17 more per task onfourcores and25 on32workers, despite fewer turns/forwards.
This observational accounting motivates testing correlated future controls.
Blend random continuation offsets with prior-step elite priority vectors, using
the same mutation masks and RNG consumption. Only offsets persist; every future
state/action/score is simulated anew. Default blend0 must preserve the full
trajectory. No map-specific condition or fairness objective is introduced.

`R05_FUTURE_ELITE_BLEND` implements the optional continuation experiment, default0.
Each noisy continuation selects one retained vector in rank order (cyclic if
needed); at the existing mutation-mask positions, its offset is blended with
the same random draw. Blend1 uses the retained offset exactly. Before history
exists, use the original random futures. Values outside[0,1] and positiveblend
with fewer than two retained vectors are rejected. All roots see the same
continuations, branchzero remains constant-offset, and total work is unchanged.
Regressions cover zero-mutation invariance, task turnover, virtual task assignment
and worker determinism. Full default controls must reproduce3655/3705 before
any new score is accepted.

Separately test larger fixed budgets for the persistent-parent policy: K16384
with8or16 continuations, and K8192 with16 continuations. This distinguishes
spending extra work on more root candidates from reducing noise in each root's
estimated future value. Keep fourgenerations/E8/P8 and all other settings fixed,
source05559b7, against existingK8192/B8=3705. Larger budgets have not uniformly
helped; these are bounded full-run tests, not an assumption of monotonic scaling.


Current four-core sampled profiling moves the CPU bottleneck: candidate ranking
is about5% of policy time; optimistic and kinematic PIBT passes about16% and26%.
Test exact cached kinematic eligibility. The ranking key already includes task,
stage, position, heading and moving state. R05_KINEMATIC_MASK (default0) stores
which sorted candidates obey the turn/move restriction. Executable PIBT visits
only these indices in original order, retaining every occupancy/reservation
check, failure fallback and expansion-limit rule. Separate compile-time pass
modes also remove the optimistic pass's dynamic kinematic branch. The behavior
must remain exact, including configurations with dynamic push costs or pinned
components. Full trajectory and timing controls are required before adopting it.

Full continuation-blend results: K5120 blend0/.5/1=3655/3528/3506;
K8192=3705/3724/3594. Both default controls exactly reproduce prior full
trajectories. The3724 selected record is a19-task gain; pairedseeds are running.
Do not infer uniform improvement from this budget-specific result.
Kinematic-mask build-v60 (60c5f9b) passes regressions9.44s; fullcontrols running.
Firstfew four-core samples show roughly7% lower lookahead time and26% less
time in executable PIBT. These are early CPU samples, not a full-run timing
or throughput claim. Full strict1s confirmations remain necessary.

Use the measured mask savings to attempt larger declared four-core budgets:
firstK5120, then6144or6400, each with futureeliteblend0or0.5, E8/P8/gen4 and
cache512. These are new fixed configurations, not retries of a failed setup.
No count depends on elapsedtime. Both the full deadline and throughput must
pass; larger K alone has repeatedly failed to improve throughput.

Three larger-four strict cases fail: K6144/blend0 atstep0=1036.945ms,
K6144/blend0.5 atstep0=1839.045ms, K6400/blend0 atstep1=1088.306ms.
Both step0 failures execute the same first5120 work that took about821ms in
the mask control. They do not establish that the larger later budget caused
the failure. Host/cache/frequency/scheduling interference is possible but not
isolated. Preserve all original failures. The user previously authorized a
relaxed development budget; separate declared5s copies of allfour cases will
measure full throughput and phase timings. Do not compare their score directly
to NMS's1s baseline or promote them into the strict1s frontier. Subsequent strict
confirmation or implementation improvement is required. Work counts stayfixed;
no time-based partial search or selection among incomplete runs.

At K16384, B16 scores3741 whereas B8 scores3643; K8192/B16 is3568.
The selected result favors using more work to evaluate each root's future,
while preserving1024 root candidates, over simply doubling roots at B8.
Test1024 roots with24 continuations and the retained-futureblend0.5 variant.
Also attempt32768/B32 with an explicitly smaller first-step16384 budget.
Use the exact kinematic-mask optimization, with a3741 full control atK16384/B16.
The32-worker mask on/off controls both reproduce the complete3724 trajectory;
mean latency231.5->221.9ms in that pair. Four-core controls are still running.
These higher-budget runs remain strict1s; no partial solution is permitted.

The K8192 future-blend five-seed check is complete. Blend0:3743/3492/3730/3705/3513,
mean3636.6; blend0.5:3577/3610/3693/3724/3660,mean3652.8 (+0.45%,3/5positive).
The highest single score3743 uses blend0/seed0. Preserve the small positive
mean and large seed variation; this is not independent-input validation.
Source d6a3e0f produced that record, full2000/strict1s/32workers, max401ms.

The fourth strict larger-four case (job8900399) never launched its solver:
GRID exposed64 physicalcores instead of4, and the preflight guard rejected it.
Qacct records exit1,0 wallseconds,0.074 CPUseconds; no native log or usage file.
A separate preflight-rejection record hashes the allocation/spec/scheduler log.
Allfour original attempts are terminal: three deadlines and one infrastructure
failure. Do not report the missing summary as an unfinished benchmark.
The high-budget32worker attempts also preserve two strict failures: K24576/B24
atstep0=1101.406ms; K32768/B32/first16384 atstep2=1148.664ms.

A potential next general search idea is staged allocation of continuations.
Karnin, Koren and Somekh's [Sequential Halving, ICML2013, section4](https://proceedings.mlr.press/v28/karnin13.pdf) allocates a fixed sampling budget
in rounds, eliminating poorer alternatives between rounds. Our possible
adaptation would screen candidate priority vectors with a few continuations,
then spend the remaining declared work on survivors. This is an inference,
not a MAPF result or a transfer of the paper's statistical guarantees.
Before implementing, measure whether2or4-branch estimates retain the eventual
best candidates under8or16 branches. Correlated continuation controls, the
constant branch and changing generated candidates make premature elimination
a real concern. Keep an incumbent anchor and compare finalists at equal sample
counts. All predeclared stages must finish; no clock-based early return.
A possible two-stageK5120 design per generation:256 candidates x2 branches,
then64 survivors x12 additional branches =1280 evaluations, fourgenerations.
This uses14 branches per finalist. It is only a proposal, not implemented.

Another exact CPU idea, also not implemented: represent stationary, eligible
cycle vertices as a bitset and precompute sparse word masks for each geometric
ring. Check rings in the existing order; clear their bits after an accepted
cycle. This could reduce the roughly13% of sampled policy time in cycle scans
without changing scores or collision behavior. It needs equivalence checks on
maps spanning multiple64-bit words, holes and overlapping accepted cycles.

The full v60 four-core mask off/on controls both reproduce every field of the
3655 trajectory. Means748.8/702.3ms, maxima881.7/825.7ms. Enabling the mask
saves6.2% in that pair; relative to the original v55/cache64 implementation
(790.7ms), the new cache512/specialized-pass/mask configuration saves11.2%.
Source60c5f9b is now the faster verified four-core manifest at the same score.
This does not erase the separate larger-budget deadline or allocation failures.

The K16384/B16/E8/P8 seed check is complete:3705/3617/3715/3741/3567,
mean3669.0, versus3636.6 forK8192/B8. This is+0.89%,3/5 positive pairs.
Its best3741 is slightly below the smaller-budget selected3743. More
continuations improve the five-seed mean modestly but have not improved the
maximum further. These remain planner seeds on one development input.


R05_CYCLE_MASK implements the optional exact cycle-scan optimization (default0).
Precompute each existing ring's sparse64-bit word masks over compact vertex IDs.
After executable PIBT and pre-cycle proposals, mark occupied stationary vertices
without forced headings. Keep the original ring order and gain arithmetic;
use the masks only for eligibility, clearing accepted rings before considering
overlapping ones. The legacy scan remains available for paired controls.
Regression cases decode all masks on an obstacle map spanning three words, and
compare dense multiword trajectories with task turnover, larger rings, forced
headings, overlapping proposals and one/two workers. No performance claim before
full timing and trajectory checks. This is a general implementation change.


Resource preflight failures now write a terminal invalid summary before returning.
A wrong CPU model, missing core binding or insufficient CPU quota still prevents
any solver launch. The collector can therefore distinguish rejected allocations
from jobs still running. A focused test covers all three external conditions,
asserts no child solver starts and checks the batch collector reports completion
with invalid status. The previously frozen rejected attempt is unchanged.

v61 cycle-mask regressions pass (11.25 seconds). Its full controls are next.
The separate relaxed-deadline v60 four-core search scores3576/3709 forK6144
with futureblend0/.5, and3648/3611 forK6400. The3709 candidate averages923ms
and peaks1495ms under a declared5s deadline; it is not a strict1s frontier.
Try the exact cycle optimization on it as well as the established3655 control.
Keep explicit first-step5120 work and regular6144; no time-based early return.


Before staged continuation allocation, add observational branch diagnostics.
R05_BRANCH_DIAGNOSTICS=N (default0) records each already-evaluated branch score
every N steps, in deterministic serial order after the workers finish. It adds
no rollouts or random draws and changes neither ranking nor selected actions.
Full controls must retain their complete trajectories. Offline analysis will
ask whether two/four branches keep the final best and top-eight candidates when
retaining one-quarter or one-half of each generation, always preserving its
first anchor. Report survival, top-eight recall and lost best full score; these
are screening diagnostics, not measured throughput gains from an unbuilt method.

The first diagnostic build passes regressions11.65s, but one strict32-worker
run fails at step0=1035ms. Individual score numbers were printed to unbuffered
stderr, causing thousands of writes per observed step. Buffer each generation
into one write before using the diagnostic for new measurements. Preserve the
failed original; this logging-only correction changes neither computed work nor
selection. Full trajectories still need verification.


Draft a two-stage fixed-work continuation search, default disabled. With
R05_SCREEN_BRANCHES=s and R05_SCREEN_KEEP=q, each generation evaluates all
roots on s common futures, retains exactly one in q (plus its anchor within
that quota), and evaluates those survivors on every remaining future up to B.
The work per q-root group is q*s+B-s; K must divide into complete groups in
every generation. No clocks alter work or return an unfinished search.
For example K5120/B14/s2/q4/generations4 tests1024 roots and fully evaluates256,
versus640 roots all evaluated atB8 in the currentK5120 control. Partial means
never compete with full means; only fully evaluated candidates become parents
or persist to the next real step. Cached shared prefixes and score accumulators
resume in branch order, with first-action/promise consistency checks. A runtime
branch-count assertion verifies the actual global work equals K.

Meaningful tests compare staged and exhaustive search with identical futures
and equal root streams, including explicit first-step budgets and shared-prefix
on/off. Nonzero-noise/risk cases check worker determinism, task turnover and
collision certificates. Full diagnostic results must justify screening before
choosing new throughput experiments; this draft has no throughput claim yet.

v64's exact-screening regression fails because its tiny first-step fixture
allowed only one finalist per generation: forcing the anchor then removed the
actual best. Tighten the work contract to require at least two finalists per
generation, so both anchor and best survive. Add explicit rejection coverage
and enlarge the matched-root fixture while preserving its smaller first step.
This is a failed test caught before any throughput experiment, retained in
build-v64; use a new frozen build for the correction.

The corrected staged implementation passes the complete regression suite13.73s.
Two completed diagnostic runs (3743 and3709) reproduce every trajectory field
of their prior versions. Across80 sampled generations per run, screening on
2branches and retaining25% preserves100% of the eventual winners, and99.1%/97.0%
of top-eight candidates. This is an observational result on sampled states, not
a guarantee for the new search. It supports full2000-step experiments at
K5120/B14/s2/q4 andK5120/B10/s2/q4, plusK8192/B10/s2/q4. All use fourgenerations,
E8/P8, existing policy/scheduler/tricks. Compare blends0/.5 where declared.
Keep unchanged-search controls at3655four and3743on32. TwoK5120/B14 candidates
also run directly on four cores under1s; larger32 runs first measure behavior.

The initial staged32-worker full runs score3670/3574 atK5120/B14/blend0/.5,
3635 atK5120/B10/blend0,3730/3572 atK8192/B10/seed0/blend0/.5, and3755 at
K8192/B10/seed3/blend0. The unchanged control exactly reproduces3743.
3755 is the new selected32-worker record (+18.4% vs matchedNMS3172), with
257ms mean/353ms max and530MB RSS. Relative to ordinaryK8192/B8: seed0 loses13
andseed3 gains50. Finish seeds1/2/4 before any replicated-improvement claim.

Use the remaining32-worker deadline headroom forK16384/B10/seeds0and3 and
K16320/B14/seed3, allscreen2/keep4/blend0. Also test declaredfirstK5120 then
K6000/B14 orK6144/B10 on both32workers andfourcores. ExistingK5120/B14 direct
four-core confirmations remain running. Every experiment is a full2000-step
strict1s run and completes its declared fixed budget; no partial-score promotion.

The four-core3709 configuration now completes under strict1s in sourcef81b760:
mean819.6ms, max888.5ms, RSS463MB. Its complete trajectory equals the earlier
relaxed source60c5f9b result. This reaches+27.3% versus matchedNMS4=2914 on the
development input. Do not attribute the full timing difference to cycle masks;
host conditions also differ, and the standalone cycle-mask four-core control
had a first-step1277.9ms failure. All prior failures remain archived.

Two larger staged32-worker cases fail at step0: K16384/B10/seed0=1093.3ms and
K16320/B14/seed3=1133.0ms. No throughput is assigned. Test new explicit startup
budgets8192 and8000 respectively, retaining the larger regular budgets. The
full-K16384/B10/seed3 run continues separately; its later sample lookahead is
about508ms. First-step reduction is fixed at configuration time, not chosen
from observed runtime. It can change the trajectory and requires full evaluation.

The first larger stagedK6000/B14/first5120 case scores3664 on32workers, below
3670 atK5120/B14. Increasing work is again non-monotonic. Test a narrower
comparison that preserves1024 roots and256 fully evaluated finalists: s2/B12
withK4608, s2/B16 withK5632, ands4/B16 withK7168; the existing s2/B14 usesK5120.
AlluseE8/P8/gen4/seed3/blend0. The ordinary fullB16/1024-root reference scored
3741 usingK16384. Staged searches can diverge through eliminated parents, so
identical trajectories are not assumed. The s2/B16/K5632 option also receives
a direct four-core strict1s run: the currentK5120/B14 first-step profile is
approximately827ms, leaving plausible room for this measured work increase.

The fixed1024-root comparisons score3718 at s2/B12/K4608,3695 at s2/B16/K5632,
and3664 at s4/B16/K7168. More continuation precision is not monotonic in actual
throughput. The B12/K4608 option has a direct four-core strict run underway.
First4608 thenB12/K5760 scores3770 on32workers, whileK6120 scores3660. Confirm
3770 on four cores with a paired worker-affinity experiment. These are new
configurations, not measurements of four-core throughput until confirmed.

Resource allocation already pins each process to its assigned CPUs. Separately
test OpenMP worker binding (false versus close, places=threads), keeping the
process allocation and algorithm identical. GNU documentation defines these
[worker-binding policies](https://gcc.gnu.org/onlinedocs/libgomp/OMP_005fPROC_005fBIND.html)
and [hardware-thread places](https://gcc.gnu.org/onlinedocs/libgomp/OMP_005fPLACES.html).
A local probe using the solver's same libgomp verifies all workers remain inside
a four-CPU process mask; close gives four distinct singleton masks. This checks
mechanism, not a speedup. Full paired runs on one GRID host will compare exact
traces, latency and usage: four cores with the3770 candidate, and32workers with
the3794 record. Both retain explicit runtime-affinity diagnostics. Do not infer
a benefit until complete; inspect actual logged places against each case mask.

Staged K8192/B10 five-seed results are3730/3642/3611/3755/3715, mean3690.6,
versus ordinaryK8192/B8 mean3636.6 (+1.485%,3/5positive). This is one input,
not independent-input validation. The direct four-coreK5120/B14 controls
reproduce3670/3574 exactly; the unmodified control reproduces3655. All strict.

New32-worker records:3776 atK16384/B10/seed3 (567ms mean,768ms max), then3794
atfirst8000/K16320/B14/seed3 (473ms mean,525ms max,578MB RSS), allscreen2/keep4,
g4/E8/P8/blend0. Both are source5f81613 and fully audited. The larger seed0
configuration withfirst8192 scores3667, so more budget is not uniformly better.
The new3794 record is+19.6% versus matchedNMS32; there is no five-seed or fresh
validation for it yet. Testfirst8000/K24000/B14 within remaining32-worker time.

Direct four-corefirst5120 thenK6000/B14 andK6144/B10 finish3664/3654, means818/
848ms, maxima891/912ms, exactly matching fast32 counterparts. Both lose to3709.
Worker-affinity diagnostics on the running paired jobs verify every observed
worker mask stays within its individual case's allocated CPUs; close uses
distinct singleton masks. Early timing is mixed; wait for full results.

## 4,000-task campaign: saved-state decision audit

The next target is4,000 tasks from the verified3,852 record. More work alone
failed (K28800=3706), so first measure whether the short-horizon score ranks
useful decisions. `R05_SNAPSHOT_EVERY` is an opt-in observational diagnostic;
it saves the pre-decision environment and solver state plus selected, fully
evaluated alternatives at expanding ranks. Cache contents are invalidated on
restore; immutable distance tables and exact task costs are rebuilt as needed.
There are no diagnostic random draws or changes to candidate selection.

`random05_decision_probe` is a separate offline executable. It restores a saved
state, reproduces the first decision exactly, forces one completed alternative,
and repeatedly replans for a declared number of steps at a declared fixed work
budget. The task stream is handled by the harness and revealed only upon real
simulated completions, following the simulator's pool-size rule. The planner
never receives unrevealed tasks. Record cumulative tasks/waypoints/motion at each
step under common downstream seeds. Reduced-budget continuations are a proxy,
not an oracle for the full-budget policy or a competition throughput result.
Dense ongoing-task regressions require exact action, schedule and persistent
state equivalence after restore, including both PRNG modes and task replacement.

## Motion-component recombination (new general experiment)

Saved alternatives often differ in10–80 separate groups of robots, including
small cycles and individual pre-rotations. This motivates combining whole motion
components rather than fixed spatial regions. At the first future time all roots
have the same positions because they execute the previous step's promises.
Connect robots whenever either parent's next promise enters an occupied source
or shares a destination with the other parent, including currently empty cells.
Choosing either complete parent within each connected component preserves both
vertex and edge-swap legality; certify every constructed hybrid anyway.

`R05_COMPONENT_TRIALS` (default0) and `R05_COMPONENT_ROUNDS` spend a declared extra
number of complete B-branch evaluations after the ordinary search. Donors are
fully evaluated distinct first decisions at expanding ranks. Shuffle the eligible
components using the independent local stream, test one donor component per
candidate, keep the best, and repeat. `R05_COMPONENT_MIN_AGENTS` can restrict
proposals to multi-robot motion. Force the proposed first decision in every
continuation, then simulate all later interactions normally. Reuse the original
common future draws; no unseen tasks, new map rules or elapsed-time stopping.
The baseline remains in the portfolio, and fixed extra work is checked exactly.

This differs from naive regional crossover by preserving the actual dependency
groups, including two parents competing for the same hole. Full-run improvement
is still a hypothesis. Regression checks cover that collision trap, independent
rotations, dense ongoing tasks, shared-prefix equivalence, worker determinism,
and saved-state replay. Forced first decisions must not change under averaging.

## Recheck completion value after continuation averaging

The seven saved-state probes show weak score/128-step completion correlations,
particularly in the later states. This is not proof of a causal model defect:
only eight correlated candidate decisions and three cheaper downstream seeds
were measured. Full-budget control reproduces the original trajectory metrics,
so the replay mechanism itself is independently checked.

An earlier completion bonus failed with single-future candidate scores. The
current14-future average and staged survivors can reduce that reward's sampling
noise, which is a concrete reason for a bounded recheck (bonuses2/4/8). Everything
else stays at the3,852 configuration, including horizon scale1.5, seed4 and fixed
K16320. Compare complete strict1s runs to the existing exact control; do not
assume a win or expand the sweep without evidence. This remains the same general
completion-value feature, not a new algorithm or an isolated test of averaging.

## [trick] Direction-aware deadline estimate with controlled units

The old cutoff treats two tasks with equal remaining hop distance alike even
when one requires travel against the preferred lanes or more turns. Raw weighted
costs were unsuccessful in the colleague's log, partly motivating step-unit
calibration. This experiment preserves the pool's total estimated work: normalize
each currently assigned oriented chain cost by sum(hop distances)/sum(chain
costs), then blend with its hop distance. Apply the same observed steps-per-cell
and cutoff scale. No hindsight, hidden future tasks or offline fitted labels.
`R05_TRIAGE_GUIDED_MIX` defaults0, stays in[0,1], and needs the declared horizon
and --trick RANDOM-05. Started assignments remain locked.

The normalization holds the aggregate distance estimate fixed and changes which
chains appear feasible. It is not a calibrated completion probability or a
guaranteed remaining-time bound. A controlled two-robot fixture distinguishes
a one-cell forward trip from an equal-distance trip requiring a half-turn; dense
checks cover task replacement, collision safety and worker determinism. Test
mix.5/1 at the existing scales1.25/1.5 only after regression validation.


## Replanning inside the forecast

The saved-state diagnostics show that a root's short averaged score does not
reliably rank its128-step completions under subsequent replanning. This is an
observational result on a small correlated sample, not proof of a causal defect.
Completion bonuses and motion-component recombination also fail their full runs.

The next bounded generic hypothesis is to rerank several fully evaluated first
decisions by simulating repeated solver calls with a smaller fixed inner budget.
Each inner step uses the same task matcher and pipeline, with deterministic
future seeds common to all compared roots. Only the currently visible task pool
is copied: completed tasks are removed, and no hidden replacements are read or
invented. Progress is measured along the task executed at each step, rebased
after reassignment. All selected roots and future steps finish before selection;
no elapsed-time stopping or partial answer is permitted.

The ordinary search still supplies the candidates and persistent priority
vectors. Immutable map and chain tables may be shared, while runtime state and
caches stay separate. A one-root control must preserve the full selected
trajectory and persistent state. Full2000-step benchmarks will decide whether
a more faithful, but smaller-budget, future policy is useful.

`R05_REPLAN_ROOTS` now implements this optional forecast (default0). Select
distinct first actions/promises at expanding score ranks. Each of
`R05_REPLAN_FUTURES` common-seed futures executes that first decision, then
`R05_REPLAN_STEPS-1` actual solver calls at fixed `R05_REPLAN_K`, with
`R05_REPLAN_CONTINUATIONS` branches per inner root and one search generation.
The inner calls run serially within each future; whole futures run in parallel.
No nested reranking, wall-clock stopping, or invented replacement tasks occurs.
All motion, task-lock and visible-pool accounting checks run in each forecast.

Source70 regressions passed27.78s, including exact one-root decisions and
persistent state over dense turnover, worker-count/checkpoint determinism, and
a depleted visible pool with unchanged live task state. The full batch uses
the3,872 preset, declares a50-step startup without reranking, and compares off,
one-root control,4roots*2futures,8*1,8*2 atinnerK32, plus4*2 atK64. All use
forecast depth8 and innerB4; strict1s/32GB/full2000. New options stay off until
full results establish a benefit.


## Preserve some priority across intermediate waypoints

The phase audit compares the3,872 trajectory with matched NMS32. In steps1–250,
ours finishes413 versus521 tasks despite2,196 versus2,181 waypoint events and
64,590 versus65,286 forward moves. Initial assignments have similar chain-length
distributions (both314two-stop tasks), and ours has slightly less internal hop
work. This is observational evidence of different progress concentration, not
proof that108tasks are recoverable. NMS locally sorts by remaining task distance
and squares the resulting rank weight on Random05; its algorithm differs in
many other ways. Read-only source: `nms/Solution/Objects/Environment/robot_handler.cpp`
and `nms/Solution/Planner/PIBT/pibts.cpp`.

A bounded generic test retains a fraction of accumulated age after an intermediate
waypoint, rather than resetting all age. `R05_WAYPOINT_AGE_RETAIN` defaults0,
preserving the previous behavior; .5 halves it and1 keeps it until whole-task
completion. Whole-task completion still resets age, and unopened reassignment
does not reset it. Retention keeps the existing inheritance and random-priority
search; it does not replace them with shortest-remaining-work ordering, which
the colleague found harmful. It might instead prioritize long chains too much.
Only full-run results will establish its effect.

Source71 age-retention regressions passed17.89s. They cover intermediate
waypoints versus whole-task reset, dense turnover and assignment locks, worker
determinism, snapshot restoration, and optional simulated aging. Five full
strict cases test retain0/.25/.5/.75/1 on the3,872 seed4 preset; retain0 is
the unchanged-behavior control.


## Trick: weight progress toward short remaining chains

NMS's local `PIBTS::update_score` weights distance gain by a robot weight derived
from remaining-task rank. Random05 squares that rank fraction. Our score instead
sums equal per-agent cost decreases. The phase audit motivates a separate test
of this evaluator difference, without replacing PIBT's age/offset priorities.
The colleague reported no gain from urgency/closeness weighting, so this is a
bounded recheck grounded in our measured completion/waypoint gap, not an assumed
new solution. Plain terminal completion bonuses already failed here.

`R05_SCORE_RANK_POWER` defaults0. At power>0, rank currently active agents by
their exact remaining oriented task-chain cost, give tied costs equal average
ranks, then raise the reverse-rank fraction to the power. Normalize active
weights to mean1 so dispersion retains its overall cost scale. The weights stay
fixed within all futures for this real step and are rebuilt after real matching.
This gives more value to movement on shorter remaining chains and may increase
long-order delays. Require --trick RANDOM-05 explicitly. The implementation
currently rejects turnover forecasts, closed-loop reranking or a completion
bonus together with these weights; those combinations need separate semantics.

Source72 regressions passed27.92s: tied-rank/order/scale invariants, competing
completion choices, dense task locks and turnover, worker determinism and
checkpoint replay. The actual entry was also checked to reject positive power
without the trick flag. Full2000-step powers0/.25/.5/1/2 are declared on the
3,872 preset, with retention/reranking disabled.


## Allocate workers inside more faithful forecasts

The first closed-loop forecast variants lose120/82tasks. Their downstream
policy is much cheaper than the real one:K32, one generation, one retained
vector. The live solver usesK16320/four generations/eight retained vectors.
The earlier counterfactual diagnostic usedK1280. Policy mismatch is a possible
explanation, not an established cause of the failed full runs.

`R05_REPLAN_THREADS` (default1) splits the declared total worker allocation
between whole forecasts and their internal candidate evaluations. Outer workers
are min(number_of_forecasts, total_workers/inner_workers); therefore simultaneously
active workers never exceed the declared total. Caches remain private per shadow
and per inner worker. `R05_REPLAN_POLICY=1` retains the live generation/elite/
persistence settings, clamped to the smaller inner portfolio; default0 preserves
the first prototype. No search work is skipped or time-limited partway through.
This makes it possible to test more faithful future search without assigning
most of the32workers no forecast work. PASSIVE OpenMP waiting and thread limit32
will be declared in the experiments to park idle teams. Full timing and throughput
comparisons are still required.


## Further averaging at the current staged-search budget

The high-budget baseline screens3,264 roots with2branches, retains one in4,
and finishes each retained root with14branches (K16,320 total,4generations).
Increasing K alone to24,000/28,800 previously reduced full throughput. More
candidate roots can amplify evaluator noise, so the next bounded experiment
increases the number of futures per retained root instead: B18/22/26, each
at approximately fixed work (K16,320/16,240/16,256) and at fixed3,264roots
(K19,584/22,848/26,112). Startup K is rounded down from8,000 to complete
generation groups; all exact counts are in branch-allocation-full-v69.json.

All six cases use the unchanged source233f5bf/build-v69, the3,872 seed4 preset,
strict1s/30s/32GB, full2,000steps, and the same declared guidance/horizon tricks.
No fresh held-out inputs are involved. This tests a search-budget allocation
hypothesis; neither extra averaging nor the greater work is assumed to help.


## Trick: restrict short-chain score preference to startup

Full rank-weighting tests reveal a phase tradeoff: power.5 completes471 tasks
in the first250steps versus413 for the3,872 baseline, but ends at3,845. Its
last250steps complete562 versus621; power1 gives487 initially and3,670 total.
These are different trajectories and task exposure histories, not an estimate
of the benefit of switching policies. The exact phase audit is under
results/rank-progress-phase-v72.

R05_SCORE_RANK_STEPS optionally limits rank weighting to the first declared
number of real solver steps. Default0 means unlimited, preserving source72.
After the boundary the solver returns to equal progress scoring; priorities,
work budget, task locks and RNG streams are unchanged. It inherits the explicit
--trick RANDOM-05 requirement from positive rank power. This tests whether an
initial preference for completing short chains can help without retaining that
bias as the visible pool changes. Full2,000-step results are required.


## Independent future samples for finalist selection

The current four-generation search selects thousands of candidate vectors
against the same14sampled futures. Increasing K alone reduced full throughput
in two tests. Overfitting those common future samples is one possible cause,
not an established diagnosis. Extra B18/22/26 tests examine more averaging
throughout search. A separate generic test instead spends a small extra budget
only on completed finalists after the ordinary search.

R05_RESCORE_ROOTS (default0) selects the existing best plus the next distinct
first decisions/promises by original full score. Each gets a complete, common
set of R05_RESCORE_BRANCHES futures (default64), drawn from an independent
per-step RNG stream. The constant-offset branch remains the shared reference.
This uses currently visible state only; it is unrelated to held-out task inputs.
R05_RESCORE_BLEND is the weight of the original score (default0, pure new
sample). Full work finishes for every selected finalist before selection.
One finalist, or blend1, must preserve the original trajectory and all persistent
state. Parent history keeps its original ordering with the chosen root first.
Concurrent component/closed-loop reranking is rejected to keep the comparison
interpretable. No change to default behavior or deadline handling is intended.


## Replicate the18-future record and sample additional planner seeds

The first fixed-work B18 run completes3,877, five above B14's3,872 on seed4,
with independent replay and strict timing. B22/B26 at fixed work lose. This
small selected gain is not a demonstrated mean improvement. Predeclare paired
B14/B18 runs on additional planner RNG seeds5/6/7/8, plus B18 on previously
measured seeds0/3. Inputs, horizon and all other settings stay frozen; these
are planner randomness comparisons, not new or held-out task/start inputs.
Retain all outcomes, update the selected best if justified, and report paired
means separately. The user's success criterion allows a selected planner seed.


## Follow up the3,917 startup-preference result

Source74 power.25 for250steps completes3,917, independently replayed, versus
3,872 for the unchanged control. Power.5for100/250/500 gives3,879/3,756/3,801;
power1for250 gives3,755. Stronger preference is not better. The chosen gain is
45tasks on one planner seed and is not yet replicated. Next declare10full
strict runs: powers.125/.375 for250; power.25for125/375/500; the selected
.25/250 with B18 instead ofB14; and its plannerseeds0/3/5/6. Baseline B14
seeds0/3 already exist and5/6 are running. All task/start inputs remain the
same development instance; no held-out input is consulted.


## Combine two measured directions and repeat generic rescoring

The independent-future batch is complete: controls3872exact; R4B32=3885,
R16B64=3906,R16B128=3659,R16B64blend.5=3819,R32B128=3867. All valid.
The best generic evaluator change adds34tasks to its exact3872control but
remains below the3917 startup-trick record. Next compare the source75 startup
control with3917, then combine its.25/250startup setting with R4B32 orR16B64.
Also repeat generic R16B64 without startup preference on plannerseeds0/3.
All five are full strict runs on the development input. No additive gain assumed.

The startup.25/125 original was refused before solver launch on research42
(expected16bound cores, observed64). Preserve that attempt and declare an
identical solver/configuration allocation repeat, excluding42 and34.


## Selected-seed objective and follow up3,928

B18 seed5 reaches3,928 while the seven paired seeds0/3/4/5/6/7/8 have a
slightly lower aggregate than B14. This is a selected-seed record, not an
average algorithmic improvement. The user explicitly permits selected seeds
to meet the target. Declare an additional fixed set of plannerseeds9–24 at
the unchanged B18configuration; retain all16outcomes, including failures, and
separate the selected maximum from the distribution. This is search over
planner randomness on the same development input, not held-out validation.

In parallel test source75's exact3928control, its startup.25/250 variant,
independent rescoring R4B32 orR16B64, and startup.25 plusR16B64, all at B18
andseed5. These five fullstrict runs test measured directions and their
interaction; the individual gains are not assumed additive. Fresh task/start
inputs will be generated only after freezing a selected qualifying candidate.


## GRID allocation refusal is separate from solver performance

The installed manual /opt/n1ge/man/man1/submit.1 (binding section, lines430–521)
confirms that binding is advisory and can be omitted when the requested cores
are unavailable. The installed m_topology_inuse complex is RESTRING and not
consumable. These facts explain why resource eligibility alone is insufficient
to certify a process allocation; the specific causes of our three refusals are
not proved. Keep the existing actual-affinity/quota guard, which refuses any
incorrect allocation before launching the solver. Retry in a separate named
case with unchanged solver settings and preserve each original refusal.
No scheduler configuration or other user's allocation is modified.


## Trick: new guidance patterns at the current planning budget

Earlier layout-seed searches through48 used much smaller planning budgets.
The current-budget local retunes of field15 all lost, which rules out those
settings but not other layouts. Predeclare eight new field seeds49–56 with
plannerseed5/B18 and every other3928setting fixed, including the existing
one-edge flip from flipseed5. This isolates generated field choice within the
selected configuration. All use explicit --trick RANDOM-05, full2000steps,
strict1s/30s/32GB, and the same development input. No gain is assumed and
all eight outcomes will be retained. This is a map-specific layout search,
separate from generic planner changes and planner RNG-seed selection.


## Separate static-continuation weight from sample count

Independent rescoring R16B64 improves all three checked planner seeds0/3/4:
3805/3893/3906 versus3778/3845/3872 (+109total, +0.948%). These are planner
seeds on one input, not independent task/start validation. B128 nonetheless
loses strongly on seed4. Variance alone is not the only difference: the current
mean gives the constant-priority branch weight1/B, so increasing sample count
also reduces that branch's influence. Whether this explains the loss is unknown.

R05_RESCORE_STATIC_WEIGHT (default-1, preserve the legacy mean exactly) optionally
sets the constant-priority branch's weight explicitly, with remaining mass
spread over the random-future mean. This separates mixture choice from sample
count. Every declared branch is still evaluated, including at weights0 or1.
The ordinary search objective stays unchanged; this affects only finalist
rescoring. Nonzero variance coefficients are rejected with explicit weights
until weighted variance semantics are specified. No map-specific rule is added.


## Fixed mixture for finalist forecast scoring (2026-09-21 00:56 UTC)

Build-v76 passes its regression suite (31.82 s; binary SHA256 `5332e1acf082031df3d6a56baadf526e15541427836b86cabb9b1bd65c330213`). The predeclared eight-case full strict batch fixes the R16B64/seed4 reference and tests constant-priority forecast weights -1 (legacy), 0, 0.05, 0.1 and 0.25, plus B128 at weights 1/64, 0.05 and 0.1. The legacy control must reproduce 3,906. Increasing samples previously also diluted the unchanged-priority branch; whether that explains any performance loss is unknown. This experiment separates those changes. No task/start validation inputs are used.


## Future mutation after four-generation refinement (2026-09-21T01:01:42.252684+00:00)

The old future-mutation check preceded the current G4/E8/P8 staged search. A selected root can now descend through several mutations, while simulated futures still reset 30% of priorities per later step. That is a plausible forecast mismatch, not a measured cause. The bounded new batch holds the main search fixed: B18/seed5 tests future mutation0.1/0.2/0.5/0.8 against the repeated3,928 control; B14/seed4 with R16B64 rescoring tests0.5/0.8 against3,906. The changed rate affects both ordinary futures and independent rescoring where enabled. All are full strict runs. Earlier failed rates and all new outcomes remain archived.


## Optional routing preference inside priority search

The current population varies priority vectors, while every robot ranks its
moves using a fixed guidance cost. The new general `R05_MOVE_BIAS` experiment
(default0, range0–4) lets a quarter of agents prefer one sampled compass direction
by a small cost offset. A deterministic hash of the candidate's existing priority
vector supplies the preference, so retained candidates retain their routing
proposal. This couples the two proposal mechanisms; it is not an independent
route optimizer. Spatial intent and executable PIBT see the same perturbed order.

Candidate rollout scores, true goal costs, cycle gains and collision checks
remain unchanged. Only local copies of cached rankings are perturbed; kinematic
masks are recomputed after reordering. No map coordinates, task-stream knowledge
or horizon dependence are introduced. The operation-based prototype rejects the
option because it has a different action-ranking implementation. Dense checks
must establish that the feature actually changes decisions while preserving
cache, worker and checkpoint equivalence. Full zero-bias control and throughput
runs follow only after the regression suite passes.


Late-assignment diagnostic on the3,928 trace (`results/late-assignment-diagnosis-v76/audit.json`): atsteps1250/1500 every unlocked robot already gets a hop-estimated feasible task. Maximum-cardinality feasible matching adds only3/4/2 slots at1750/1850/1900, and zero at1950. Those counts use visible tasks and avoid counting one short task as multiple opportunities; they are not predicted throughput gains. This does not support making scheduler feasibility the main next change. The B18/seed5 R16B64 allocation repeat completes at3,704, so rescoring gains measured at B14 do not transfer to that selected B18 record.

Build-v77 passes all regression tests (21.37 s). Compiled inputs match the workspace byte for byte. The declared full strict comparison uses B18/seed5 at move biases0/0.125/0.25/0.5/1/2, with zero required to reproduce3,928. Binary SHA256 `ca5840674488735bcd31dbed3416f309e9928ce2954a1c1fcfe70308dbea3e5c`.


The first bias0.5 attempt finishes step0 computation in1,066.438ms and correctly exits124. Allocation passed16-core/no-quota checks onresearch53. The cause of the slow step is unproved; other amplitudes pass step0. Preserve this failure. Two separately declared attempts use identical source, seed and fixed search work: a strict repeat and a5s diagnostic. Only a complete strict run can update the frontier.


Move bias2 reaches a replay-verified3,933 on B18/seed5, versus3,928 with bias0 (exact control). Bias0.125/0.25/1 gives3,885/3,757/3,905; the original0.5 attempt times out atstep0 and declared follow-ups remain separate. A five-task selected gain is not a replicated improvement. The bounded follow-up checks bias2 on planner seeds0/3/4 against their existing B18 controls, and biases3/4 onseed5. Other search settings remain frozen. Full strict runs only; no new task/start inputs are used.

### Move-proposal results, 2026-09-21 01:28 UTC

The zero-bias control gives3928; biases0.125/0.25/1/2 give3885/3757/3905/3933. Bias2 is a new selected maximum, five tasks above3928, with independent replay and strict timing passed. It does not yet establish a seed-averaged gain. The original bias0.5 case failed atstep0 (1066ms); its declared strict repeat and5s diagnostic both finish3745. The original failure remains in the evidence; its cause is unproved.

### Frozen transfer completed, 2026-09-21 01:52 UTC

Generic no-tricks Random01..05:647/1079/1582/1558/2226. Frozen3928recipe:592/1122/2171/2456/3928; the Random05control is identical in all six fields. Matched shipped NMS01..04:649/1228/2359/2580. All14new successes independently replay and pass1s/32GB. Five large pipeline representations analytically need95–189GB; user chose report-only, no port. Five new NMS large attempts fail30s preprocessing (12.1–16.1GB observed RSS), not demonstrated RAM failures. Generalization is limited in these frozen profiles; one input per density, no density-specific tuning, identical819-cell geometry. See GENERALIZATION.md and linked audit.

### Move-proposal refinement declared, 2026-09-21 01:55 UTC

Bias3/seed5 scores3941 (strict full run, mean514/max590ms, independent replay passed), eight above3933. Bias4 gives3894. Bias2 seed0 gives3798 (+132 against its3666 baseline); seed4 gives3785 (-92 against3877); seed3 remains running. The selected maximum does not establish a mean gain. Next freeze six full comparisons on source77: bias3 on seeds0/3/4; amplitudes2.5/3.5 onseed5; and bias3/seed5 withK24480 but identical first7968. The larger budget tests whether broader move proposals benefit from more roots within remaining runtime; earlier larger-budget tests without this feature were negative, so no gain is assumed. All runs remain strict1s/32GB; startup/rescoring off. No generalization outputs are used for tuning.

Bias2 follow-up complete: seeds0/3/4/5 score3798/3909/3785/3933 against3666/3675/3877/3928 (+132/+234/-92/+5). Aggregate15425 versus15146 (+1.842%), three of four positive. Seed5 selected the setting; the other three are planner seeds on this same development input, not fresh task/start validation. Inputs, search settings and16physical/32worker allocations match. Evidence: results/move-bias-followup-split-full-v77/paired-bias2-outcomes.json.

## Fraction of robots receiving routing proposals

2026-09-21: the source77 proposal bias uses one quarter of robots in each root, a fixed design choice. Source79 adds `R05_MOVE_BIAS_FRACTION` in[0,1], default0.25, without changing the direction hash or adding random draws. Activation uses hash bits separate from direction bits; the default reproduces the original low-two-bit gate exactly. Fraction0 bypasses the proposal bias, and fraction1 exposes every robot to a preferred direction (when that neighbor exists). This is a general search option, not map geometry tuning. Actual cost/score and legality checks remain unchanged; only each rollout's local candidate ranking changes. Cached rankings stay unbiased. Dense regression cases check zero-fraction identity and cache/worker/checkpoint equivalence at1/8,1/2 and1. A full default-fraction control must reproduce3941 before interpreting experiments.

Source79 regression passes34.04s. Build SHA256 `5338e02c5cfa2b42dbb7bdda4a5b1c6dd52a6dfe483d614d8f3dddbb96accee6`; all compiled source and test files match the working tree. Freeze six strict full cases: bias3 at fractions0.25(control),0.125,0.5,1; bias2 at fractions0.5,1. Same B18/seed5 settings and first7968/K16320. The quarter control must reproduce3941 exactly. No elapsed-time stopping, new task/start inputs, or map retuning.

The paired phase audit sums bias0 versus bias2 over plannerseeds0/3/4/5. At500/1000/1500/2000 steps the gains are+27/+4/+148/+279 tasks. Thus131 of the279 aggregate gain accrues in the final500 steps; gains are not confined to that final interval. This does not separate horizon interactions from routing improvements, because trajectories and task sets diverge. Onseed5, bias3 is+42 at500, -48 at1000, -27 at1500 and+13 at2000, another warning against ranking variants from short prefixes. Evidence: results/move-bias-phase-v77/.

## Expanded RANDOM campaign

2026-09-21T02:17:27.230561+00:00: user requests general improvements and explicit tricks for all five RANDOM cases, while preservingRandom05 4k priority. The first23 cases are frozen in experiments/random-density-first-full-v79.json and described in RANDOM_PROGRESS.md. General noise/dispersion choices are applied across all five cases; per-instance choices are separately flagged tricks. No large-map port is planned.

### Further declared comparisons, 2026-09-21 02:23 UTC

Source77 bias3 reaches3978 on plannerseed0; seed3/4 give3839/3782 andseed5 gives3941. Relative to matched bias0 baselines3666/3675/3877/3928, total15540 versus15146 (+2.601%,3/4positive). A bounded follow-up tests plannerseeds6/7/8/24, with existing bias0 references3832/3851/3876/3904. Seed24 was the best earlier expanded-seed result; this is explicitly selected development search, not independent validation. No fresh task/start inputs are generated.

For RANDOM01–03, test the existing three-action operation policy at fixedK128 andK512, four generations/eight elites, no continuation branches. Use64 revisits and protect successful inherited chains, the stronger old operation variant. This failed at RANDOM05 but removes the pipelined policy's turn/move restrictions, so it is a bounded architecture check at lower density before a new windowed optimizer. No guidance or horizon trick; all32workers/1s/32GB and full horizons. Source79; other baseline settings remain fixed.


## General windowed-search prototype (source80, September21)

Lower-density frozen transfer and the operation-policy trial leave a substantial
RANDOM-02/03 gap. Add an optional `R05_WINDOW` mode, disabled by default. It uses
our existing matching scheduler and exact oriented task-chain costs, compares a
fresh pipelined seed with a short retained prefix plus regenerated tail, and runs
independent deterministic LNS islands. Each repair removes a small nearby group
and plans it with time-space A* against the remaining vertex and reverse-edge
reservations. All islands perform their declared number of iterations. Failed
bounded repairs restore the complete incumbent; elapsed time never truncates the
search, and the existing strict entry deadline still raises a failure.

This is an independent implementation of the windowed/LNS pattern inspected in
the archived Kitty Knight WPPL planner, including its parallel local optimizer.
No participant source is copied. There is no map name or geometry-specific rule
in the planner. Instance-specific selection or guidance still requires the
matching `--trick` flag. Full runs, independent replay and repeated comparisons
will decide whether this mode is useful; it is not yet a performance claim.

Settings: `R05_WINDOW` (0/off, otherwise at most32), `R05_WINDOW_KEEP` (6),
`R05_WINDOW_ISLANDS` (32), `R05_WINDOW_ITERS` (24),
`R05_WINDOW_NEIGHBORHOOD` (8), `R05_WINDOW_EXPANSIONS` (20,000 per A* repair).
Actual threads use the existing `R05_THREADS`. Island RNG streams depend only on
planner seed, step and island index. Checkpoints retain the complete prior window.
Tests cover repeated task stops, immediate legal motion, dense task turnover,
worker/cache equivalence, checkpoint replay and restoration after failed repairs.


## Windowed repair neighborhoods (source81, pending evaluation)

Optional `R05_WINDOW_BLOCKERS=1` follows each selected robot's unconstrained
oriented task-chain route, collecting the robots whose actual reservations block
it, recursively up to the neighborhood limit. Nearby robots fill unused slots.
This directly targets interactions instead of choosing all neighbors by distance
at one sampled time. `R05_WINDOW_EQUAL=1` separately permits changed full paths
with identical total and terminal costs, allowing exploration on score plateaus.
Both are disabled by default. All full-plan reservation and collision checks,
fixed work counts, and strict entry deadlines remain in force. Dense turnover,
worker/cache equivalence and checkpoint restoration tests cover the new modes.
No map-specific rule or future task is used.


## Windowed initialization portfolio (source82, pending evaluation)

The first windowed mode compared one deterministic pipeline rollout with a
retained-prefix rollout. Add `R05_WINDOW_STARTS` (default1) to compare a fixed
portfolio of complete pipeline starting plans before LNS. Extra starts use
independent step/seed/index RNG streams and the existing `R05_NOISE` amplitude.
The best complete plan seeds every LNS island; retain
its priorities for the next step. All starts finish before optimization. This
implements the colleague log's best-of-rollouts direction without using its
unavailable code or tuned fields. The one-start default preserves the prior
trajectory. New dense worker/cache/checkpoint tests exercise four starts with
blocker groups and equal-cost exploration. Full controls precede promotion.


The initial source82 proposal failed the existing dense-mobility floor with four
starts plus blocker/equal repairs; do not benchmark or promote that build.
Source83 revises initialization ranking to summed remaining task cost across the
window. This tests whether progress-based ranking avoids preferring a stationary
seed under cumulative travel-cost scoring. The LNS objective is unchanged, and
the one-start control retains its original ranking. The same mobility regression
and threshold remain in place; diagnostic output now identifies a failed setting.


## Retained-prefix initialization (source84/85 experiment)

Source83 extra fresh starts lost throughput, especially guided RANDOM-03. A fresh
pipeline seed begins with no pending forward motion. Source84 tested retaining
the short prefix in every candidate and comparing seeds by the LNS path-cost
objective; it failed the existing dense mobility regression (39 moves, threshold
>150). The failed patch and build are archived, with no benchmark or threshold
change. Source85 instead retains source83's progress ranking and varies only
the tail after the committed prefix. This isolates whether repeated fresh starts
caused the performance loss. Default `R05_WINDOW_RETAIN=0` preserves source83.
The original and new failed-repair, worker/cache and checkpoint checks remain.

Source85 also failed the unchanged mobility floor (38 moves); the original
fresh-start and failed-repair controls passed. Both proposed retained-prefix
variants are rejected, and source is restored to f217eee. Their patches, exact
builds, and regression logs remain in results/build-v84 and build-v85. No full
benchmark used either failed build. Retaining the prefix by itself is insufficient
to cure this crowded-state failure.

## RANDOM-03/04 ten-percent campaign: fixed-work scaling

The user now targets at least10% above matched NMS on both densities and
robust subsecond runtime. Raising the explicit window iteration ceiling from
2048 to8192 allows measured work scaling on RANDOM-03, whose selected run
averages275ms. The default count is unchanged, and the optimizer still completes
every declared attempt; the strict deadline fails any overrun. Source86 makes
no search-policy change. Test the full regression suite and unchanged2048
control alongside4096, with qualification across seeds before promotion.
The first mixed-source batch separately tests eight routing/scheduling changes
and four RANDOM-04 runtime controls. See RANDOM34_CAMPAIGN.md for criteria.

## Exact ranking-cache read fusion (source87)

RANDOM-04's sampled policy time at depth16/K11520 is about29% setup,19%
candidate ranking,7% ordering,28% spatial/kinematic PIBT,7% cycles and9% actions.
These are sampled worker elapsed times, not a causal speedup prediction.
Optional R05_FUSE_CACHE_HITS copies cached candidates in the first lookup pass
instead of reopening the entry in the later candidate pass. This preserves the
rank order and kinematic mask exactly; dynamic push costs still bypass caching.
The default is off. Dense task-turnover, cache-eviction, worker and checkpoint
regressions compare it with the uncached policy, including the dynamic bypass.
Full RANDOM-04 same-budget trajectories must match before any timing claim.

## Cooperative window repairs and exact search-storage reuse (source88)

The current32 LNS islands independently improve the same seed until the end of
the step. Optional fixed sharing rounds let all workers refine the best complete
plan after each round. This tests whether cooperative refinement beats independent
diversity at the same declared total repair count. Each round is a deterministic
barrier; its best plan cannot worsen the previous complete incumbent. Seeds use
step/island/round, with round0 preserving the old seed stream. One round remains
the default. Window-search storage reuse is a separate exact option, using the
existing per-search epoch tags and private worker storage to avoid repeatedly
allocating/clearing the state tables. Defaults preserve the old implementation.
Dense turnover, worker count, cache choice, full checkpoint replay and mobility
tests cover both. Neither feature is enabled based on map geometry.

## Compact aligned ranking cache (source89)

The previous CachedRanking entry occupies104 bytes: its40-byte header can
cross a cache-line boundary, and an entry can span three64-byte lines. Its
heading (0–3), candidate count (1–5), and five-bit kinematic mask fit in bytes.
Store those fields compactly, align the candidate block to32 bytes, and make
the whole entry96 bytes. Header lookup then stays in one line and the complete
entry in two, while capacity in entries and all scores/keys remain unchanged.
Compile-time layout checks enforce that representation. Existing uncached/cache,
eviction, worker, dynamic-cost bypass, checkpoint and dense turnover tests are
the semantic controls. No throughput or speed claim precedes full equivalence
and measured timing comparisons. This is a general representation optimization.

## One-line cached rankings (source90 experiment)

Graph construction already rejects more than4096 free cells. Cached candidate
destinations therefore fit16bits and headings8bits without changing score bits.
Store a candidate in8bytes instead of12, and each five-candidate ranking plus
its24-byte header in one aligned64-byte line (source89 used96bytes). Keep the
original64-bit hash/address sequence while compressing the stored pose/stage
key to32bits. Very long stages above65535 use the ordinary uncached calculation;
epoch values above32bits also bypass caching, so truncation cannot cause false
hits. No score/order/priority/work count changes. Existing dense uncached/cache,
eviction, fused-read, checkpoint and worker controls test exact policy behavior.
Full frozen controls and repeated timing measurements remain necessary.

## Fixed-work annealed window repair (source91 experiment)

Cooperative LNS still accepts only nonworsening repairs, which can trap the
small groups around a local optimum. NMS's local push search also explores
worse scores via simulated annealing (read-only pibts.cpp). Test the general
idea in our own window optimizer: optional R05_WINDOW_TEMPERATURE allows a
complete legal repair with probability exp(-cost_increase/temperature), cooling
linearly by fixed iteration count. Each island separately preserves its best
complete incumbent and returns that, so neither an incomplete repair nor a
worse walk endpoint replaces the incumbent. Temperature0 is the exact default.
Full-plan validation and no-worsening checks remain. Dense turnover/mobility,
worker count, cached/uncached equivalence and checkpoint replay exercise the
new path before full performance experiments. It is not a map-specific rule.

## Shared immutable task/pose rankings (source92 experiment)

Each worker currently caches the same candidate rankings independently, and
real steps invalidate them even while a task's cost field stays fixed. Optional
R05_SHARED_RANKINGS_MB instead builds immutable rankings for an assigned task's
(stagedgoal,cell,heading,moving) states, shares them across workers, and retains
them until the task leaves the visible pool. Construction runs in parallel
before search; every entry uses the existing score arithmetic and stable order.
A fixed MiB cap and stable agent ordering select tables that fit; others retain
the exact existing calculation. Candidate scores, priorities, movement bias,
collision resolution, futures and deadlines remain unchanged.

This prototype bypasses dynamic push prices, virtual task matching, nested
forecasts and operation planning. Wait/intent configuration changes invalidate
tables. Tables belong to Chain objects and checkpoint restore rebuilds them.
Dense7x7 regression comparisons cover cost-cache off, prospective wait, disabled
intent rotation, dynamic bypass, move-bias proposals, partial/full memory caps,
worker counts, task turnover and full checkpoint replay. The32GB process guard
remains; source92 has no speed claim before regression and full paired controls.

## Explicit startup LNS budget (source93 experiment)

All four larger-window R03 attempts time out on the first call: H24/I8192
uses1055/1048ms, H28/I6144uses1216ms, H32/I6144uses1578ms. Initial search
expands180/204/281million states, before any retained plan exists. Preserve
these failures. Optional R05_WINDOW_FIRST_ITERS declares a smaller fixed count
for timestep0, analogous to the reactive planner's existing FIRST_K. Zero
(default) retains the regular count. Positive counts must fit the regular budget
and divide evenly across sharing rounds. All attempts still complete, and
any later overrun still fails. Worker/checkpoint/dense-turnover tests exercise
the smaller first budget without weakening timing or movement checks.

## Skip unused spatial neighborhood ranking (source94 experiment)

When recursive reservation blockers fill the whole LNS group, the previous
implementation still generates400 distance keys and partially sorts them, then
overwrites the entire selected prefix with blockers. Optional
R05_WINDOW_FAST_GROUPS builds the blocker group first. If it fills, discard
exactly the same N mt19937 draws and skip the unused distance/sort work; if not,
run the original key generation, partial sort and fallback ordering unchanged.
Reusable membership epochs avoid per-repair allocation on this path. A counter
reports skipped sorts. Groups, subsequent draws, paths and work counts must
match the old implementation. Default remains off. Dense worker/cost-cache/
checkpoint regressions and full exact controls precede timing claims.

Raise only the explicit WINDOW_ITERS ceiling8192->16384 for subsequent scaling
if the optimization creates enough headroom. Default work is unchanged, every
configured repair finishes, and the strict deadline still rejects overruns.
