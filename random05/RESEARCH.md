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
