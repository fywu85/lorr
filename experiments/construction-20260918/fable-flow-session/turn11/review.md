I read the supplied diffs and ran nothing. Two calls I made earlier were wrong. The cost-key shortlist I proposed was tested and is rejected. My concern that age-aware complete fields would lengthen empty travel had the wrong sign.

## Diagnosis

**Demonstrated bugs:** I found none in the motion path this turn.

**Designed approximations that touch loaded motion:**
- **Arrival scoring.** `TemporalGeometry::cost` sets `d = -t*unit` at the last step the robot is on its goal. Sitting on the goal through the horizon scores −4, while arriving and continuing scores the arrival time. The next errand is never scored.
- **Fallback metric.** Robots without an oriented table use the unit-turn fallback.
- **Tie term.** The `-op*unit` term reaches 2.56 steps.
- **Restart.** Every step restarts from all-wait unless warm start is on.
- **Worker selection.** Workers are compared on scalar score, with a strict `>` that keeps worker 0 on ties.

**Observation from your replay table (not a causal claim):**
- Extra forward moves per turn come to 0.79–0.82 in all four rows, including KK and the collapsed run.
- Waits per turn come to 1.36–1.48 in the three healthy rows.
- The three components rise and fall together. CGAR's counts in all three are roughly twice KK's.

**Inference on objective versus search:**
- Search breadth returned little under this objective: four workers gained 0.68%, and two-owner transactions lost 0.29%.
- Guidance and assignment returned much more: the refreshed field, table coverage, graded tolls, and the flow-aware age-aware pickup fields.
- The residual therefore looks like too many conflict and detour events being created. It looks less like weak resolution within five steps.
- That points to the objective or guidance level, but it is unproven.
- One arithmetic point for reading the turn count: a toll detour around a tolled corridor costs about 2 cells and 4 rotations. Part of the turn excess is therefore the price of the convention itself.

## Change 1 (untested): fractional turn price in the scaled metric

**Mechanism.**
- A rotation costs the robot one slot.
- It also holds a corridor cell for that slot, and the potential does not price that.
- A small surcharge makes the metric prefer the path with fewer rotations among paths of equal elapsed time.

**Synthetic example.**
- Route P has 10 cells and 3 turns. Route Q has 12 cells and 1 turn. Each takes 13 steps.
- At scale 4 with turn cost 4, the two tie, and the current code breaks the tie arbitrarily.
- At turn cost 5, P costs 55 and Q costs 53. The robot takes Q with no time loss and two fewer rotations in traffic.

**Opposing mechanism.**
- Detours that respect the tolls are rotation-heavy.
- A turn price therefore lowers the effective toll strength.
- The earlier coarse test, turn cost 2 without flow, was negative.

**Consistency checklist:**
1. **Parameter.** Add a surcharge in scaled units: oracle turn cost T = scale + e, with T ≤ 16. Keep physical `turn_cost_` at 1. Relax the `initialize` guard for this parameter only.
2. **Oracle.**
   - Pass T to `turn_oracle_.init`. `max_edge_cost_` and the bucket count already follow T.
   - The uniform-BFS branch is disabled whenever e > 0. Dial then runs before the first publication, so check build times in steps 0–127.
3. **Temporal scoring.**
   - The adapter's `robot_turn_cost` must be T for oriented robots.
   - With `extra = T - unit`, `cost()` already charges prefix turns and terminal-wait rotations.
   - Extend your paid-progress checks to (scale 4, T = 5 and 6) before trusting it.
4. **Fallback robots.**
   - They stay at unit turn cost by design.
   - The mixed basis that exists today for tolls persists.
   - Watch the fallback share.
5. **Wait-seed rotation and construction priorities.** Both read oracle values and are consistent automatically.
6. **Scheduler.**
   - `pickup_search_.run` and the complete fields must also receive T.
   - Otherwise `estimate()` mixes T-weighted cached tables with unit-turn searches inside one comparison.
7. **Bounds.** Compact 16-bit tables, the ≤ 16 limits in both pickup searches, and `kInf` margins are all satisfied for the warehouse.
8. **Exclusivity.** Guide routes stay mutually exclusive with this option.
9. **Neutrality.** With e = 0, the run must reproduce the reference hashes exactly.

**Minimal tests:**
- Independent repeated relaxation on small maps for several (scale, T) pairs with directed tolls.
- The paid-progress identity along weighted-shortest prefixes:
  - A turn that is on the route must net one slot.
  - An all-wait plan must gain exactly one slot through the terminal rotation discount.
- The P/Q fixture above, asserting the robot's first action follows Q at T = 5 and that the distances tie at e = 0.
- A scheduler basis-equality test on a map without pockets: the cached oriented table, the bounded search and the complete field must agree for one robot and one goal.

**Validation.**
- Run seeds 0 and 2 at T = 5 and T = 6, at the strength chosen by your in-progress matrix.
- Read the loaded replay per task: turns, waits and extra forward moves.
- Add a count of traversals against tolled edges.
- A beneficial signature is fewer turns, with waits and against-toll traversals not rising.
- If against-toll traversals and waits both rise, the turn price is undercutting the convention. Reject it regardless of the total.

## Change 2 (untested): mixed warm/cold worker portfolio

**Code basis.**
- `temporal_adapter.hpp` constructs every worker with the same `initial` plan, or with none.
- All four workers are therefore either warm clones or independent cold restarts.
- With cold workers, the selected plan can alternate between equal-scoring optima on successive steps.
  - Example: at a symmetric merge, either robot a or robot b goes first.
  - Alternation makes both robots execute start-then-yield turns and waits.

**Change.**
- Warm-start worker 0 from the previous accepted plan and keep workers 1–3 cold.
- Selection already uses a strict `>`, so worker 0 wins exact ties. Continuity is preferred exactly where the objective is indifferent.
- There is no added work.
- By the objective, the result is never worse than the best of the three cold workers.

**Evidence so far, not additive by assumption.**
- Warm start alone gained 0.28% on two seeds.
- Four cold workers gained 0.68% on two seeds.

**Validation.**
- Compare against the four-worker control on seeds 0 and 2.
- Read from the replay: the share of turn reversals, and turns and waits per loaded task.
- Confirm all decisions are valid and the control hashes are exact.
- If reversals do not fall, the premise is wrong whatever happens to the total.

## One const audit before touching arrival scoring

- In `audit_forward_blockers`, flag owners whose conflicting reservation is a sit on the goal cell after arrival.
- Report that count beside stationary roots.
- If it is negligible, drop continuation scoring permanently.
