I ran nothing. This is a read of the V68 delta, your replay tables and the NMS excerpt.

## V68 delta

I found no defect.

- The guard ordering is right.
- The generic selector still rejects `--trick`.
- Lanes-off, short, matching, next-errand, audits and branching are all rejected.
- Init fails closed if `weighted_forward()` is false.
- The components receipt carries `remaining_flow`.
- The independent macro replay matches the NMS excerpt on all three points:
  - The paid extra runs through the arriving edge.
  - Service uses the last hit.
  - Trailing waits use the free ±90°/180° minimum.

Two notes:

- The service branch is exercised only at the goal cell itself. That is adequate but thin.
- V68 forbids matching, so a V68 win still needs an explicit guard relaxation before it can join the 147,328 profile.

## Decision on the full V68 result

- **V68 at or above lane-only on both seeds, or within noise:** build the native cost model once and test it as a fidelity experiment. The spec is below.
- **V68 below lane-only on both seeds, with executed contraflow and waits both up:** run one cheap ratio arm first.
  - The arm is remaining-flow at forward 1 / opposing 10 / turn 1, scale 1.
  - It needs no oracle change, but it does need an explicit selector and a new field hash.
  - If it recovers to at least lane-only, build native.
  - If it does not, stop the NMS-metric line.
- **V68 below lane-only without that signature:** stop the line. The native profile depends on pure-potential scoring, and pure-potential scoring would then be what hurts.

Reasons native is worth one build:

- It is the last cost-model difference between two near-identical PIBTS kernels.
- More same-objective search did not help.
- Under lanes your loaded turns already equal the NMS diagnostic (5.94 against 5.80).
- The motion clues that remain are waits and detours.

Native separates three numeric relations that CGAR currently ties together:

| Relation | NMS | CGAR now |
|---|---|---|
| Contraflow : forward | 10 : 1 | 4 : 1 |
| Turn : forward in the potential | 1 : 20 | 1 : 1 |
| Maximum op tie term | 0.128 forward steps, 2.56 turns | 2.56 of either |

- The tie difference exists because CGAR kept the ×50 score scale while its distances are in step units, not NMS's 20-per-step units.
- The third relation is the scalar issue you flagged. Setting `unit=20` inside the present formula would keep the tie at 2.56 steps.

**Zero-host pre-check.** From the archived NMS trace, run the same replay for two numbers, and set them against your lane traces:

- the executed contraflow share;
- the share of waits and traffic in rows 3–10 and 125–137.

This is a direction clue only. It decides whether the band arm deserves hosts.

## Native profile, staged

The selector is `CGAR_TRICK_NATIVE_METRIC=1`. It is valid only under the CLI flag with lanes 1, and it rejects short and matching for the first isolation.

**Metric**

- Forward weight: w(u,d) = 20 + 180·[d opposes the lane at u].
- Arm N2 adds +1 for cells in rows 3–10 and 125–137, in all four directions.
- Turn cost is 1.
- Wait is unpriced.
- D(s) is the reverse shortest cost from an oriented state to the goal cell in any heading.

**Score.** This is the exact form of the excerpt.

- Let t* be the last slot at the goal cell. If t* exists, P = −t*, raw, not −20t*.
- Otherwise P = D(end).
  - If the last action is a wait, take the minimum with D at h±1.
  - If the last two actions are waits, also take D at h+2.
  - No turn cost is added for these rotations.
- S = 50·P − op, with op raw.
- There are no paid forward or turn extras, so the negative `turn − unit` extra never arises.
- This is a separate function from `flow_cost`.

**Planner plumbing**

- `guidance_turn_cost_` becomes 1 by explicit override, bypassing `turn_cost·scale`.
- The PIBT-pass wait cost stays at `flow_cost_scale_ = 20`.
- Learned flow stays disabled under lanes.

**Scheduler**

- It stays coherent by construction if `pickup_scale = 20` flows everywhere:
  - Chain = 20 × spatial chain.
  - Pickup costs come from the native oracle.
  - Matching thresholds are 4×20.
- This is an explicit simultaneous repricing: turns become nearly free and contraflow costs 10× in pickup costs.
- Log pickup and chain estimates, and expect cohort shifts.

**Arms**

