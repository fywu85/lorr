I ran nothing. This is design advice from the verified V86 facts and your audits. The queued ON audits should decide the choice, and I am not predicting their content.

## Order: test a calibrated margin tier before shortest-first

**Reason 1: what the trigger actually selects.**
- The first reversal came at tick 3843 / 3828, which is R ≈ 1,160, about 3.6 task cycles before the end.
- Latching on the first impossible pair, or on the first reversal, fires at about the same place.
- From that point, "ascending spatial LB inside the feasible tier" is a short-preference policy lasting about 1,160 ticks.
  - It uses pickup weight 1 in spatial units.
  - It ignores lanes.
- In V80 the weight-1 short arm was the worst steady-state variant, at −3,645 / −3,669. Only its first window improved.
- A latched shortest-first would therefore test endgame cherry-picking, not slack. Its result could not be attributed to the slack audit.

**Reason 2: packing direction.** This is a structural argument, not evidence.
- In the last cycle each robot finishes at most one more task.
- Feasibility is roughly nested: a robot with more remaining time can do whatever a later robot can.
- Robots arrive in order of decreasing R.
- For nested one-task-per-agent matching, "largest that fits" maximizes the count. Shortest-first spends the scarce short stock on robots that did not need it.
- Shortest-first is robust to slack but points the wrong way on stock.
- Largest-fit points the right way on stock, but without calibration it picks exactly the pairs with LB ≈ R.
- A margin tier takes no position on packing. It only removes admissions that are wasted on average. That gives the smallest footprint and the cleanest attribution.

**Shape of the margin.**
- Your table has mean duration − LB of 13.3, 15.1, 18.4, 22.8, 28.9 across the LB buckets. That is closer to additive than proportional.
- A single cumulative ratio would under-predict the short tasks that matter at the end.
- Use prospective running means of (duration − LB) in those same five LB buckets, or an online fit a·LB + b.
- Calibrate only on single-holder tasks whose completion has already been observed. Mid-run completions are essentially uncensored.
- Keep three tiers, each with the unchanged HRRN/cost/id order inside:
  - first, LB + m(LB) ≤ R;
  - second, LB ≤ R < LB + m(LB);
  - last, impossible.
- This stays a heuristic. A mean margin still leaves a boundary pair near even odds.

**Go condition.**
- Count the ON run's feasible-tier late admissions that did not finish.
- The test is worth running if that count is a meaningful share of the 1,181 gap, and many of those admissions had an unassigned alternative at that tick whose LB + m ≤ R.

## If you run shortest-first anyway

**Trigger.**
- Neither latch isolates slack.
- The less misleading of the two is "first impossible pair".
  - It depends only on data the scheduler sees.
  - It does not depend on sort or heap internals, or on shortlist composition.
- A cleaner variant needs no latch.
  - Compute the mode once per entry, in the task loop: "some pooled task has chain + repeats + 1 > R".
  - It is robot-independent and fixed before any comparator runs.
  - It is stable, because such a task is never admitted ordinarily.
  - It still fires early on an outlier.

**Comparator.**
- Use a lexicographic key: (tier, feasible ? LB : constant, ordinary).
- Store LB in `Pair` inside `pair_for`. Never recompute it inside the comparator.
- With the mode fixed per entry, this is a strict weak order.

**Dangers.**
- `retain_better` has to use the same key.
  - If it does not, the top-16 retention keeps the HRRN-best pairs and can discard the shortest ones.
  - The rule is then silently ineffective, though still a valid order.
- A sort key is more sensitive to basis mixing than a tier is.
  - A pair bounded by Manhattan, because its table is missing, looks shorter than the same pair bounded by BFS.
  - The order would then favour tasks whose tables are not resident.
  - Use a uniform key, for example Manhattan-only for ordering, and keep the stronger bound for the tier.
- Pairs built with `assess_horizon=false`, as in the bucket sort, carry no LB. They must never reach the keyed comparator.
- Fair admission stays ordinary.
- Spatial LB ignores lane and turn overhead. Reusing the existing `Pair.cost` would give lane-aware order, but it weights pickup by 5 and so is not a slack order.

**Minimal fixtures.**
- An old long task against a fresh short task, both feasible: identical before the trigger, flipped after it.
- Mixed-tier and both-impossible comparisons are unchanged.
- More than 16 feasible candidates where the HRRN top 16 excludes the shortest task. The shortest must still be chosen.
- The sampled fallback path.
- Fair admission unchanged.
- A shelf fixture that pins down the key's basis.
- A change of mode takes effect only at the next entry, and the order is ordinary at and after H.
- A brute-force property test over random pair sets, checking asymmetry and transitivity.
- An exact prefix against the minimal-horizon run, up to the mode tick.

## Fixtures the margin tier needs

- Ledger exactness:
  - A task admitted at tick a and observed complete at schedule tick n has duration n − a.
  - Check this against your saved-trace convention.
- Retargeted tasks are excluded from calibration.
- Prospective-only use:
  - Bucket means at the decision tick come only from completions that were already observed.
  - Check them against hand counts.
- The tier boundary is inclusive.
- Serial and parallel runs give identical results.
- Fair admission, held tasks, started tasks and matching are unchanged.
- An exact prefix against the minimal-horizon run, up to the first middle-tier reversal.
