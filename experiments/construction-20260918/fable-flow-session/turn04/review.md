## Corrections absorbed

- **Tied seed turns.** They are 0.62% of turns in the collapsed window. My "turn inflation is largely a tie artifact" reading is refuted in magnitude. Planned rotations dominate (2.03M of 2.65M), so the churn comes from the planner's own successive plans.
- **Cache flushes.** Cache-only adds 0.5–0.8% on seed 0, while real cost updates add 9–10%. Flushes are not the mechanism there. You are right that equality would only have impugned the refreshed costs, not learned flow as a whole.
- **Still open.** Whether the gain comes from a better field or from non-stationarity is undecided. Refresh256 is slightly worse than refresh512 while flushes are mildly positive, which is weak evidence against non-stationarity as the driver. That is an inference.

## 1. Const audit

1. **It classifies only the single best alternative.**
   - If that alternative touches three robots while the next strictly-better one is free, the root is still filed under `many`.
   - For the branching decision you need the easiest class over all strictly-better alternatives. Two-branching is necessary only where no strictly-better alternative has one movable blocker or fewer, and some alternative has exactly two.

2. **There is no gain threshold.**
   - "Strictly lower cost" includes differences in the `op` tie term alone.
   - Report gains of at least one full step (`distance_scale × unit`) separately, or weight the counts by gain.

3. **The stride aliases with the refresh interval.**
   - Audits fire at timestep ≡ 63 (mod 64), and publications fall at 128+512k.
   - Each flush is therefore sampled only at offsets −1, +63, +127 and so on, and the post-flush fallback transient is never seen.
   - Use a stride coprime to 512, for example 67.

4. **`unblocked` is the class most relevant to Question 3 [code fact].**
   - A free, strictly-better alternative would always be accepted if it were tried.
   - It stays unused for one of three reasons:
     - the root was never re-attempted;
     - the `horizon_` depth filter hid it;
     - the attempt aborted when a better-ranked blocked candidate's chain was rejected, because `return 2` ends the attempt instead of trying the next candidate.
   - That makes `unblocked` direct evidence of unfinished local search under fixed work.

## 2. Gap accounting and two changes

**Gap accounting.** Cycle time is 371.7 robot-steps per task against KK's 326.8, a gap of about 45. From your numbers it splits as follows:

| Component | Steps/task | Source |
|---|---|---|
| Loaded motion overhead | about 23 | ratio 1.1746 vs 1.0799 on 242 cells |
| Chain length | about 9 | 242.32 vs 233.84 |
| Empty travel | about 11 | 76.6 vs 65.7 |

Part of the empty figure is motion overhead and not pickup distance. Roughly a third to a half of the gap is therefore on the assignment side, with your cohort caveat acknowledged.

**A. Throughput-oriented dispatch with the existing `CGAR_HRRN=0`**, if you have not already run it under refresh.

- **Code fact.** The score is 1 + age/cost, and cost only breaks ties.
- **Mechanism.** With pool ages in the hundreds and costs of 200–400, age dominates among the at most 16 nearest candidates. The scheduler serves old tasks, while the competition counts tasks completed.
- **Inference.** Permanently leaving the roughly 5,000 longest tasks in the pool trims the completed-chain mean by about 3% for a typical spread. That is about the size of KK's chain gap.
- **Cheap falsifier first.** Add a hash-neutral dispatch-regret counter: chosen cost minus the cheapest cost among that robot's candidates. If the mean is under about 5 steps per task, drop the idea.
- **Full-run falsifier.** On refresh512, I predict the completed-chain mean moves toward 234, empty steps per task fall, and tasks rise by at least 2%. A rise under 1% falsifies it.
- **Expected failure signature.** The pool clogs: stale long tasks saturate the 16-candidate limit, and the gain decays window by window. The follow-up would be a graded age weight, not HRRN back on.
- **Starvation.** Fair admission, about one per 10,000 admissions, still bounds it.
- **Bias risk.** Chain costs mix BFS legs and Manhattan legs when `sched_tables_` runs out. Under cost-ranked dispatch, the under-estimate bias you raised applies here too. Check the share of Manhattan legs, or enable chain-cost refinement.

**B. Late binding of the empty leg.**

- **Mechanism.** About 20% of robot-steps are empty, so about 2,000 robots are en route at any time.
  - About 28 tasks are revealed per step, so about 2,000 new tasks appear during one 76-step approach.
  - A committed robot ignores all of them.
- **Code fact.** `reassign_unopened` only swaps between assigned robots, once per task. It never looks at the free pool.
- **Change.** Let an en-route robot exchange its unopened task for a free one when (d′ + chain′) beats (d_remaining + chain) by a margin. Compute all distances on one basis, from the robot's own bounded BFS.
- **Precondition.** Your harness must permit releasing unopened tasks.
- **Offline falsifier, current information only.** Replay a saved run and accumulate the greedy saving. If it is under about 3 steps per task, drop the idea.

**Weighted scheduler scoring.**
- I withdraw the cached-table form. Mixed bases favor candidates that lack tables, as you say. Candidates also come from a robot-centered unit BFS, not from goal tables.
- The same-basis form is a bounded forward Dial search over (cell, heading) using the published costs, inside `candidates()`. Staleness at a publication step is one step.
- It is second-order to A and B.

**Dwell-time tolls.** By my own criterion, your concentration numbers are borderline, so I would shelve them.

## 3. More fixed search work

- **Verdict.** It is credible, but I expect less from it than from A.
- **Headroom is real.** Mean decision time is about 250 ms, and the run uses 1.2 of 4 cores on average.
- **What more work can convert.** Extra single-chain work can convert only the audit classes `unblocked` and `one_movable` with at least a one-step gain. It cannot touch `two_movable` or `many_movable`.
- **Cheaper alternative.** If `unblocked` is material, run a deterministic post-repair sweep over stationary roots that takes free, strictly-better candidates. It is score-monotone and O(n), it sidesteps the abort rule, and it costs far less than 4× the work.
- **Prediction.** I expect your queued 4×4M run to gain at most about 1.5%. A gain of 3% or more would reverse this ordering.
- **What remains unsettled.** Best-of-workers maximizes the 5-step objective, not throughput. The +0.55% portfolio figure came from no-flow 50k runs. Neither settles the question.
