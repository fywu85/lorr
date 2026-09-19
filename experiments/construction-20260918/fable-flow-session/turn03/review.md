## Facts and inferences

**Established by your runs:**
- Refresh512 reaches 134.5k on both seeds, 8 tasks apart, with final windows of 28,215 and 28,214.
- Frozen plus warm start reaches 123.4k on seed 0 and 123.3k on seed 2.
- Strict wait turns rescue frozen seed 2 to 121.5k.
- Refresh512 plus strict reaches 133.7k on seed 2.

**Inferred:**
- The seed-2 collapse is an avoidable unstable state and the field is not fatal, since three unrelated local changes each avoid it.
- None of those changes is shown to remove the instability; each may only sidestep that one trajectory.
- The refresh gain survives strict on seed 2 (133.7k against 121.5k, +10%), so it does not come from suppressing tied seed rotations.

## 1. Cache-only refresh control

The design is sound as a test of whether the flush alone is sufficient. I see five issues.

1. **The seed-2 baseline is confounded.**
   - Frozen-legacy seed 2 is a collapsed run, and three perturbations already rescue it to about 122k.
   - A cache-only result near 122k on seed 2 means the collapse was avoided. It does not mean the refresh gain was reproduced.
   - Pre-register the question as whether cache-only reaches about 134k, against the healthy-frozen level of 122–123k. Seed 0 is the clean comparison.
   - Better still, also run the three arms under strict=1, where the seed-2 frozen baseline is healthy: 121,534 / ? / 133,672.

2. **A flush acts through two channels [code fact].**
   - Until a goal's table is rebuilt, scoring uses the toll-free fallback.
   - In the same period `turn_oracle_.find(goal)` is null, so the whole wait-seed rotation block is skipped. Tied and useful seed rotations both vanish.
   - Under the legacy rule, that second channel overlaps the strict-wait effect.
   - To separate the two, log the new rotation counters and the exact/fallback counts every step for about 64 steps after each publication, in both arms. This logging does not change any actions.
   - The strict=1 arms remove the tied-rotation part of the second channel.

3. **Match what genuine refresh actually did.**
   - Genuine refresh flushed only at publications with `cache_reset=1`. That is probably all of them, but verify it from the logs.
   - Use the same call site in `plan()`.
   - Clear exactly what `set_forward_costs` clears: tables, LRU and prefetch. Do not clear `oriented_goals_` or the spatial oracle.

4. **What this control cannot decide.**
   - If cache-only is about equal to frozen, a better field and non-stationarity both remain possible. Loading a late refresh field at sample 128 and freezing it is still the complementary control.
   - If cache-only reaches about 134k, the learned field is a red herring.

5. **No rescue claim yet has a null perturbation.**
   - On frozen-legacy seed 2, burn one `temporal_rng_` draw at step 129, in at least three variants.
   - If most variants still collapse, the instability is robust, and the strict, warm-start and flush rescues carry information about mechanism.
   - If most recover, the original collapse was one unlucky path, and the rescues tell you little.

## 2. Two next changes

Rough arithmetic from your numbers:
- Refresh steady state is about 355 robot-steps per task.
- The target's whole-run average needs about 327 or fewer.
- In healthy frozen-flow late windows, waits are 6.5% of actions and turns are 6.7%, so no single category closes the gap.

Please send the refresh512 late-window forward/wait/turn split. Also send forward-closer/farther/equal under both the unit metric and the weighted metric. That apportions the gap between queues, toll detours and displacement.

**A. Publish measured dwell time through the refresh channel.**
- **Mechanism:** Refresh shows that a field learned under the traffic it induces is worth about 10% on both seeds. It carries only contraflow information, because `observe` discards `u == v` and with it all delay information.
- **Change:** With B ≥ 4, add a capped per-cell entry surcharge of about B·(mean dwell − 1) at the same publication points. D then approximates expected travel time. This requires v31 to be valid.
- **Upside:** Bounded by waits plus queue-induced turns.
- **Offline falsifier, from saved refresh512 trajectories:**
  - If the top 5% of cells hold under about 25% of non-moving robot-steps, a cell toll cannot steer traffic.
  - For sampled trips, if time-metric shortest paths save less than 1–2% against the realized routes, drop the idea without a run.
- **Run criterion:** Late-window waits must fall, and tasks per 1000 steps must rise beyond the roughly 0.6% configuration noise, on both seeds. If waits fall but extra forward moves offset them, count it as falsified.

**B. Price scheduler legs with the planner's published metric.**
- **Code fact:** Chain costs come from `DistanceOracle`, which is unit BFS with no flow costs and no orientation. Assignment is therefore blind to the field the planner obeys.
- **Mechanism:** Under a one-way-ish field, a robot that is near in unit distance can be far in flow distance.
- **Change:** Use already-cached oriented tables for the robot-to-first-errand leg, with no new builds, and divide by B.
- **Offline falsifier:** Recompute that leg under both metrics over all refresh512 assignments. Drop the idea if the chosen robot would change in under about 5% of assignments. Also drop it if realized pickup time is not better predicted by the weighted metric.
- **Upside:** Your chain-length gap of 240 against 234 bounds the pure assignment part near 2.7%.

**Free check alongside:** Run refresh256, since going from 1024 to 512 improved both seeds.

## 3. Branching displacement

- **Verdict:** Not yet justified.
  - Three non-search changes now avoid the collapse, so the search neighborhood is not indicated as its cause.
  - For the 28k steady state, it is simply unmeasured.
- **Counter to measure first:** Sample about 1 step in 64, in a non-mutating pass so the hashes are preserved. For each robot ending on a non-progress selection, classify why its best lower-cost candidate failed:
  - `-2` touching exactly two non-fixed robots;
  - `-2` touching three or more robots, or any fixed robot;
  - single-blocker chain failure or budget exhaustion;
  - rejection by `consider()`;
  - no better candidate existed.
- **Reporting and decision rule:** Report this for the refresh steady state. Build a bounded two-branch repair only if the first class is at least about a third of lost-progress robot-steps.
- **Ceiling:** Non-moving steps are about 13% of actions in healthy flow windows, and fixed 4M work means branching buys depth with fewer attempts.
