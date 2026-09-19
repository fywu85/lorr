## Retractions

- **Rank-weight starvation is withdrawn.** With `CGAR_TEMPORAL_EQUAL_WEIGHT=1`, `power` is 1 for every active robot. My "far robots weigh ~1/n" argument applies to none of the compared runs.
- **"Never un-tolled" is withdrawn as a code invariant.** Your regression shows fields can go neutral and reverse. I also predicted refresh would not help seed 0, and that was wrong.
- **Collapse onset inferred from p90 age is withdrawn**, because that age includes pool wait.
- **"Search is saturated" is downgraded** to: the search changes you tested gave modest gains.
- **"A jam only the primary drains" is contradicted.** Almost no robot is motionless in the late window.

## Revised diagnosis

**Observed motion.**
- In steps 4001–5000 of flow seed 2, actions are 27% forward, 47% wait and 27% turn.
- Immediate turn reversals are 28% of turns (740k of 2.65M).
- In every healthy run, flow or not, that share is 7–8%.
- This describes the motion and does not establish a cause.

**Two generators fit and are not yet separated:**
- **Tied-heading wait-seed rule.** It emits exactly "opposite turns with no forward between", and it scales with blocked robot-steps. There are 7.3M wait-plus-turn actions in that window.
- **Receding-horizon plan churn**, where each step's fresh plan undoes the previous one.

Your split rotation counters will decide between them. The tied-heading rule may be more than a counting artifact here: a robot at the head of a queue is mis-oriented half the time when its slot opens, which lowers queue discharge, and that matters only under congestion. So test the strict-improvement option on frozen seed 2 as well as seed 0.

**Seed-0 matrix.**
- All three margins use the same 128-step counts, so the edge sets are nested: margin75 ⊂ margin50 ⊂ margin25.
- Dropping the 75–87.5%-dominance edges (margin75) costs about 3%, and doubling the toll (strength 2) also costs about 3%.
- Adding the 62.5–75%-dominance edges (margin25) costs 55%.
- On a good field, damage comes from which edges are tolled, specifically the genuinely bidirectional ones, and not from toll size.
- I expect v31 scaling to move a good field by only a few percent. Whether it rescues seed 2 is open.

**Field overlap.**
- Fields 0 and 1 share only about half their edges, and both work.
- Field 2 is no farther from field 0 than field 1 is.
- So no particular field is required, and gross overlap does not mark the bad one.
- Subtle field structure versus metastable dynamics remains undecided.
- The strongly-connected-component result rules out disconnection only, as you say.

## Refresh512 (+9.45% on seed 0)

Yes, it changes my recommendation. Priority shifts from dissecting the frozen collapse to finding out why refresh helps.

**Candidate mechanisms:**
- **A better field.** Refresh has more samples, and its counts come from traffic that is already tolled. The field may approach a fixed point of the traffic it induces, so few robots pay tolls and local slack survives. This is a hypothesis.
- **Periodic cache flushes.** Each changed publication clears the oriented tables, giving temporary toll-free fallback heuristics.
- **Non-stationarity.** A changing field spreads load over time.

**The one control to run first: field transplant.**
- Reconstruct a late seed-0 refresh512 field offline. Your reconstruction tool already matches production counters.
- Load that field at sample 128, freeze it, and publish nothing further. That gives one flush, exactly as in the 122,896 baseline.
- Compare windows 2–5 against about 28.0k (refresh) and about 25.2k (frozen).

| Transplant result | Meaning | Next step |
|---|---|---|
| About 28k | The field alone is sufficient. Flushes and non-stationarity are unnecessary. | Load the mature field into seed 2's RNG as the robustness test. Then ask offline what separates mature fields from 128-step fields: share of traffic paying tolls, corridor coherence. |
| About 25k | The refresh process matters. | Run a flush-only control: unchanged frozen field, forced invalidation at the same publication steps. This separates flushes from non-stationarity. |
| In between | Both contribute. | Same flush-only follow-up. |

**Free checks on the existing refresh run:**
- Read `exact_metric`/`fallback_metric` in the steps after each publication to see how long the fallback period lasts.
- Count completions per 50 steps, phase-locked to publications. A sawtooth would implicate the flushes.
- Count edges changed per publication to see whether the field converges.

**Pending seed-2 refresh512 run:**
- A rescue would support the better-field reading.
- A collapse would mean the churn phase is reachable regardless. Churning moves would then also be feeding the cumulative counts.

## Branching displacement

- **It is unlikely to fix rolling-horizon churn.** Branching improves solution quality within a step but does nothing for consistency between steps. A stronger per-step optimizer can flip between near-equal optima more often, and under fixed 4M work it trades attempts for depth.
- **Add a hash-neutral attribution counter first.** For each robot that ends on the wait seed, record why its better candidates failed:
  - `blocker == -2`;
  - a fixed blocker;
  - chain failure or budget exhaustion;
  - rejection by `consider()`.
- Split the counts by healthy versus churning windows. Build branching only if `-2` rejections dominate in the churning regime.
- **Warm start is the existing lever for consistency.** Its small no-flow gains do not predict its effect where reversals are four times higher. Frozen seed 2 is the informative case.

## Order of work

1. Run the field transplant.
2. Collect the seed-2 refresh result, the split rotation counters, and the `-2` attribution.
3. Run the flush-only control only if the transplant does not reproduce the gain.
4. Tune after that.
