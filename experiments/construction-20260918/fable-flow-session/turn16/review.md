I ran nothing.

I accept your three corrections:
- **First-action invariance:** my claim was false.
- **Four peeks:** they do not give the exact coupling between legs.
- **Repeated partition phase:** it still changes roots and RNG use.

Every timing figure below is an estimate from reading the source.

## 1. The one measurement: a frozen-decision replay harness under `perf`

**Inputs to dump, at two ticks.**
- Pick a typical tick and a slow tick, for example a tick of the kind that timed out.
- For each robot, dump the cell, the heading, and the sorted list of (op, cost).
- Also dump `pinned`, `power`, `order`, the worker and region seeds, and the best worker's selections.
- Paths come from the same geometry initialization, so they need not be dumped.

**Replay and verification.**
- Replay `construct`, `repair` and `repair_temporal_regions` in a loop on isolated GRID cores.
- Assert bit-equality with production at that tick:
  - selections and double scores;
  - all `TemporalStats` counters;
  - the next RNG draw.

**Build provenance.**
- Build with `-g` only.
- Verify that the `.text` hash equals the production binary's.

**Profiling.**
- Run `perf record -e cycles:u --call-graph dwarf`.
- Run `perf stat` for cycles, instructions, cache misses, L1-dcache misses and dTLB load misses.
- The output gives:
  - per-function shares for `blocker`, `reserve`, `build`, the constructor phases, the `choice_regions` pass and `clock_gettime`;
  - whether the inner loop is memory-bound or TLB-bound.
- The work is frozen and the outputs are checked, so search differences cannot confound the timing.

**What your counters already say.**
- Your slow step showed about 51.6M candidates over 200,000 regional attempts in 395 ms on four threads. That is roughly 30 ns per candidate per thread.
- It points to the per-candidate path as the target, not to setup.
- The harness must confirm this.

## 2. Ranked exact optimizations

**1. Co-locate each edge's five slots with its minimum cell's five vertex slots in one 64-byte record.**

*Why it is exact.*
- This is a pure layout change.
- `used_edges_[e][t]` becomes `slots_[e >> 1].edge[e & 1][t]`.
- Values, scan order and return values stay identical.
- `blocker`'s result depends only on the set of owners. It never depended on which table holds a slot.

*Why it might pay.*
- Each candidate reads up to five edge slots from a second 2.8 MB array.
- For a move a→b, the edge belongs to min(a, b).
- The vertex scan already loads that cell's line at t−1 or at t, or the line is hot from the robot's own removal.
- The edge loads would therefore stop costing extra cache lines.
- This helps the global and regional passes alike, and also the constructor's reinsertion.

*Payoff.*
- It is plausible only if `perf` shows that `blocker` dominates and misses on the edge array.
- In that case it is the only item here that could approach 100 ms on a heavy step.

*A smaller alternative with the same invariant.*
- For t ≥ 1, skip the edge lookup when `used_cells_[cells[t]][t-1] < 0`.
  - Reservations are complete, so a robot traversing that edge in the opposite direction must own that slot.
  - A robot traversing it in the same direction was already seen at slot t−1.
  - Keep the t=0 lookup, because a pinned robot's current cell is not stored in the table.

**1b. Huge pages, only if dTLB misses are material.**
- The geometry table is roughly 0.9 GB. Each robot's 129 paths form a block of about 5.7 KB, and the blocks are scattered, so a robot switch is likely to miss the TLB with 4 KB pages.
- Applying `madvise(MADV_HUGEPAGE)` to the geometry, reservation and choice arrays has no behavioural effect.
- Check the RSS rounding against the 32 GB cap.
- Decide this from the dTLB counters. If they are low, skip it.

**2. Share regional setup within a round.**

*Base reservation table.*
- Every region constructor re-validates and re-inserts all 10,000 robots into a fresh 4.2 MB table, which also costs fresh page faults.
- `merged` is built serially again after each round.
- Exact alternative:
  - Build the base table once per round. The previous round's `merged` already is that state.
  - Copy it into each worker's private, reused scratch with `memcpy`.
  - Copy `score_` bitwise. It uses the same index-order summation, so it is bit-identical.
  - Validation then happens once.

*`choice_regions`.*
- The pass is serial. It performs 10,000 small vector allocations and about 5M path lookups per round.
- Flatten it into one array with per-robot offsets. It is a pure function of the owner map and the paths, so it can also be parallelized deterministically.

*Index construction.*
- The build dereferences `path->depth` for every choice across three horizons.
- Store a copy of `depth` beside each choice once per decision. This is exact.

*Expected payoff.*
- I expect tens of milliseconds in total, roughly proportional to the number of rounds.
- It is certain but modest, and it matters most for your four-round arms.

**3. Clock calls.**
- There are about two per attempt (the repair loop and the root `build` at counter 0), plus one per 16 recursions.
- I estimate a few hundred thousand to a million vDSO calls per step, which probably comes to a few milliseconds of wall time.
- Touch this only if `perf` shows 1–2% or more.
- If so, keep the call sites but read the clock every Nth call through a local counter.
  - The search is unchanged.
  - An overrun is detected at most a sub-millisecond late.
  - The final complete-entry check still fails the run.

## 3. Your three candidates

**1. Caching the classification per shift.**
- It is exact. `allowed[k]` depends only on the owner map and the paths, and roots and frozen crossers are still recomputed from the current selections.
- Two rounds use phases 0 and 1, so there is nothing to reuse there.
- The reuse appears only in the four-round arms, where your 4-part partition repeats phases geometrically.
- Treat it as part of optimization 2's flattening. It is not a separate win.

**2. Sharing admissible index lists across regions.**
- There is little to share.
  - A robot is a root of at most one region per round.
  - The regional `candidate_order_` lists are therefore disjoint.
- What can be shared:
  - the depth classification, per decision;
  - `choice_regions`, per phase;
  - the base reservation table, per round.
- My guess at the cost ranking is:
  - first, reinserting all robots;
  - then zero-fill plus page faults;
  - then the `depth` dereferences.
- Have the harness time the constructor phases separately. Do not trust my ranking.

**3. Clock calls.** I agree: measure first and expect little.

## 4. Exactness protocol for each change

- In the harness, assert for both ticks and several seeds:
  - identical selections and identical doubles;
  - identical stats, including candidates, recursive calls, repairs, accepted, kept, reverted and maximum depth;
  - an identical next RNG draw.
- Compare median wall time and its spread over at least 30 repetitions on pinned cores.
- In production, the full-trajectory hashes on seeds 0 and 2 must equal the reference.
  - Timing never feeds into decisions.
  - Any hash difference is therefore a bug and not noise.
- Record source, binary and `perf` data hashes together.

## 5. If the measurement disagrees

- The profile may show the inner loop is compute-bound:
  - low miss rates;
  - time spent in `take`, in branch logic, or in `exp` and RNG.
- In that case none of the above recovers 100 ms, and I see no other exact lever in the supplied source.

**The exact-only constraint then has a limit.**
- Regional work is bounded by attempts. It is not bounded by candidates, because the regional `candidate_limit` is 0.
- This is why the distance-scale-513 run could jump from 140 ms to 395 ms.
- Making regional time predictable would need a fixed candidate budget.
- That is a work-policy change, not an exact optimization, and it would need its own paired runs.
