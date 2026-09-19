I read the supplied diffs and ran nothing. I found no reproducible correctness bug in `OrientedPickupSearch::run`, the scheduler integration, or the metric lifecycle.

**The search is correct:**
- Min-heap ties break on state index.
- Stale entries are skipped but still count toward the pop bound.
- All costs are at least 1, so the first settled heading of a cell is that cell's minimum.
- `visit` only ever sees settled cells.
- Generation stamps make the scratch reusable after an exception. Your regression covers that.
- A Timeout passes through unchanged.
- `turn_cost_ * flow_cost_scale_` cannot exceed 16, because your configuration forces unit turns whenever the scale is above 1.

**The lifecycle is consistent:**
- `set_forward_costs` swaps the cost vector and flushes the tables together inside `plan()`.
- The scheduler therefore sees a matched snapshot (costs, tables and scale) from step p or earlier.
- Behaviour before the first publication is exactly the old scheduler.

**The units are consistent:**
- The pair cost is pickup weight × scaled pickup distance plus scale × chain.
- The HRRN age term is scaled the same way, so the ratio does not depend on the scale.

My turn-08 estimate of +0.2–0.4% for the batch policy was not borne out, because your six-seed result was −0.085%. I will not reuse that coverage arithmetic.

## Findings

**1. `estimate` lambda in `Cgar::schedule`: mixed bases inside one comparison. This is an approximation with a systematic sign.**
- **Where it shows up:** fallback sampling and global samples compare several tasks for one robot.
- **How the bases differ:**
  - A cached oriented value prices a task with heading, tolls and the certificate domain.
  - Every other task is priced at unit distance or Manhattan distance times the scale.
- **Why cached tasks are penalised:** oriented tables are admitted for robots' current goals, so free-task pickups rarely have one. The cached minority is over-priced against the uncached majority.
- **Minimal case:**
  - The robot faces east.
  - Task A is 10 cells west along an edge tolled against westward travel, and its table is cached. It costs 2 turns plus 10 moves at cost 2, which is 22.
  - Task B is 12 cells west in the same situation, and its table is not cached. It is priced at 12.
  - `better` picks B, although B would cost at least 26 on A's basis.
- **Scope:**
  - Fair admission compares one task across robots. That is one basis except for robots with unreachable table values.
  - Global samples are off by default.
  - Fallback sampling runs only after an empty local search.
  - The main search shortlist is one basis and is unaffected.
- **Action:** Read `fallback_assignments` and the cached and approximate estimate counters. If fallback sets are material, use the approximate basis for every member of a cross-task set. Keep oriented values for fair admission only when every compared robot has a finite value.

**2. `candidates` lambda: what the 2×2 can and cannot show. This is a measurement issue, not a bug.**
- **HRRN dilution.** Under HRRN the final pick among the 16 is dominated by age, so the flow metric mainly changes which tasks make the shortlist. A result near zero in the HRRN1 arm is expected from that structure. It should not be read as "alignment is irrelevant". The HRRN0/pickup5 arm is the informative one.
- **Changed reach.**
  - The new search has 8,192 pops including stale ones, spread over four headings.
  - That settles roughly half as many cells as the old 2,048-node BFS did, by my estimate.
  - It settles them in cost order instead of hop order.
- **Counters to compare across arms first:**
  - `pickup_flow_cells` per search;
  - `pickup_flow_limits`;
  - `empty_searches`;
  - `fallback_assignments`.
- **`estimated_pickup_cost`.** It now includes tolls and turns in the enabled arm. Do not compare it across arms. Use the travel audit.

**3. Work per call has no total cap. This is a hypothetical exposure.**
- **Shape of the work:** free robots × 1–2 searches × `pickup_flow_nodes_`.
- **Steady state:** about 28 free robots per step gives at most about 0.5M heap pops.
- **Failure mode:** a mass-free event after the first publication would reach 10⁷–10⁸ pops and fail the entry.
- **Current risk:** I see no evidence the current instance can produce such an event.
- **Deterministic guard (uses robot order, not the clock):** beyond the first N robots in rotation order, use the BFS branch.
- **Cosmetic:** `limited` can be true when only stale entries remain in the heap.

## One bounded next experiment

**Measured pattern:**

| Configuration | Seed 0 | Seed 2 |
|---|---|---|
| Graded margin 0 | 56,537 (late collapse) | 139,796, your best single total |
| Direct-cost pickup weight 5 (build limit 32) | best direct-cost total | collapsed |

Across these two policies, wider or stronger settings raise throughput in the healthy state and also raise collapse risk. The field is still the only lever that has produced gains, and the collapse risk is what stops it being pushed further.

**Hypothesis:** the margin-0 collapse on seed 0 is preceded by higher field churn than in the healthy runs. Churn is the fraction of tolled directed edges that flip sign, appear or vanish between consecutive publications, bucketed by dominance. At margin 0, sign noise on nearly balanced edges keeps re-orienting conventions.

**Offline test, current information only, using your reconstruction tool on saved runs:**
- Compute churn per publication for graded margin 0 on seeds 0 and 2, and for graded margin 25 on both seeds.
- Reject the hypothesis if seed-0 churn before the throughput decline is within about 10% of seed 2's.

**If the offline test is not rejected:**
- Implement sign hysteresis: an edge keeps its published sign until the opposite dominance exceeds the margin.
- Rerun graded margin 0 on seeds 0 and 2.
- Pass: seed 0 finishes its final 1000 steps at 28k or more, and seed 2 retains at least 139.3k.
- Anything else rejects the change.
