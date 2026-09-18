# Proposed CGAR implementation sequence

This records the proposed sequence. Items 1–2 are now implemented and evaluated in the [strict-deadline study](../throughput-20260918-strict/README.md); later items remain proposals. The first target is to recover the draft's throughput while retaining the migration's correctness fixes. Use one policy across maps: no map-name branches, special coordinates, hand-built lanes, or per-map weights.

The [diagnostic experiments](../throughput-20260917/README.md), [Fable review](review.md), and [checked assessment](assessment.md) support repairing dispatch first. In the original full sequential runs, SORTATION fell from 62,703 to 30,922 tasks and WAREHOUSE from 37,812 to 26,350. Those regressions precede the ten-way parallel evaluation. Replacing the scheduler with the draft control while retaining the corrected motion backend raised SORTATION from 4,704 to 14,337 tasks over 1,000 steps. That control retains draft scheduling shortcomings and also changes cache accesses; it is diagnostic, not a production rollback.

## 1. Make the effects measurable

Add low-overhead counters for local, fallback, and explicit fair assignments; deadline, node-limit, and task-cap exits; redundant search work; and sampled evaluations. Measure pickup distance and delay, completed-task windows, outstanding task ages, robots targeting each pickup, and recovery lengths. Record CPU quota and observed CPU availability with the executable, settings, and seed.

Use the same instrumented executable for paired policy variants. Instrumentation and wall-clock variation can themselves change deadline outcomes, as the earlier experiments showed.

## 2. Repair fallback quality and remove redundant searches

Compare a fixed-size global sample of unused eligible tasks by pickup plus remaining-chain cost, keeping the oldest task as a candidate and retaining the independent fair-admission rule. Preserve the separate PIBT random stream. Complete the prescribed sample or raise an explicit timeout. Per the user's clarification, elapsed time must not silently change the algorithm's work or return a partial result. Scheduling and planning share the actual entry deadline.

Skip the second search when the first found no candidate and the search bounds are unchanged. If candidates were found but taken by other robots, replenishment is still necessary. Do not treat a bounded search failure as proof that no eligible task exists globally.

Validate uniqueness, eligibility, parked-robot exclusions, primary/ticket semantics, fairness, and short-budget behavior. Preserve normal-budget assignment coverage. The WAREHOUSE prototype's 5,696 to 8,464 tasks over 1,000 steps is encouraging; sustained full-horizon gains remain unverified. The SORTATION sampling comparison also crosses instrumentation versions and changes time allocation, so it is not a pure one-flag estimate.

## 3. Share candidate discovery and make deferred work progress

Replace repeated fixed-radius searches with shared graph searches from eligible task endpoints, producing nearby robot-task pairs. Replenish unmatched robots with additional candidates; account for task availability at each endpoint. Evaluate this independently from changing the scoring objective.

Keep each scheduling decision complete under its prescribed work policy, including assignment coverage and fair admission. A deadline miss must fail explicitly. Time-driven deferral and partial returns from the original proposal are not adopted. Validate sparse endpoints beyond the old search limit, arriving tasks, and unique eligible coverage. Any future incremental algorithm needs a separately justified progress rule and an explicit change to this contract.

This addresses Fable's checked observation that many later fallback assignments result from the 2,048-cell cap, not deadline exhaustion. Additional CPU alone does not remove that cap.

## 4. Test the objective separately

First ablate HRRN without widening candidates or changing other policies. Then compare whole-chain cost with a globally weighted pickup-distance-plus-chain objective. Keep explicit fair admission and measure task-age tails and unfinished old tasks, so shorter jobs do not silently displace difficult jobs indefinitely.

The NMS pickup weight is an experiment to test, not a value to copy unquestioningly. Use the same weight on every map. Broader matching under the current objective regressed holdouts; those observations do not isolate HRRN from distance-estimate inconsistency, matching changes, cache accesses, and time allocation.

## 5. Make distance estimates refinable and protect routing work

Track whether scalar leg and chain costs are exact or approximate, upgrade estimates when exact information becomes available, and invalidate them at the correct remaining-stop epoch. Retain useful exact scalar costs without requiring every full distance table to stay resident. Avoid routing-cache recency changes from broad scheduler inspection and retain priority for the persistent primary's exact table.

Keep stall tracking on a consistent distance basis. When the basis changes from Manhattan to exact distance, reset or translate the progress minimum so the information upgrade does not trigger a false stall. Routing fallback counters do not measure the accuracy of cached task-chain estimates; instrument these separately.

## 6. Repair poor allocations through bounded reassignment

Permit reassignment only before the first pickup. Exclude the persistent primary and robots locked by active recovery. Require a meaningful verified improvement, cap churn per task, preserve eligibility and unique ownership, and issue consistent task tickets.

Both selected leader schedulers use unopened-task reassignment. Its benefit in CGAR is unmeasured. Test it only after initial task selection is improved, so its effect can be attributed and it does not become a substitute for good allocation.

## 7. Reduce recovery latency while retaining complete witnesses

Reject known opposing move commitments before the final collision guard. Separately, derive robot and cell dependencies from a complete validated recovery witness, and test concurrent execution of independent moves. Replay the resulting actions through independent validation, including rotations and reservations.

The GAME log contains an approximately 2,000-move transaction involving 66 robots and holding the single transaction slot for over 1,000 steps. This supports a latency concern but does not isolate a throughput penalty. Do not abort unresolved transactions or discard witness completeness for a higher task count.

## 8. Improve generic routing after dispatch

First test observed congestion as a preference among moves of equal progress potential. Then test bounded orientation-aware lookahead using uniform turning costs and the existing action/reservation checks. Keep traffic preferences separate from the static potential used for progress tracking.

A default-planner substitution can help size a routing gap as a diagnostic control; it does not preserve CGAR's recovery properties and is not a production replacement. The active leader snapshots also use map-specific routing inputs outside this proposal's scope.

## Validation and promotion

Start with items 1 and 2, in separate reviewable changes. Keep subsequent mechanisms separately switchable during evaluation and do not combine all of Fable's first-patch ideas into one result.

- Run the existing correctness regressions and meaningful new cases for the changed behavior.
- Evaluate all ten archived horizons with the same production limits, executable configuration, and seed before claiming a suite-wide gain. Preserve and report RANDOM-05's existing restricted capacity mode.
- Use matched controls on reserved physical cores with CPU availability recorded; independent instances can run concurrently. Repeat promising comparisons under comparable resource conditions, then test multiple seeds; at least three timing repeats and three seeds for the final candidate is the intended validation target.
- Report completions per 1,000-step window, pickup and delivery delays, outstanding task ages, fallback reasons, and recovery activity alongside total tasks and errors.
- Keep fixed count limits identical across matched policy variants and fail explicitly on wall-clock expiry. Final competition-style evaluation must retain the actual wall-clock limits.
- Recover the draft's full-horizon throughput before claiming progress toward the leaders. Benchmark KittyKnight and NMS under comparable resources afterward; the current experiments do not establish the attainable gap without their map-specific inputs.

Finite tests and zero simulator errors do not prove starvation freedom. Preserve persistent primaries, task/stop tickets, eligibility filtering, parked-robot exclusions, fair admission, and complete validated recovery witnesses throughout.