- Arms: N1 (no bands) and N2 (bands), on seeds 0 and 2.
- Reference rows: the exact lane-only rows and the V68 rows.

## Safeguards

- **Default-exact behaviour**
  - Weights validate to [1,255] only under the native selector.
  - Every other profile keeps the ≤16 bound.
  - Dial bucket count is `max_weight + 1`, computed per profile. Default paths stay at 17 and bit-identical; native uses 202.
  - No-flag and lane-only fingerprints must reproduce exactly.
- **The three searches**
  - Reverse oracle Dial: needs the parametrised bucket count, and must assert the bucket count exceeds max(weight, turn).
  - Complete pickup field: replace the hard-coded 17 buckets and the `(infinity−16)/64` guard with the profile maximum.
  - Bounded pickup search: it is heap-based, so only its edge validation needs relaxing. Its pop bound is unchanged.
- **Positive weights.** Reject any free directed edge with weight 0 or above 255. Keep turn in [1,16].
- **Compact tables**
  - A value reaches 65,535 only with roughly 330 forced contraflow cells. Keep the lossless wide fallback.
  - The LRU already budgets wide logical bytes, so the table budget is unchanged.
  - RSS must still be measured.
  - Print the wide-fallback count in the summary.
- **Arithmetic**
  - The score is int64.
  - 50·D stays below about 4M for real distances.
  - `kFar`-based fallbacks times 50 still fit.
- **Work and deadlines**
  - State counts are identical.
  - Native Dial adds O(max distance) empty-bucket steps per table, which is small against 154k states.
  - Build limits and quotas are unchanged, and a timeout remains a failure.
- **Asset and receipt**
  - Regenerate the asset from the unmodified NMS guidance dump, with the band bit.
  - The receipt prints `forward=20 opposing=200 band=0|1 turn=1 score=pure_potential tie=raw`.
  - It also prints a hash of the full weight vector.
- **Liveness.** Pure potential has no paid-progress identity. Liveness claims rest only on the unchanged certificate, primary and recovery layer.

## Regression cases

1. The oracle on the real warehouse field at weights 20/21/200/201 and turn 1, against an independent priority-queue Dijkstra over all oriented states. Include goals inside the band rows and goals outside them.
2. Per-edge equality of the C++ field with the NMS dump.
3. Both pickup searches against independent relaxation at native weights, plus boundary cases:
   - Weights 1, 201 and 255 are accepted.
   - Weights 0 and 256 are rejected.
   - The Dial-invariant assertion fires on a deliberately small bucket count.
4. A long forced-contraflow corridor:
   - It must overflow the compact table.
   - Values must remain exact.
   - LRU byte accounting must be unchanged.
5. The score, replayed independently over all 129 macros at sampled states, with hand-counted fixtures for:
   - raw last-hit service;
   - one trailing wait;
   - two trailing waits;
   - a one-forward-step difference that must beat Δop = 128;
   - a one-turn difference that must lose to Δop ≥ 51, which documents NMS's relation.
6. A test-only port of `get_smart_dist_IMPL` written from the excerpt, compared on a small grid.
7. Generalised bucket code at maximum 16 reproduces today's tables and fields exactly.
8. Scheduler plumbing:
   - Uniform scaling to 20 with turn 20 leaves assignments identical.
   - Switching turn to 1 changes a hand-counted pickup choice.
9. The negative gates:
   - no CLI;
   - lanes 0;
   - short;
   - matching;
   - generic selectors;
   - turn surcharge.

## Readouts and rejection

**Readouts**

- Tasks.
- Final-1000 rate.
- Loaded waits, loaded detours and loaded turns.
- Empty work per task.
- Executed contraflow share.
- Completed-chain mean.
- Pickup and chain estimates.
- Wide-fallback count.
- Maximum decision time.
- RSS.

**Reject the NMS-metric line if both conditions hold:**

- Neither N1 nor N2 beats the better of lane-only and V68 on both seeds.
- Loaded waits plus detours per completed task do not fall.

If that happens, keep 4/16/4. The motion gap is then not in the cost model, and further metric variants would be tuning.

**If loaded waits or detours fall while empty work rises enough to cancel the gain:**

- The loss belongs to the scheduler repricing, not to the potential.
- Record that finding.
- It does not reject the motion result.
