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
