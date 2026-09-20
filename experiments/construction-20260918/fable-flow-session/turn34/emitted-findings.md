I ran nothing. This is a read of the delta, your exposure samples, and the NMS excerpt.

## Delta: no correctness defect found

**Fixes that read correctly**
- The stale-ticket fix is right. Fresh holders join the candidate scan but can no longer nominate `oldest`.
- The `fair_tasks_` insertion now covers matching.
- The static-metric gate mirrors `pickup_metric`.
- Mode 0 of the grouping is unchanged: `location_count` equals `group.size()` there.
- The shared `used[]` array prevents a robot entering a group through both indexes.

**Notes**
- **Anchor exhaustion ends the pass, not the quota.**
  - Your t=2200 sample has 9,974 groups over about 208 passes, so about 48 per pass, not 64.
  - Earlier groups absorb the 128 anchor candidates.
  - Roughly 10% of residents go unexamined per pass (1,416 selected against 1,577 resident).
  - Walk the whole rotated resident list for anchors.
- **The 16/16 split is hard.**
  - When one class is scarce, slots stay empty.
  - Optionally refill from the other class after the BFS.
- **Receipts are incomplete under `--trick`.**
  - `CGAR_REASSIGN_MATCH_GROUPS` and `..._PICKUP_GROUPS` change a TRICK component.
  - Only the generic init line prints them.
  - Put both into `[CGAR_TRICK_COMPONENTS]` and into the benchmark's expected components. Otherwise two different TRICK configurations share one receipt.
- **Test gaps.**
  - No fixture has a robot that qualifies in both indexes. A bijection assert would cover it.
  - No fixture saturates the 16/16 caps.
- **Budget pressure in your samples (estimates only, not a forecast).**
  - Lanes+short at t=1200: `budget_protected/moved` is about 4.4, against 1.6 for the generic t=2200 sample.
  - The estimated saving per moved task there is about 49 weighted units, roughly 12 step-equivalents.
  - Under short-task preference the one-move budget is what removes most holders.

## Choice: (a), remaining-flow score with static lanes

**Why it is structural.**
- From your excerpt, the NMS macro score is a pure potential: `hm(end)`, the terminal-wait rotations, the goal-crossing override, then `*50 - op`. It charges nothing for the edges traversed.
- CGAR adds the paid forward surcharge on top of a score of the same form.
- At production units, turn 4 equals unit 4, so CGAR's turn extra is already zero.
- Under lanes, `remaining_flow=1` therefore makes CGAR's score exactly the NMS form.
- The factor changes what the field means: a routing potential robots may locally violate, against a cost paid on every contraflow step. It does not change a magnitude.

**Why it isolates.**
- It touches only the temporal score.
- The oracle, tables, scheduler pickup prices, work counts and RSS stay identical to the verified lanes arm.
- Any difference is therefore attributable to the score form.
- Options (b) and (c) cannot claim that.

**Why the generic null does not transfer.**
- Under the learned field only 1.5–1.7% of executed moves were tolled, so the paid term rarely bound.
- The static field puts a contraflow option beside a large share of vertical moves.
- Your lanes arm gained mostly at startup, with late rates equal to the learned field's. That pattern is what a field whose strength is not the lever under paid semantics would produce.

**Why before (b).**
- A change of contraflow ratio or bands also reprices scheduler pickups against a spatial chain. That is the confound noted in turn 30.
- Cheap turns (1 against forward 20) make `turn_cost - unit` negative, so the present cost form would reward gratuitous rotations.
- Cheap turns only make sense on top of a pure-potential score, so (a) is the semantic precondition for (b).
- A 10:1 ratio alone is reachable within current bounds: scale 1, forward 1, opposing 10, turn 1. It needs no oracle change.
- I would run that ratio arm only as the conditional follow-up below.
- Native 20/200 with +1 bands fits uint8. It still needs three things, all worth doing only after (a) wins:
  - the ≤16 edge-weight validation relaxed;
  - Dial bucket counts generalised, including the hard-coded 17 in the complete pickup field;
  - compact-table overflow re-audited.

**Why before (c).**
- Your sample does show the budget binding under short-task preference.
- But (c) needs new liveness work:
  - repeated retargets reset ticket, `best` and `stall` each time;
  - cooldown becomes the only limiter;
  - ticket handling across retargets would need changing under the selector.
- Its payoff is confined to the short-task line.
- Make it the follow-up on that line only if the calibration shows realized savings tracking estimates with too little volume.

## Safeguards

- **Selector.** Add a fourth explicit selector in `tricks::options`, for example `CGAR_TRICK_REMAINING_FLOW`. Print it in the components receipt. Keep `CGAR_TEMPORAL_REMAINING_FLOW` rejected under `--trick`.
- **Isolation.** First run only: lanes 1, short 0, matching 0. Keep the existing rejections of next-errand, the audits and branch work.
- **Cost path.** Assert at init that `weighted_forward()` is true under the static field, so the `flow_cost` path is taken. Otherwise the flag is a silent no-op. Print `paid_forward_extra=0`.
- **Regression.**
  - A static-field fixture where a legal macro containing one contraflow hop is chosen with the flag on and rejected with it off.
  - Scores checked against an independent replay.
  - Identical actions between arms wherever no candidate touches a contraflow edge.
- **No-op equality.** The flag-off lanes arm must reproduce your verified lanes trajectory exactly.

## Arms and rejection

- **Arms.** Two runs, lanes + remaining-flow on seeds 0 and 2, against the existing lanes rows.
- **Primary readouts.**
  - Tasks.
  - The final-1000 completion rate. The lanes gain was startup-heavy, and a score-form change should show in steady state.
  - Loaded overhead per completed task (loaded mean minus chain mean).
- **Mechanism readouts from the replay.**
  - Share of executed forward moves on contraflow edges.
  - Loaded detours, turns and waits.
  - Empty steps.
- **Reject** if both seeds show tasks at or below lanes-only, with no reduction in loaded overhead or the late rate.
- **If the contraflow share does not move,** the flag is not biting. Investigate; do not reject.
- **If the contraflow share rises and waits grow by more than detours plus turns fall,** the potential at 4:1 is too weak to hold lane discipline without the paid term.
  - Only that outcome justifies the single ratio arm (remaining-flow at 1/10/1).
  - Its new field hash must be printed in the receipt.
  - Pickup and chain estimates must be logged.
- **If the ratio arm also fails,** the paid term is not the mismatch, and the motion gap lies elsewhere.

## One pre-check on the NMS side

- Your excerpt has an `ENABLE_DHMR` branch that replaces the static `hm` with a per-robot heuristic.
- Confirm whether the archived 154,795 build defines it.
- If it does, the NMS potential is not the static lane metric. In that case even a win for (a) would not mean CGAR matches what NMS runs.
