I read the V47 diff and ran nothing.

## 1. V47 review

I found no demonstrable bug.

**Score accounting.**
- `next_errand_cost` implements the agreed form: −4U + D*_next(end) − B, plus every turn and toll extra over the whole path.
- The adapter returns the continued score in place of the native score and does not add the native toll term again.
- Trailing-wait virtual rotations are only ever applied after service.
- The op-0 seed and the geometry's all-wait path have identical cells, so the `arrives[0]` flag computed from the geometry paths applies correctly to the seed.
- The independent replay identity in your regression is the right oracle. It covers every variant of replacing trailing waits with rotations.

**Cache lifetime.**
- The `peek` runs after the PIBT builds and after the batch builds.
- Nothing later in `plan_temporal` builds, trims or retains tables.
- A later `find` only splices the LRU list, and the table map is node-based, so the pointers stay valid through the preparation workers.
- The workers read only const data, and the counters are per worker.

**Protected and fallback robots.**
- Pinned robots are skipped before any lookup.
- A robot falls through to native scoring for its whole candidate set in any of these cases:
  - it has no exact current metric;
  - it is guided;
  - its next table is missing;
  - the next errand is the current goal;
  - any heading at the goal is infinite on the next table.

**Intentional approximations I noticed:**
- The finiteness pre-scan covers every geometrically valid arriving path, including paths the later core, allowed and witness filters will drop. Near pockets, one infinite endpoint disables the whole robot even if that candidate would never be offered. This is conservative and not a bug.
- The final seed-turn projection still uses the current-goal metric. It matters only for a robot already standing on its goal at plan time, which the task lifecycle makes rare. You have already flagged this.
- A path that visits the next errand and then leaves keeps the next potential. You have flagged this too.

**Counter semantics.**
- `eligible` counts robots that are nowhere near arrival.
- The informative readouts would be:
  - robots with at least one arriving choice;
  - how many selected choices are next-scored;
  - of those, how many leave the goal inside the window and how many hold it.
- The last split shows directly whether plans stop holding the goal.

**Unknown effect.**
- Your own audit puts the known-next-only class at about 7 robots per frame.
- The finishing-only class is about three times larger, and this policy does not touch it.
- I would expect a small effect unless the changed candidate set matters more than the audit suggests.
- You have already disclaimed the buckets as bounds, and I agree.

**Missing meaningful tests:**
1. **Whole-set fallback triggered by an endpoint.**
   - Setup: the goal is finite on the next table, but one arriving path ends on a cell outside the next table's domain.
   - Assert the entire candidate set is bitwise native.
   - Kind 5 only covers an infinite goal.
   - This needs a fixture with a pocket.
2. **Robot already on its goal at plan time** (a task assigned at its own cell), with a known next errand.
   - Check that the seed arrives at t=0 and first-forward candidates do not.
   - Check that the executed seed rotation is what you expect.
3. **A reason-coded split of `unavailable`.**
   - Reasons: no table, infinite goal, infinite endpoint, fallback or guided metric.
   - Without it, the full runs cannot say how much of the policy was actually active.

**Deadline.**
- The added work is one serial `task_pool.find` per robot, plus a scan of at most 129 operations for robots within distance 5.
- That is negligible.
- The real risk is the regional reference's 962.69 ms maximum.
- In the 200-step screen, report the maximum entry with the option ON against OFF at the ticks around each publication.

## 2. Retaining the best state in regional repair

**The rollbacks do not establish that a better interim state exists.**

1. **What a rollback proves.**
   - It proves only that the final score is not above the initial score.
   - The "accepted" count includes equal-score moves and annealing-accepted losses, so many accepted attempts do not imply any net gain.

