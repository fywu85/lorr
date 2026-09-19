I read the v40 source and ran nothing. I found no reproducible counterexample. The `select_primary` concern is withdrawn given your correction.

## 1. v40 correctness

**Invariants that hold by construction:**
- **Pointer lifetime.**
  - Batch admission finishes before any temporal code takes table pointers.
  - `tables_` is node-based, so a rehash never moves elements.
  - Nothing evicts mid-step, because `table()` never trims and `retain`/`trim` run only at the start of `plan()`.
- **Serial/parallel equivalence.**
  - `compute()` is const and reads only immutable state.
  - Each worker owns its own scratch buffers.
  - Results are admitted in rank order, and the rank is a total order (count, ticket, goal id). LRU state is therefore identical for any thread count.
  - The hash-ordered `missing` map is sorted before use.
- **Exceptions.**
  - `prefetch` joins all workers on every path.
  - It stores results only if no worker failed.
  - A Timeout leaves `prefetched_` empty, and the entry fails.
- **Protection and invalidation.**
  - The pass only adds tables.
  - Publication happens earlier in `plan()`, so the metric cannot change mid-batch.
  - Work is fixed by counters and never by the clock.

**Source facts worth acting on:**
1. **Transient capacity overshoot.**
   - `table()` inserts without any capacity check.
   - The cache can therefore exceed `max_bytes_` by up to batch + demand builds until the next `plan()`.
   - This is safe for lifetimes and costs only a few hundred MB of memory.
   - If capacity ever fell below the number of live goals, each step would build tables, evict them, and rebuild them. That is not your production case (over 13,000 tables against at most 10,000 goals).
2. **Mode-2 admission staleness.**
   - In mode 2 the batch skips any goal outside `oriented_goals_`, and that set refreshes only every 32 steps.
   - Example: a robot receives goal g at step 33 and no other robot wanted g at step 32. The batch cannot build g until step 64, whatever the batch size.
   - This is a concrete limitation only if production runs mode 2. Its magnitude is unmeasured.
   - Add a counter for goals skipped because they are not yet admitted.
   - Since capacity exceeds the live goal count, admission is not binding. Refreshing it every step is an O(n) deterministic fix.
3. **Wall time.**
   - Compaction and admission run serially on the main thread.
   - The 128 demand-pass builds are also serial.
   - The same tables would cost less wall time inside the parallel batch.
   - A reasonable configuration is demand limit 32 plus a parallel batch.
   - The 0.946 s maximum at batch 512 leaves 54 ms of margin, and each full run has ten publication steps. A deadline overrun fails the run explicitly, which is the correct semantics, but the run is lost.
   - Log which step produces the maximum entry time.
4. **Statistics.** `prefetched_hits` now also counts batch admissions. If both features are enabled, the speculative "used" statistic is conflated.

**Missing tests that target actual risks:**
- **Mode 2.**
  - A goal appears one step after an admission refresh.
  - Assert the skip counter and serial/parallel equality.
  - Assert that the 32-step `retain` never evicts a table admitted in the same step.
- **Deadline expiry inside a chunk with 4 threads.**
  - Assert that the Timeout propagates and nothing terminates.
  - Assert that `prefetched_` is empty and that the LRU list and the map have equal sizes.
- **Tiny cache with overshoot.** Assert that the next `trim()` evicts the same set for serial and parallel runs.

## 2. Strategic judgment

**Inference on the coverage lever.**
- Raising the build limit from 32 to 128 added 1.81% throughput and 21.7 points of average sampled coverage. That is about 0.08% throughput per coverage point.
- About 2.6 points of average coverage remain, plus short dips after each invalidation.
- I expect v40 to add about 0.2–0.4% over limit 128. That makes it insurance against the dips, not a new lever.

**Candid position.**
- No mechanism your measurements support is worth more than about 2%.
- If margin 25 (+1.4%) and the higher build limit (+1.8%) stack, you reach roughly 139k. That is still about 10% short of 152,981.
- KK used more CPU and memory than your rules allow.
- My earlier assignment predictions were wrong: HRRN off did not help, and late binding gained far less than my 1.5% prediction. Weight what follows accordingly.

**Where the remaining gap sits, from your travel audit.** These are different task cohorts, so the split is not causal.
- About half is assignment-side, per task: roughly 9 steps from chain mix and roughly 10 steps from empty travel.
- About half is motion overhead.
- Search-side changes are spent: four workers +0.2%, two-owner transactions −0.29%.
- Conservative late binding gained 0.19%.

**The one experiment: price pickups in the planner's metric inside the scheduler.**
- **Change.**
  - In `candidates()`, replace the unit BFS with a bounded, robot-centered forward search over (cell, heading).
  - Use the published forward costs and turn costs.
  - This puts all 16 candidates on one basis.
  - Chain legs stay unit-cost, because they do not depend on the robot.
- **Measured premises.**
  - Direct-cost dispatch improves monotonically with pickup weight on seed 0:

    | Pickup weight | Seed 0 tasks |
    |---|---|
    | 1 | 131,199 |
    | 3 | 134,397 |
    | 5 | 136,149 |

  - Weight 5 beats HRRN by 1.2% on seed 0.
  - Weight 5 collapses on seed 2, with 4,447 tasks in the final 1000 steps. An assignment rule alone can therefore push traffic over the congestion boundary.
- **Source fact.**
  - The scheduler prices pickups by unit distance, blind to heading and to the tolls the planner obeys.
  - A stronger refreshed convention (margin 25) widens that mismatch.
- **Hypothesis.**
  - Unit-nearest pickups are disproportionately upstream of the robot.
  - Strong pickup weighting multiplies against-flow trips.
  - Those trips destabilize seed 2 and inflate empty travel.
- **Offline gate, current-information only, on the saved weight-5 runs.**
  - For each assignment, compute the chosen pickup's weighted-oriented cost minus its unit distance.
  - Also check whether the best of the 16 candidates changes under the planner's metric.
  - Compare seed 2 before its collapse against seed 0.
  - If the best candidate changes in under about 5% of assignments, or the excess is small and flat, drop the idea without a run.
- **Falsifiable full-run outcome.**
  - Weight 5 stays healthy on seed 2, with a final window of at least 27k.
  - Weight 5 keeps at least +1% on seed 0.
  - Under HRRN, empty robot-steps per task fall by at least 2%.
  - Reject the change if any of these fails.
- **Honest size.** I expect +0.5–2%, plus possible access to the weight-5 regime. It will not close the gap alone.

**A no-code companion, secondary.**
- Test graded tolls with strength 4 at cost scale 4 and a low margin.
- Your measured pattern is margin 75 < 50 < 25 under refresh, while doubled strength and uniformly weak tolls both lose.
- Graded tolls keep the full-step toll on fully polarized edges and use fractional tolls on weaker ones.
