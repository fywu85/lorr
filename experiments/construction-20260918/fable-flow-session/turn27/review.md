I did not run anything. Reading the delta, I found no defect in the scoring change, the goal-hit and toll cutoffs, or the protected construction. There are three regression gaps and one legal combination that mixes objectives.

**What holds in the implementation**

- **Default-path identity.** With the option at 0, `flow_cost` computes the previous composition exactly.
  - The surcharge has the same first-service cutoff, the same unit argument and the same scale multiply.
  - The adapter applies it under the same `oriented && !guided && weighted_forward()` condition as before.
  - Evaluation order is unchanged.
  - `forward_surcharge` never calls the memoised `distance`, so the memo state matches too.
- **Cutoff.**
  - By default the move onto the goal is paid and later moves are not.
  - With the option ON nothing forward is paid, and arriving candidates keep `-t*U`.
  - Your hand count is right: the paid extras are 4 + 0 + 2, giving −612 by default and −912 with the option ON.
- **Oracle replay.** It matches production on the edge cases I checked:
  - `paid` is tested before `first` is updated, so the entering move is charged.
  - `turns_first` includes a turn on the hit tick.
  - For own-cell starts, a forward-first path pays every move and a wait-first path pays none. Both match the `break`.
  - The nine trailing-wait variants reduce to the production minimum.
- **Protected construction.** I found no hidden dependency.
  - The primary's progress, stall and recovery use the spatial potential.
  - Pinned robots have a single choice.
  - Priorities and the wait-seed projection read oracle values only.
  - The certified PIBT pass keeps its own paid macro ranking.
- **Two local objectives.** Pinned and protected robots therefore stay on the paid objective while the temporal layer uses pure potential.
  - This follows from your design and is not a bug.
  - It is worth one sentence in the experiment note.
- **Parse order.** Every flag the new guard reads is parsed before the guard runs.

**Finding 1: a legal combination silently mixes objectives.**

- Non-unit turn pricing is not rejected.
- With `CGAR_TURN_SURCHARGE > 0`, or `CGAR_TURN_COST > 1` at scale 1, `cost()` still pays turn extras while forward extras are dropped.
- This is inert in the reference configuration, but it is legal.
- For this first experiment, reject `turn_surcharge_ != 0 || turn_cost_ != 1`.

**Finding 2: four rejection tests pass without exercising the new guard.**

- Earlier, pre-existing rules fire before the guard in these cases:
  - `CGAR_TEMPORAL=0`.
  - `CGAR_FLOW_STRENGTH=0`, caught by the scale-4 rule that requires flow.
  - `CGAR_ORIENTATION_GUIDANCE=0`, caught by the rule that learned flow requires orientation guidance.
  - `CGAR_GUIDE_ROUTES=1`, caught by the rule that guide routes require flow to be disabled.
- The guard's `requires` clauses are therefore untested. With scale 4 they are effectively unreachable.
- **Fix:** assert the exception message, matching a substring unique to this guard.
  - Add one reachable case: scale 1 and flow strength 0, with temporal planning and orientation guidance on.
  - Apply the same message check to the trick-combination rejection in `warehouse_trick_regression`.
- The service-audit, conflict-audit, branch-work and next-errand cases should reach the new guard.

**Finding 3: the production-level ON tests may be vacuous.**

- The three sub-regressions run under ON with warmup 4 and margin 25.
- Nothing asserts that a penalised edge was ever published.
- Nothing asserts that ON changed any decision.
- The neutral tests prove only the inert path.
- Add one weighted production fixture that asserts three things:
  - `flow_penalized_edges > 0`.
  - At least one action differs from OFF.
  - Every step is collision-free and the primary completes.
- Without it, the active ON path at adapter level is covered only by the function-level oracle.

**Finding 4: adapter-level default identity currently rests on the full-hash controls.**

- The function-level check `off == independent.native` compares against the production helpers.
- It cannot catch a wiring error in the adapter.
- The two OFF seeds that will run alongside cover this, so this is a statement of the dependency and not a request for another test.

**Minor points**

- **Receipt.** Make the runner check two-sided.
  - ON runs must contain `[cgar-temporal-score] remaining_flow=1`.
  - Controls must not contain it.
- **`getenv` in the save/restore loop.** `std::string saved = getenv(key)` is safe only while those keys are set.
  - They are set at that point.
  - A sub-regression that unsets `CGAR_TEMPORAL` or `CGAR_ORIENTATION_GUIDANCE` would turn this into undefined behaviour if it were ever moved above the loop.
  - `temporal_primary_regression` unsets both at its end.
  - Keep the current order, or guard against a null return.
- **Audits.** The conflict and service audits are rejected under ON, which is right for their paid-progress partitions.
  - The offline replay readouts are unaffected: tolled-move share and the per-epoch corridor minority share.
- **Overflow.** I accept your bound: five-slot tolls in int64.

I retain your other corrections:

- States have legal macros, not three one-step forward alternatives.
- The measured tolled-move share of 1.50–1.66% is not an estimate of ranking changes.
- The startup attribution is plausible, not isolated.
- Similar tails, and counts that differ in offsetting ways, prove neither equal fields nor cancellation.
