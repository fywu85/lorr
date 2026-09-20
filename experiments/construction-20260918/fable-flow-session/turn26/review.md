I ran nothing. The predictions below are uncertain and I label them as predictions.

## 1. Safety, liveness, overflow and sign

**Safety, liveness and overflow.** I see no issue on any of these.

- The option removes a nonnegative term, so score magnitudes shrink.
- Collision validity never depended on scores.
- Fallback-metric robots already carry no surcharge.
- The protected primary does not rest on the paid-progress identity:
  - Its progress test uses the spatial potential and stall counters.
  - Recovery uses exact spatial tables.
  - It is pinned in the temporal layer with a single choice.
- The certified PIBT pass keeps its own paid comparison in `pibt_candidates`: remaining cost plus edge cost plus turn cost. Your option does not touch it.

**The sign property to state explicitly.** Removing the paid term does more than reward expensive edges that lie on the optimal path.

- A move over an edge costing U + τ, with regret δ, lowers the potential by U + τ − δ.
- Without the paid term it beats an optimal untolled move whenever δ < τ.
- With the paid term it loses whenever δ > 0.
- So the option refunds the toll locally. A robot accepts up to τ of regret per tolled move to use that edge.
- The toll still shapes routes through D.
- In the summed objective, a robot crossing tolled edges also wins conflicts. Its slot is worth U + τ against everyone else's U. That is the evacuation effect you described.
- Under your graded field, τ is at most one step.
- This is the intended change of surrogate, not a bug. It should be pinned in a fixture so it is never mistaken for one.

**Audits and transactions.** Their "physical" quantity, cost plus op × U, stops meaning paid progress under this option. Label their outputs accordingly.

## 2. Is the factor redundant?

**No.** Per-agent constants cancel in the gain. Examples are the seed's potential or a common baseline. The surcharge is path-dependent, so it changes two things:

- the ranking among one robot's candidates;
- every sum comparison in construction, repair and regional acceptance.

**How often it can matter.** It matters only for candidates that cross tolled directions.

- Your replay's `tolled_forward_moves` over `learned_active_forward_moves` gives the executed share. I expect a few percent. That is a prediction.
- The executed share understates the effect. Many rejected candidates are rejected only because of this term, so tolled traversals should rise once it is removed.

**My prior for the generic run is neutral to slightly negative.**

- Every earlier weakening of deterrence in the learned field lost:
  - scales 2 and 4 at strength 1;
  - strength 2 was mixed;
  - broader margins were unstable.
- NMS keeps its corridor minority share at 0.18% without a paid term. It does so under a 10:1 cost ratio, where D itself routes almost everything with the lanes.
- CGAR's learned ratio is at most 2:1, so that inference does not transfer.

## 3. Fixtures, comparison, and whether something better exists

**Minimal fixtures:**

1. **Neutral equivalence.** Before the first publication, and under an unweighted field, all candidate costs and actions are bitwise identical with the option ON.
2. **Hand-counted refund pair.** On a small directed-toll grid, take three one-step alternatives from one state:
   - an optimal untolled move;
   - a tolled move on an equal-total route;
   - a tolled move with regret below τ.

   By default the first two tie, the op term decides between them, and the third loses. With the option ON, both tolled moves win by exactly (τ − δ) × scale.
3. **Independent oracle.** For every operation from sampled states, the option's cost equals the default cost minus the surcharge times the scale. This covers the goal-hit branch as well. The turn extras and the op term are unchanged.
4. **Rejected combinations.** Next-errand scoring and guide routes are rejected. Decide the trick combination explicitly, because under `--trick` this option becomes the NMS-semantics factor and belongs to that frontier.
5. **Existing regressions.** Run the primary, recovery and capacity regressions with the option ON. Check serial against parallel preparation. With the option OFF the run must be byte-equivalent.

**Causal comparison.** Run seeds 0 and 2 with the option ON against exact controls. Read these:

- totals and per-1000-step windows;
- the final-1000 rate;
- loaded turns, waits and detours per task;
- empty robot-steps;
- the tolled-move share;
- the corridor minority share from your replay, computed per epoch;
- the maximum entry time.

How to read the results:

- **Beneficial signature:** waits fall and the minority share does not rise materially.
- **Harmful signature:** the minority share and waits both rise. Reject on that even if the total looks flat.
- Go to six seeds only if both seeds improve.

**Is there a clearly better bounded generic operator?** I do not see one. This one is cheap and cleanly falsifiable, so run it.

**A free measurement comes first,** on the runs you already have. The TRICK gain is a fixed total, with tails of 29.9k against 30.0k and 29.86k. So:

- Difference the TRICK and generic runs window by window over the first 1,000 to 1,500 steps.
- That sizes the early-phase opportunity. The early phase spans:
  - steps 0–127 with no field at all;
  - then a noisy first field until step 640;
  - the cache-flush dips.
- If most of the roughly 2.3k-task gain lies before the second publication, the generic analogue is a candidate experiment: making the learned field usable earlier.
- If the gain is spread evenly, it is not.
- Do this before designing anything.

## 4. What the TRICK tail says

**The guidance-only explanation of the steady-state gap is weakened.**

- NMS's lane directions ran at your 4/16 costs, under paid scoring, inside CGAR's scaffold.
- They did not raise the late completion rate.
- So in steady state the learned field does about as well as that layout, and the persistent advantage is an early-phase effect.

**What that early effect consists of:**

- a field from step 0;
- no publication flushes;
- pickup quotes from tick 1.

**This does not make guidance irrelevant.**

- These NMS elements are untested:
  - the 10:1 ratio;
  - near-free turns;
  - potential-only scoring;
  - bands.
- The lanes cut loaded turns by about 36% and waits by about 13%.
- Empty robot-steps rose about 6%, and that cancelled the gain.

**Interpretation.**

- That cancellation is a whole-system interaction.
- One plausible reading is that one-way layouts lengthen the far pickups that age-aware dispatch favours. Both competitors pair their guidance with pickup-dominated assignment that rematches tasks.
- I do not claim that assignment dominates. Cohorts, semantics and budgets all differ.

**What follows.**

- Guidance and assignment changes may need to be evaluated jointly at some point.
- A future TRICK arm should report empty work alongside loaded overhead before anything is concluded from totals.
