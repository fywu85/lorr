I ran nothing. This is a read of 8546afa and its tests. I make no claim that anything passes.

## Verdict

I found no concrete defect in the ledger, the time convention, the comparator or the zero-flag path.

## Ledger and time

- **Duration convention.**
  - Completion is observed at the first schedule call after TaskManager removes the task.
  - So `duration = now − admitted` equals the ticks used, from the admission tick to the completion tick inclusive.
  - That is the same convention as `bound ≤ H − now`.
  - A task admitted and completed within one tick is first observed one tick later with duration 1. This matches your "same-step completion is not yet observed" fixture.
- **Training conditions are all checkable at that call.** `observe` runs before this entry's new assignments. At that point:
  - the holder's schedule entry is −1;
  - the robot is standing on the final errand cell.
- **A rejected or altered proposal is caught on the next `observe`.** Both `agent_assigned` and `curr_task_schedule` are tested there.
- **Prospective use.**
  - The snapshot is frozen per entry.
  - Only completions observed before the decision can enter it.
- **Early-return path.**
  - Tasks assigned in this entry are still in `chain_cost_`, because the prune uses the free set from the start of the entry.
  - `record_horizon_proposal` therefore finds their chain on that path too.

## Same-call matching and new admissions

Recording after `match_unopened` handles these cases:

- A fresh task X moved from robot F to robot H inside the same entry.
  - X is recorded once: for H, from H's position, at this tick, as single-holder.
  - H's displaced task is invalidated by the first loop in `proposed()`.
  - It stays invalid until the pool removes it.
- Two fresh robots swapped by a cycle both record their final holders.
- A robot's just-completed task is erased by `observe` before the robot's new record is created. Two live records cannot collide on one robot's fresh admission.

## Comparator, heap and isolation

- **Strict weak order.**
  - The order is lexicographic: tier in {0, 1, 2} first, then the ordinary comparator.
  - Tier is a pure function of the stored pair and the frozen snapshot, so this is a strict weak order for both the sort and the retention heap.
  - `horizon_impossible` is equivalent to tier 2, so the two flags cannot disagree.
- **Flag off.**
  - Tier collapses to 2 or 0.
  - `selected` then equals the V86 minimal comparator on every comparison, and outside the horizon it equals the ordinary comparator.
  - The ledger calls are all guarded by `horizon_margin_`.
- **Prefix proof.**
  - `selected != minimal` is the right reference for the prefix proof against your V86 ON runs.
  - Before the first logged margin reversal, every comparison result equals the minimal-horizon result.
  - The ledger does not feed into decisions any other way.

## Concrete items

1. **Bound violations are hidden.**
   - A completion with `duration < bound` currently lands in `excluded_completions`. That case would signal a convention bug or an unsound bound.
   - Count it separately. It should stay at zero, and it is the cheapest online detector.

2. **Receipts.**
   - `[CGAR_TRICK_HORIZON_MARGIN]` is a separate line. It is not part of `[CGAR_TRICK_COMPONENTS]`.
   - The immutable verifier should require it exactly when the margin is configured.
   - It should label the result known-horizon with margin.

3. **Supported modes.**
   - Pool exchange is rejected.
   - The old swap pass is already rejected under `--trick`. If it were ever allowed, `proposed()` would still invalidate moved tasks.
   - Short preference composes without ledger changes.
   - The budget shadow works on a copied proposal, so it cannot reach the ledger.

4. **Ledger cost.**
   - `observe` and `proposed()` walk every record, about 10,000, with a hash lookup each, every tick.
   - Report the added decision time. It is timing only and does not affect results.

## One materially missing fixture

- **The gap.** No test drives the real scheduler through a same-call matching move of a fresh admission with the margin on.
  - The "final first holder" test calls the model directly with a hand-made proposal.
  - The real path is the one most likely to hide a wrong-robot or wrong-tick bound.
- **How to add it.** Extend your existing fresh-holder matching fixture, where the fresh task's pickup table is already resident.
- **Assert the following.**
  - The displaced task never trains.
  - The moved fresh task trains on completion with `excess = duration − bound(final holder at the matching tick)`.
  - The bound-violation counter stays at zero.

## One measurement qualification

- The calibration cohort is never-retargeted tasks.
- Matching preferentially moves tasks whose initial pickups were poor. The survivors therefore likely show smaller excess than the full population.
- Some records also use Manhattan bounds, which you already noted.
- These two biases push in opposite directions. Treat the bucket means as a heuristic ranking signal, not a duration model.
- The V90 offline counts use exact BFS and a different cohort. They should not be read as a forecast for this production rule.
