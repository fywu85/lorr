## A. Seed-2 collapse

Tags: **[F]** fact, **[C]** inferable from the supplied code, **[H]** hypothesis.

**[C] The metric alone cannot stall a robot.**
- Every edge stays usable and `compute()` is a complete Dijkstra.
- `cost()` plus `forward_surcharge()` is paid-progress consistent, so every non-goal state has an action that lowers D by exactly its price.
- Stalls are therefore multi-robot: a jam nucleates and is never dissolved.

**[C] Why a jam persists once it forms (with or without flow):**
- D is static and blind to congestion. Joining a queue tail scores as full progress until the robot is physically blocked.
- `FlowGuidance::observe` skips `u == v`, so blocked robots emit no signal. Crossing counts measure throughput, never delay.
- `power` is remaining-distance rank with no aging, so far or rerouted robots weigh about 1/n.
- `consider()` is effectively greedy after about 5k repairs, because the temperature starts at 1e-3 and decays by 0.999 per step while loss is measured against the global score. Backing a platoon out costs many robots and pays off beyond 5 steps, so it is never accepted.
- Only the single certified primary drains the jam. Liveness holds while throughput does not, which reconciles "protected progression survives" with 3.6k tasks per window.

**[F→H] Flow adds the nucleation risk.**
- I assume the seeds differ only in planner RNG; correct me if not. The no-flow spread is 0.06% while flow is bimodal. The variance therefore enters through the one seed-dependent object flow adds: the field frozen from 128 noisy steps.
- At strength 1, `extra` is always 1, so the field is binary and the margin is the only selector.
- [F] Seed 2 is already −2.9% in window 1 and −5.6% in window 2, so it is inferior from step 128 rather than a late accident. Its p90 age of 3170 ≈ 5000 − 1830, so a stuck population formed near onset and never cleared.
- [F] warmup1024/margin50 is about equal to no-flow, while warmup128 gives +12% in the good seeds and margin 0/25 collapse on seed 0.
- [H] Long-run demand is near-symmetric. The 128-step field is mostly symmetry breaking, not learned asymmetry. It is corridor-coherent because each traversal votes along a whole corridor.
- [H] A quasi-random orientation has no balance guarantee. Nothing balances untolled directed capacity across cuts or alternates adjacent corridors. A bad draw, or tolling genuinely bidirectional edges at low margin, funnels return traffic into few lanes and causes spillback.
- You can check this now by comparing `penalized_edges` across warmup128, warmup1024 and margin 0/25 in the existing logs.
- [H] One mechanism would unify your negative results (guide routes, turn costs, low margins, terminal multiplier): each removes the unit metric's degeneracy. Under unit costs most robots have several zero-regret moves, and the local resolver depends on them. Tolls and routes turn sidesteps into full-step regrets, so conflicts resolve by waiting on favored paths.

**Unproven:**
- where the jam nucleates;
- whether the collapse follows the field or the RNG;
- whether the stuck set is a true cyclic deadlock or a slow phase;
- the scheduler's contribution (pool-wait versus held age);
- whether the turn counts mean anything, given the confound in B.

## B. Pending interventions

**v30 (cumulative refresh).**
- It can repair only small-sample noise.
- [C] It is a ratchet. The tolled minority direction is used less, so dominance rises and the edge is never un-tolled. Polarization spreads along corridors, and jams stay invisible because non-moves are not counted.
- I predict `penalized_edges` will grow per publication toward the margin-25/0 regime that collapsed on seed 0.
- [C, verify] Every changed publication hits `tables_.clear()`, while rebuilds appear to be budgeted (`turn_table_budget_ = 32`, prefetch ≤ 32). I have not seen the code that spends that budget.
- If so, part of each interval runs on the toll-free `fallback_distance`. Read `exact_metric`/`fallback_metric` after publications, and add a control that flushes the caches with an unchanged field.

**v31 (B scaling).**
- Pre-freeze trajectories are B-invariant, so B ∈ {1, 2, 4, 8} plus strength 2 is a clean dose–response on an identical field per seed. That is more informative than v30.
- It can fix over-long detours and local rigidity. At B=8 a toll is 50 score units, against 400 per step and 8 per op index. Local choice is then nearly toll-free while D still prefers with-flow routes.
- It cannot fix tie-break concentration (any positive toll fully orders equal-length routes), a bad orientation draw, or the blindness to stalls.
- The B=8 result discriminates as follows:
  - Seed-0 gain kept and seed 2 rescued: the gain is lexicographic route preference and the collapse is toll magnitude.
  - Gain lost: real detours are needed.
  - Still collapses: the cause is field structure or concentration.

**Neither v30 nor v31 addresses** stall blindness, orientation balance, or the greedy objective with no aging.

