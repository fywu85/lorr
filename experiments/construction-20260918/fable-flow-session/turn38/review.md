I ran nothing. This is a read of the V81 and guard deltas, plus your verified tables.

## Correctness observations

**V81 backward Dial.** Correct by reading.
- `current_bucket` stays congruent to `distance`.
- Every edge is positive and below `bucket_count`, so one conditional subtract reproduces the modulo index.
- The pop order is unchanged: same bucket, same LIFO.
- The deadline check sits where it did before.

**One hardening for V81.**
- Under the old modulo, an undersized `buckets` argument produced wrong distances silently. Under the new arithmetic it writes out of bounds.
- Add one check at function entry, once per table and not in the hot loop. It should throw `std::logic_error` if `bucket_count <= max(max_edge_cost_, turn_cost_)`.
- The V77 pickup Dial is already safe. It validates every edge against `cost_limit`, and it sizes `bucket_count` from that same value.

**Short guards.**
- Only the two intended relaxations changed.
- The 24-case matrix and the four native matching cycles cover activation and the protections.
- One gap remains: no closed-loop episode runs native with matching ON and short ON together.
  - Your 128-tick episodes leave matching unset.
  - The matching fixture is a 3-robot, two-call test.
- One 24-robot episode variant would close the gap cheaply before the full runs.

## Evidence and conjecture

**Verified.**
- Native-bands, unmatched, ordinary fairness:

| Metric | CGAR | NMS diagnostic |
|---|---:|---:|
| Empty work per task | 62.5 | 62.0 |
| Loaded overhead per task | 16.1–16.4 | 16.8 |
| Completed chain | 242.3 | 234.8 |

- With your caveats, the chain is the one large unmatched clue that remains.
- In your legacy runs, short preference lost through empty travel.
  - Matching64 recovered most of that loss, and weight 5 recovered most of the rest: −6%, then −3%, then −0.4%.
  - The binding constraint on the short line was retargeting capacity, not chain pricing.
- Active resident-chain pricing lost in the generic runs. Whole-chain coverage there was only 54%, and the quote/native ratio was 1.016–1.023.

**Conjecture.** None of this necessarily transfers to the native metric. The three hypotheses relevant to (B) are:
- Whole-chain coverage under static lanes should be near complete, because tables are never flushed.
- The resident quote should be nearly exact there, because the heading relaxation costs at most 2 units per leg against 20 per step.
- The spatial×20 surrogate should differ from the native chain mostly by a near-uniform ratio, with small task-specific residuals from lane detours, bands and contraflow. A uniform ratio only rescales the pickup weight.

## Ranked diagnostic: (A) first, attached to runs you already plan

Attach a budget-inclusive matching shadow to the four top-arm runs: native-bands + matching64 + weight 5, with short OFF and ON, on seeds 0 and 2. It needs no dedicated hosts. It addresses the mechanism that actually limited the short line.

**Spec.**
- Every 100 ticks, run a separate pass after the real pass has committed.
- Give the shadow its own cursor.
- Group all resident holders, including the budget-protected ones.
- Solve each group and apply the same cycle thresholds as the real pass.
- Commit nothing, and touch neither `reassigned_tasks_` nor cooldowns.
- Use only table peeks.
- Both runs must reproduce the unshadowed fingerprints.

**What to log.**
- Accepted shadow cycles that contain at least one budget-protected row.
- Their saving divided by 20, and the number of rows moved.
- Count each task at most once per task-life, using a shadow-only id set. This avoids the repeated-opportunity double count you flagged.
- Cycles with no protected row are work the real pass would do anyway, so exclude them.

**Minimum evidence for a budget-2 policy trial.**
- Compute S2, the deduplicated excluded saving per assignment.
- Compare S2 with S1, the accepted first-move saving per assignment in the same run.
- Convert S2 to steps with that profile's own measured yield, which is the verified empty-work change per unit of S1 between matching OFF and ON.
- Read the converted number as an order-of-magnitude indicator, not as a causal estimate.
- Trial budget 2 only if two things hold:
  - The converted S2 is comparable to the short arm's deficit against the fair arm, expressed in steps per task.
  - S2 is not small relative to S1.
- If S2 is a small fraction of S1 in both arms, drop (c).

## (B) is second

- Its ceiling is small. A ratio-only effect is equivalent to changing the pickup weight, which you are already testing. Task-specific residuals only matter in near-ties.
- Run it as a shadow only if short shortens chains by less than legacy short did, where the chain fell about 7 steps.
- A policy trial would then need three things:
  - near-complete whole-chain coverage;
  - a rate of quote-specific first-choice flips (your mode-2-only and both-different categories) that is a material share of assignments;
  - a predictive gain over spatial×20 clearly larger than the generic audit's 5.5% MSE gain, because that level produced no throughput.

## 8M single-worker continuation

Not warranted now.
- Native loaded overhead is already at or below the NMS diagnostic.
- Every earlier extra-search variant lost or tied.
- The maximum decision time is already 1.017 s.
- A read-only score-versus-attempts curve would change this view:
  - Sample the whole-plan score after 1M, 2M, 3M and 4M attempts at sampled ticks.
  - Your repair audit already records what is needed.
  - The curve would have to show that the final million attempts still contribute a material share of the improvement.
- If that curve is flat, skip 8M.