2. **What the schedule's shape implies [code-based estimate].**
   - `consider()` divides the loss by `old_score_`. `old_score_` is the whole-plan score, because the regional object holds all robots.
   - Temperature starts at 1e-3 and multiplies by 0.999 per attempt.
   - A loss L is accepted with probability exp(−(L/S)/T_k). With S of order 10⁶ and L of order 10²:
     - almost every worsening move is accepted at the start;
     - acceptance falls to a few percent only after roughly (1/0.001)·ln(1e-3·S/(4.6·L)) attempts, a few thousand of the 25,000.
   - After that the search is monotone non-decreasing.
   - The trajectory is therefore a hot kick followed by greedy recovery.
   - Its maximum is essentially max(initial plus any very early gains, final), which the current rule already returns.
   - Each round re-heats the temperature.

3. **Score versus throughput.** Even a higher interim scalar score is not throughput, as the transaction experiment showed.

**Measure first with a const tracker.**
- Inside `repair`, keep a running maximum of `score_` and the attempt index where it occurred.
  - That costs one compare per attempt.
  - It uses no RNG and changes no state, so hashes stay exact.
- Report three things per batch:
  - maximum minus initial;
  - final minus initial;
  - the number of batches where the maximum beats the initial score by more than 1e-6 while the final does not.
- Those are exactly the batches retain-best would change.
- If that count is near zero, do not build the feature.

**If the tracker justifies it, a cheap implementation (regional flag only, global default unchanged):**

**State:**
- `best_score`;
- `best_epoch_`;
- a per-robot `dirty_epoch_`;
- `since_best`, a list of (robot, selection at best);
- a per-attempt list `attempt_touched_` of (robot, selection before the attempt), first touch only, tracked with its own stamp.
  - Do not use `visited_` for this, because it is cleared on failure paths.

**After each accepted attempt:**
- For every touched robot whose selection really changed and that is not yet dirty in this epoch, push (robot, old selection) and mark it dirty.
- If `score_` exceeds `best_score` by more than 1e-6:
  - set `best_score`;
  - clear `since_best`;
  - increment the epoch.
- This is amortized O(1) and never copies all n selections.

**At the end, after every prescribed attempt:**
- If the final score is below the best:
  - remove all journaled robots;
  - assign their best selections;
  - add them back.
- The restore is two-phase (remove everyone, then add everyone), so there are no transient conflicts.
- Assert `blocker == -1` before each add.
- Set `score_` to `best_score`, not to the inverse arithmetic, because the merged plan is rebuilt fresh anyway.
- Then apply the existing keep-or-revert rule against the initial state.

**Properties:**
- A timeout still throws.
- The search path and RNG stream are unchanged. Only the returned state differs, and later rounds differ from there.
- Each region's result is still never below its initial score, so the merged non-decreasing check holds.

**Fixtures:**
1. **Flag OFF.**
   - Selections, score, stats and the next RNG draw are bit-identical across your 512 tiny instances.
2. **Flag ON.**
   - The returned score equals the maximum recorded by a test-only per-attempt observer.
   - The returned selections equal the observer's snapshot at that maximum.
   - The plan is collision-free.
   - The score is never below the flag-OFF score.
3. **Restore.**
   - After a restore, the reservation tables equal a fresh construction from the returned selections.
4. **Journal integrity.**
   - Fixed robots are never journaled.
   - There is at most one entry per robot per epoch.
   - The journal is empty immediately after a new best.
5. **Timeout.**
   - An injected mid-repair timeout propagates and leaves no result.
6. **Regional adapter.**
   - The merged score is non-decreasing over a multi-round run.
   - Your protected serial/parallel action checks pass.

**A simpler alternative from the same code reading (hypothesis, one bounded pair).**
- The repair starts from a plan already optimized by 4M attempts.
- A kick scaled against the whole-plan score may waste the 25,000-attempt rounds that end in rollback.
- The existing `CGAR_TEMPORAL_REGION_TEMPERATURE_PPM=0` makes regional repair pure descent:
  - it is monotone and is never reverted;
  - every accepted gain is kept;
  - the work is the same fixed work.
- Run seeds 0 and 2 against the confirmed regional control, with the tracker counters in both arms.
- If the kept annealed batches carry most of the regional gain, keep 1000 ppm. In that case retain-best is the only remaining way to recover rollback work, and the tracker will say whether there is anything to recover.
- Whichever arm wins on seeds 0 and 2 still needs six seeds before promotion.