**Concrete finding in `temporal_adapter.hpp`, wait-seed block (`search.selected(i) == 0`).**
- The expression `best == right ? CR : best == left ? CCR : W` sends ties to rotation. The guided branch has the same rule.
- **Counterexample:** a robot faces east with its goal at (+2,+2). East and south are blocked and the wait seed is selected. D(east) = D(south) = 5 and D(north) = D(west) = 6.
  - Step 1: right equals wait, so the robot issues CR.
  - Step 2, now facing south: left equals wait, so it issues CCR.
  - This repeats indefinitely and W is never issued.
- **Consequence:** blocked robots with tied headings log one turn per step. Seed 2's 7.2M turns are therefore not independent evidence of a rotation pathology.
- The same applies to the drop in turns from no-flow to flow (3.68M to 2.70M at near-equal waits), since tolls break heading ties.
- The throughput effect is unproven. It is roughly neutral in expectation, but it sits at intersections.
- **Fix:** rotate only on strict improvement. That changes trajectories, so it needs a matched rerun. At minimum, split the counter into seed rotations and planned rotations.

## C. Three experiments

1. **Field × RNG swap plus an offline field audit.**
   - Dump `costs_` at the freeze and load it by env var. Run (RNG 0, field 2) and (RNG 2, field 0).
   - If collapse follows the field, the bad-draw hypothesis holds. The audit must then separate field 2 from fields 0 and 1.
   - Audit statistics to try:
     - untolled directed-capacity balance across each row/column cut;
     - orientation sign flips along degree-2 chains;
     - the share of tolled edges with total < 16;
     - co-location of tolled structure with the jam nucleus.
   - If collapse follows the RNG, the dynamics are metastable regardless of field.

2. **Toll-removal hysteresis on seed 2.**
   - Use a deterministic replay and publish a neutral field at T=2000 and at T=3000, each with a flush-only control.
   - Log per-cell blocked robot-steps per 250 steps.
   - Log stuck-set composition: rank percentile, with or against flow, pinned fraction, and proximity to pocket/txn cells.
   - Log pool-wait versus held age.
   - If throughput recovers to about 22k per window within about 500 steps, the field sustains the jam, and decaying or adaptive tolls are viable.
   - If it does not recover, the base objective has a collapsed attractor it cannot leave. Prevention margin and an aging or retreat mechanism become mandatory.

3. **Mirror test.**
   - On seed 0, keep the same edge set but toll the dominant direction instead. The change is small: swap the forward/reverse roles in `observe`.
   - If the gain stays near +10%, direction is arbitrary and the gain is symmetry breaking. In that case stop learning and construct a balanced orientation from graph topology: generic, seed-independent, with a small toll.
   - If the gain is lost or the run collapses, crossings carry real directional information and deserve better statistics.

**Abandon the learned crossing-count branch if either holds:**
- the mirror test passes, because learning is then noise and construction replaces it; or
- collapse follows the RNG, there is no recovery, and B=8 still collapses. A static toll field is then unsafe at this density without delay feedback.

## D. Assessment

**Containment of PIBT says nothing about throughput.**
- It lower-bounds nothing useful, because the target sits about 40% above the full system.
- It helps only if the searched objective is throughput, and it is not. The objective is a 5-step, rank-weighted progress sum over a congestion-blind potential.

**Search is saturated.**
- Portfolios gave +0.55%, regions +2.7%, and temperature and warm start about 0.
- A crude, noisy binary field gave +12%.
- The binding constraint is the information in the objective, not optimization power, so I would stop search-side work.

**What is missing is persistent shared traffic organization:**
- a convention that removes head-on conflict, which can be constructed and need not be learned;
- delay or density feedback, because FlowGuidance observes the wrong quantity;
- aging in `power`, so that blockers eventually win instead of starving.

**Architectural constraint.**
- 8 GiB of cached reverse tables make D effectively static, because `set_forward_costs` flushes everything.
- 0.86–0.90 s of each second is already spent.
- Congestion feedback therefore cannot live in D. It has to enter the local candidate score or a coarse routing layer, at the cost of exact paid-progress consistency.

**Guide routes.** Four variants at −60% to −75% is not a tuning problem. They delete the local slack the resolver depends on, and I would shelve them.

**The KK comparison.** KK's 1.08 travel ratio, if measured identically, shows that near-free-flow exists at this density. It was reached with a larger budget (16 physical/32 logical CPUs, 38.9 GB RSS against your 32 GB limit), so it indicates a direction and not an equal-resource bound.

For the next turn, please send `penalized_edges` per configuration, the exact/fallback counts after each publication, the split turn counters, and the v31 ladder results.
