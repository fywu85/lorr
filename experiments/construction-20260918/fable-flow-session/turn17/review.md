I ran nothing.

The V50 change looks sound from the source. `construct` and `repair` both check immediately before every root attempt, so dropping the check at `counter == 0` loses nothing. The checks every 16 recursions and the final complete-entry check are unchanged.

## Decision

- **Arm A** is coherent. It is meaningfully different from both earlier failures. I would run it as one bounded paired experiment, with one required change and one prior gate.
- **Arm B** I would defer until A shows a consistent signal.
- I see no source reason to reject either outright.
- My expectation for A is small: a refinement of the HRRN denominator, not a structural lever.

## Why A differs from the two earlier failures

- The known-next objective changed the planner's 5-step score, which acts through reservations and conflict priority. Arm A changes only which task a free robot takes. The planner's objective is untouched, so the known-next loss says nothing about pricing.
- The Manhattan-to-BFS refinement added almost no information on a warehouse map. Oriented directed legs add necessary turns and learned tolls. The spatial chain has never priced either, and both vary by task.
- The closest precedent is pricing the pickup leg in this same metric. That was mildly positive: +0.3%, and it was a prerequisite for complete pickup fields.

## Why I expect little from A

- Age dominates HRRN. A more expensive task is deferred only until it is proportionally older, so the completed mix barely moves. The benefit is limited to better choices among candidates that are nearly tied.
- B adds a regret of at most about two turns per task, on a cost of roughly 300 steps. That is little extra information for 72 MB and additional branching.

## Gate before implementing: does the oriented excess predict anything?

- Work from saved trajectories, using the reconstructed published field that was live at each pickup.
- For each completed task compute:
  - the unit chain;
  - the relaxed oriented chain;
  - the realized loaded time.
- Ask whether (oriented chain − U × unit chain) improves out-of-seed prediction of (realized loaded time − unit chain). Judge that against a permutation null in which the excess is shuffled.
- If it adds nothing, there is no information to price. Stop there without building anything.
- The work is bounded: one reverse search per distinct destination per publication.

## Required change to A: the raw fallback has a built-in sign

- The oriented chain is always at least U × the spatial chain, because both use the same certificate domain.
- A task missing any one table therefore always looks cheaper than a covered task.
  - After each publication most tasks are uncovered. The basis is then consistent, and those ticks are fine.
  - As tables rebuild, the basis becomes mixed.
  - In steady state, tasks that happen to have cache misses are favoured. That property has nothing to do with travel cost.
- The fix is ratio imputation.
  - Per scheduling call, take κ = Σ oriented / Σ (U × spatial) over the covered tasks. Use integer arithmetic in task-id order.
  - Price each uncovered task at U × spatial × κ.
  - If no task is covered in a call, that call is bitwise native.
- Without this fix, a loss cannot be interpreted.

## Source feasibility and units

- **Placement.**
  - Compute the chain where the `tasks` vector is built. It needs one `peek` per future destination and the minimum over 4 headings of `value(table, previous location, heading)`.
  - The turn oracle is neither built nor trimmed during scheduling, so the pointers are transient and safe.
  - Still call `task_chain_cost`, so the spatial oracle's admission and LRU behaviour stay native.
- **Units.**
  - Oriented values are already in scaled units. Add them inside `pair_for` without the extra multiplication by `pickup_scale`.
  - Accumulate sums in `long long` and cap them below `kInf`.
  - Keep the reported `estimated_chain_cost` in unit steps.
- **Repeated locations.**
  - They give a leg cost of 0, because the table root is 0 for every heading.
  - Per-tick service time is omitted, as in native.
- **Own-cell pickup in B.**
  - Distance is 0 at the robot's current heading.
  - The other headings cost turns.
- **Pocket pickups and any infinite state.**
  - The whole task falls back to native pricing.
- **Recomputation.**
  - Recompute on every call. Table residency changes each tick, so a publication-keyed cache would be wrong.
- **B's coupling is exact in the relaxed metric.**
  - The minimum over heading h of [W × F(first, h) + D_next(first, h)] is the shortest state-graph path through the pickup cell.
  - A rotation at the pickup cell is counted once, either in the forward field or in the next-leg table.
  - The regret is never negative.
  - With W ≠ 1 that rotation carries an ambiguous weight. Accept this as an approximation.
- **B's buffer retention.**
  - Swap the finished `distance_` buffer into the output field.
  - The next run's `assign` reuses the swapped-in buffer, so there is no steady-state allocation.
  - There is no aliasing, because each field slot owns its own vector.

## Invariants that hold

- **Fair admission is unchanged.**
  - For a fixed task the chain term is common to every robot.
  - The score is monotone in the pickup cost, so the same robot is chosen.
  - This is a good property to assert in a test.
- **Oldest-task order** depends on reveal time only.
- **Task and robot uniqueness, started tasks and primary protections** live in unchanged code and in the validator.
- **The `at_cell` bucket order does change.**
  - It uses `pair_for` with a zero distance.
  - The change is deterministic, but it alters which tasks survive the truncation in the bounded local search.

## Complexity, with F free tasks, L remaining legs, Q pickup fields and C cells

- **A:** O(F × L) hash peeks per scheduling call, each with a 4-heading read from a different table.
  - Each read is probably a cache miss, so measure this in the 200-step screen.
- **B:**
  - O(Q × C × 4) extra memory (your figure is 72 MB).
  - No extra search work, since it is a buffer swap.
  - O(F × 4) precomputed next-leg values per call.
  - O(Q × F) extra min and add operations in the full-field scan.
- **Your latency ceiling is already 962 ms.** Report the maximum entry time with the option ON against OFF at the ticks around each publication.

## Minimum tests

**Independent oracle on small grids with directed tolls:**
- Build the tables by repeated relaxation on the state graph.
- Assert, for each task, that U × spatial ≤ the relaxed oriented sum ≤ the exact chain cost with full heading coupling.
  - The exact chain cost comes from independent layered relaxation.
- Assert that B's coupled subtotal equals the true two-leg minimum at W = 1, and that the regret is never negative.
- Assert that neutral scaling (scales 1, 2, 4, 8 and surcharges 0, 1, 2 with uniform tolls) gives identical assignments.

**Scheduler fixtures:**
- The robot chosen for a fair admission is identical with the option ON and OFF.
- A task with a missing table:
  - it gets κ-imputed pricing;
  - a call with nothing covered is bitwise native;
  - a task with a single remaining stop is native.
- Repeated-location and own-cell pickups.
- A pickup in a pocket, and a pickup with an infinite state.
- Across a publication:
  - coverage goes covered, then uncovered, then covered with no stale value;
  - the turn-oracle residency set, LRU order and `oriented_builds` match OFF exactly.

**Whole-system checks:**
- Serial against 4-thread field builds with retention on give identical state distances.
- The assignment passes `validate_task_assignment`.
- The protected-task regressions pass with the option ON.
- With the option OFF, full-trajectory hashes are exact.

**Diagnostics:**
- Whole-chain coverage, split by ticks since the last publication.
- κ per call.
- The number of changed chain costs.
- The share of pairs with positive regret (B only).
- A shadow native ranking that counts changed first choices.

## Promotion

- Run seeds 0 and 2 against exact controls.
- Then run six seeds before any promotion.
- Expect a small effect in either direction.
