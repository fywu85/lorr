I found no counterexample to atomicity, undo order, reservation ownership, fixed-robot protection, or fail-closed limits by reading the supplied header and adapter diff. I did not execute anything. Below I separate why each invariant holds, one latent defect, optional optimizations, and the regression I would add.

## 1. Atomicity, undo, duplicates, cycles, ownership

- **Undo is exact.**
  - Each `Change` stores the state before that change, and `undo` replays entries in LIFO order.
  - A robot can appear twice in the trail (deactivation, then placement) and is still restored correctly.
  - The strict owner check in `reserve` turns any mis-ordered restore into a `logic_error`, so it cannot become silent corruption.
- **Sibling backtracking is sound.** `place` and `resolve` take `pending` by value, so a failed candidate cannot leak owners into the caller's list.
- **The terminal is complete.** `pending` always carries every unplaced robot down a single call chain. `pending.empty()` therefore implies every robot is active, and the `active_count_` assertion agrees.
- **Duplicates and cycles cannot occur.**
  - Owners are de-duplicated inside `take`.
  - Pending and root robots hold no reservations, so an owner lookup cannot find them.
  - Placed robots are locked, so each robot is placed at most once per branch.
  - Depth is bounded by `max_agents`.
- **Physical consistency holds.**
  - A pending owner whose start cell is claimed at t=0 can only accept a first-forward escape.
  - Staying in place conflicts with a locked reservation.
  - A swap conflicts in `edges_`.
  - This matches the baseline table's semantics.
- **Rejecting before mutating.** The owner-count, `max_agents`, fixed and locked checks all run before the first `change`.

## 2. Acceptance, floating restoration, fixed robots

- **Acceptance and restoration are correct.**
  - `before_` is the complete score, taken before the root is removed.
  - Every failure path restores `score_` from a snapshot.
  - The commit path re-asserts both completeness and improvement.
- **Fixed-robot protection.**
  - The adapter passes `pinned`, the same mask the workers used.
  - The helper re-verifies that every fixed selection is unchanged.
- **One latent defect, not a demonstrated counterexample.**
  - The final helper check compares a fresh index-order sum, `result->score()`, against `baseline.score()`.
  - `baseline.score()` is a running sum produced by millions of add and remove operations.
  - In your tested profile (power = 1, integer costs) both values are exact.
  - Under rank weights the smallest true improvement is about 1/(n+1). Running-sum drift could approach that.
  - The consequence would be a fatal `logic_error`, not a `Timeout`.
  - Fix: compare against `stats.score_before`, which is computed in the same summation order over the same selections. Treat any mismatch with the baseline as a diagnostic.
- **Documented limitation, restated.** Displaced owners can lower total physical score while operation-tie terms carry the scalar score upward. This is the behaviour you documented, not a bug.

## 3. Work limits and exceptions

- **Limits are deterministic.**
  - They are pure counters.
  - Every nested loop tests `exhausted()` before `consume`, so candidate inspections never exceed `work`.
  - Exhaustion can only return false and trigger a rollback.
- **Exceptions fail closed.**
  - `check()` can only throw.
  - The trial object is local and the input plan is immutable.
  - No result exists until construction finishes and the final `check()` passes.
  - The adapter call sits outside the worker try/catch, so a `Timeout` reaches the Entry failure path.
  - With branch work 0, the helper returns before allocating anything.
- **Optional optimization: consume after filtering.**
  - The root loop calls `consume` before its filters, so roots with no qualifying candidate still spend budget.
  - Choices are sorted by ascending cost, so the first `candidate.cost >= old.cost` could `break` instead of `continue`.
  - Both changes save budget and neither affects correctness.
- **Coverage, not correctness.**
  - `start = timestep × 256` indexes ranks, not robots.
  - Only about 10% of ranks are eligible, so each pass scans roughly 2,700 ranks while the window advances only 256 per step.
  - Consecutive passes therefore overlap by about 90%, and ranks far from the window wait about 39 steps before their first chance.
  - A persisted cursor would even this out.

## 4. Missing regressions

I found no bug to pin with a regression. The largest untested surface is below.

1. **Multi-root pass.** The fixture uses `roots=1` and `order{0}`, so several paths never run:
   - lock release after a commit;
   - `trail_.clear()`;
   - re-snapshotting `before_`;
   - a rollback that follows a commit.

   The scenario I would add:
   - Root A commits, displacing robot X.
   - Root B then fails at a terminal.
   - Root C commits by displacing X again, so X must be unlocked.
   - Assert the selections against an independent brute-force replay. B's rollback must leave A's commit intact, and the final score must equal a fresh sum.
2. **Swap-only owner.**
   - The root moves a→b at t=0 while the owner moves b→a, so the conflict exists only in `edges_`.
   - Assert the conflict is detected.
   - After a forced rejection, assert the state is restored.
3. **Depth and size cutoffs.**
   - Use an owner-of-owner chain that needs 3 or more displaced robots.
   - Set `max_agents` one below what that chain requires.
   - Assert no partial plan leaks and the input is unchanged.
4. **Fractional powers.** Use a baseline whose running score differs from a fresh sum, to pin whichever comparison policy you choose in section 2.
5. **Soundness oracle.**
   - Enumerate tiny instances, up to 4 robots with up to 4 choices each.
   - Every committed result must be collision-free, score-improving and fixed-preserving.
   - Do not test completeness, because you do not claim it.

## Interpretation of the running trials

- `owners=1` is not the existing repair.
  - It adds sibling backtracking and a terminal-only aggregate test, where `build` aborts on the first rejected chain.
  - It also adds recursion across up to 8 robots.
- Branch1 minus branch0 therefore measures transactional conversion of the one-owner class.
- Only branch2 minus branch1 isolates the two-owner capability.
- I retain your qualifications:
  - The snapshot two-owner count is not a hard closed-loop ceiling.
  - A pickup guard does not remove all task-mix or released-task confounding.

**HRRN0.** I recorded that it falsified my positive prediction. Completed chains shorten to 235.27, but empty robot-steps rise 17–18%. With only the 16 nearest candidates, ranking by d + chain trades longer pickups for shorter chains.
