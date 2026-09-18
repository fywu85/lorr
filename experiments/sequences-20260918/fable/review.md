# CGAR v6 sequence-search review: WAREHOUSE, 10,000 robots, 5,000 steps

This was read-only and I ran nothing. Tags: **[C]** proved from source, **[M]** measured in the compact results, **[H]** hypothesis. The v6-only variants (three blockers, free intents, initial wait, deferral) have no results yet. I make no claim about the running reference jobs.

Line numbers refer to the frozen snapshot under `runs/cgar-sequences-build-v6-20260918/sources/cgar/`:
- `cgar_planner/cgar.cpp`
- `cgar_planner/action_sequences.hpp`
- `cgar_planner/pibt_kernel.hpp`
- `tests/cgar_regression.cpp`
- `src/Entry.cpp`

## 1. Correctness and guarantee-preservation defects in v6

The search kernel itself is sound **[C]**:
- The vertex and swap tests are complete (`action_sequences.hpp:68-86`).
- Rollback restores state exactly (`:121-131`, `:188-193`).
- `pending` cannot overflow (`:174`).
- Every layer is replay-validated before the first action is published (`cgar.cpp:1507-1525`).
- An interrupted search exits with code 124 (`Entry.cpp:31-38`).
- Transaction cells are excluded at every step of the horizon (`cgar.cpp:1496`).

The recovery-transaction liveness floor therefore survives. The defects are in what the reservation table does not model and in how state is handed to the next step.

**D1. Commitment targets are invisible to the time-expanded table [C].**
- In the baseline, a turning or blocked robot holds `reserved_[T]` on its target, so nobody else can enter T (`cgar.cpp:1323-1327`, `:1342-1347`).
- In refinement, choice zero is "first action, then waits" (`cgar.cpp:1468-1476`), so T never appears in `owners_`. `fixed[]` protects the robot's action, not its reservation (`:1406-1408`).
- Failure scenario:
  - The primary P is turning (CCR) toward T, and an ordinary robot Y is adjacent to T.
  - PIBT denied T to Y, so entering T strictly improves Y's cost and the search accepts it.
  - On the next step P's forward move is cancelled (`:1152-1161`), and P's stall count rises until a recovery transaction fires.
- The same happens to every unchanged turning robot. This is consistent with **[M]** expired commitments rising from 98–1,140 to 333k–360k.
- **[H]** Most of the refinement's executed changes are this kind of reservation theft. That would explain a short-run gain followed by long-run loss.
- Decisive check: count changed first actions whose new cell is another robot's live commitment target.
- Fix: enter live commitment targets into `owners_` as sentinel owners for t = 1..H.

**D2. Duplicate commitments are resolved by robot index and can preempt the primary [C]** (non-free-intents modes).
- `cgar.cpp:1527-1547` writes `committed = target` with no uniqueness check. With D1, Y's path can enter P's target at t ≥ 2.
- On the next step, commitments are re-reserved in index order, before PIBT runs (`:1318-1329`). If Y's index is below P's, P's commitment is silently dropped (`else a.committed = -1`).
- In the baseline, targets are unique by induction, so index order was harmless there.
- Free-intents mode wipes ordinary commitments (`:1219-1229`). That neutralizes D2 but not D1.
- Fix: iterate that loop in `order_` (already computed at `:1279`).
- Test gap: only the primary's first action at t = 0 is asserted (`cgar_regression.cpp:465`).

**D3. The initial-wait variant deletes feasible baseline turns [C].**
- `cgar.cpp:1426` forces W on every non-kept robot, including CR/CCR turns that can never conflict.
- The sequence pool forbids a trailing unfinished turn (`:1389`), so "turn now, move after the horizon" cannot be expressed.
- Queued robots therefore idle instead of pre-rotating, then turn later in front of their followers.
- Prediction (unmeasured): the initial-wait variants lose to their twins unless non-FW baseline actions are kept.

**D4. The search budget is burned rather than used [C] + [M].**
- Candidates are cost-sorted (`action_sequences.hpp:42`), yet scanning continues past the first free candidate that does not improve (`:117`).
- Each scanned candidate is charged to one budget shared by the whole displacement chain (`:53`, `:109`).
- **[M]** 54–59 candidates per attempt against a cap of 64, and 82–91% of attempts roll back.
- **[M]** Only 1.6–2.1% of executed actions differ from the baseline (0.80–1.03M of 50M robot-steps).

**D5. The Timeout margin is thin [M] + [C].**
- About 150 MB of `ActionSequence` objects are rebuilt every step, with growth reallocation (`cgar.cpp:1374`, `:1502`).
- **[M]** System time is 27–147 s per run against 3.6–4.4 s for the baseline. Mean decision time is 0.20–0.25 s and the maximum is 0.69–0.80 s, against the 1.0 s limit.
- The running deep and multi-blocker variants quadruple the candidate cap, so expect exit-124 failures. Record those as failures, not as missing data.

**Anomalies that need diagnosis [M]:**
- **Safety waits rise from 10–82 to 161k–187k.**
  - `make_safe` runs on the baseline before refinement (`cgar.cpp:1352`), so the state handed back by refinement is breaking baseline invariants.
  - **[H]** Refined robots often end a step uncommitted while a neighbour is committed to their cell. The kernel blocks only child-to-parent swaps (`pibt_kernel.hpp:23`), so head-on pairs form and cancellations cascade down convoys.
- **Primary stall in orientation-only runs.**
  - At t = 4,800 the primary's stall counter reads 1,496, 1,061, 810 and 794 in four of the nine orientation-only runs.
  - Transaction counts are 948–1,656 against 25–129 for the baseline.
  - **[C]** `best` is never reset when a robot becomes primary. Committed robots are unpushable (`pibt_kernel.hpp:28`), and their reservations ignore priority.
  - **[C]** Under oriented costs plus turn-first, an equal-cost alternative that needs a turn loses to waiting (`cgar.cpp:905-910`).
  - **[H]** Tight committed convoys starve crossing robots until they become the primary, which then advances only through serialized transactions. Log the primary's h, `best`, stall count and whether its own cell was reserved, on every step.

## 2. Why this search is far from NMS

1. **NMS's horizon is not longer [C].**
   - It uses the same five steps, a 129-operation pool (`operations.cpp:71`) against CGAR's 189.
   - What differs is that the five-step operation is the decision variable.
     - Every robot starts at all-wait (`pibts.cpp:857-860`).
     - `build` then assigns best-first operations in priority order, displacing unbuilt robots recursively. Depth is limited only after 3,000 nodes (`:570`), and a move is accepted if the score does not drop (`:20`).
   - NMS has no commitments. It executes only the first action and rebuilds the plan every step.
   - v6 keeps a one-step PIBT decision and audits it against a world in which every other robot is "parked" after t = 1. D4 is the measured result.
2. **The handover converts a timed joint plan into untimed four-step exclusive cell claims [C] + [M].**
   - Completions per 1,000 steps, first and last windows:

     | Variant | First 1,000 | Last 1,000 |
     |---|---|---|
     | baseline | 9.1–9.3k | 5.6–6.5k |
     | sequence5_two | 9.8k | 5.1–5.5k |
     | sequence5_two_orientation | 9.4–9.5k | 4.9–5.4k |
     | orientation only, 512 MiB | 8.7–8.8k | 7.9–8.2k |
     | orientation only, 8 GiB | 8.8–9.0k | 8.3–8.8k |

   - The sequence layer removes the stability that orientation guidance provides.
3. **Where robot time goes [M]** (orientation_8192, seed 3):
   - Forward moves take 55.2% of robot-steps, turns 8.7%, and waits 36.1% (21.6% planned, 14.5% turn-blocked).
   - 25% of forward moves go farther from the goal, so net progress is 0.27 cells per robot-step.
   - About 28% of all robot-steps are push-out-and-return churn.
   - A one-step push snowploughs the blocker repeatedly, whereas a five-step operation clears the pusher's corridor once.
   - Consistency check on assignment:
     - The estimated pickup distance is 55 cells and the observed pickup delay is 208–212 steps, a ratio of 3.8.
     - That matches 1 / 0.26 cells per robot-step, so motion binds throughput, not assignment distance.
4. **NMS never idles a stationary robot [C].** All-wait robots rotate toward their best heading for free (`pibts.cpp:996-1007`), and trailing waits are scored as rotations (`:134-169`).
5. **The objective differs [C].**
   - NMS scores terminal oriented distance only, with a "reach the target at depth d, then keep moving" bonus (`:171-177`).
   - v6's cheapest continuation after arriving is to park on the endpoint for the rest of the horizon (`cgar.cpp:1461-1467`). The next stop of the task chain is not used.
6. **Compute [M] + [C].**
   - CGAR uses 19–33 ms per step on one core.
   - NMS runs 32 workers until the deadline (`settings.hpp:57,61`, `eplanner.cpp:33-54`). It accepts equal-score moves, so it can cross plateaus; v6 requires strict improvement.

The following are **not** the reason **[C]**:
- DHM and DHMR are disabled (`settings.hpp:24-26`, `dynamic_heuristic_matrix.cpp:265-266`), so NMS has no congestion-aware heuristic active.
- WAREHOUSE has no fleet cap (`info.cpp:26`).
- Robot weights are flat on WAREHOUSE (`pibts.cpp:716-717`).

## 3. Next three generic changes, ranked by expected impact

**#1. Make sequence-space priority inheritance the planner for ordinary robots.** Keep the certified one-step pipeline for the protected set and as the floor.

Fixed-work version:
1. Precompute sequence cells per (cell, orientation) once. This is about 0.6 GB and removes the per-step materialization.
2. Run today's pipeline through `make_safe` to get the floor action B and the protected set S.
3. Reserve S's first actions, the primary's path, the transaction cells, and S's live commitment targets as sentinel owners. This also fixes D1.
4. Start ordinary robots unbuilt, keeping any baseline turn. Constrain robots in S's forward-dependency closure to sequences whose first action matches B.
5. Build in `order_`, best-first, displacing only unbuilt robots. Cap nodes per root (about 512) and per step. These are counts, never clocks.
6. Carry no ordinary commitments between steps.
7. If the closure is unsatisfiable, output B verbatim. This is a deterministic rule, and it is counted.

Decisive validation:
- On the matched-goal probe on one core, require at least 6,000 arrivals (CGAR 4,159, NMS-uniform 8,047).
- On full runs, seeds 0–5, against orientation_8192:
  - Win on every seed.
  - Last-1,000-step rate at least that of the control. Every sequence variant has failed this so far.
  - Safety waits and commitment expiries back to baseline magnitude.

**#2. Parallel fixed-work improvement, gated by a reference ablation.**
- Reference ablation first:
  - Set `PIBTS_STEPS = 0` (`settings.hpp:61`) and `THR = 1` (`eplanner.cpp:33`) in the NMS-uniform reference, then rerun the probe.
  - If build-only reaches about 7,000 arrivals or more, the construction explains the gap and #2 is optional.
  - If it lands near CGAR's level, search effort is the mechanism and #2 is mandatory.
- Fixed-work version:
  - Use 16 explicitly allocated workers, each copying the constructed plan (about 5 MB).
  - Each worker executes exactly M seeded remove-and-reinsert moves, accepting equal or better cost.
  - The result is the lowest-cost plan, with ties broken by worker id. This is bit-reproducible.
  - Calibrate M so p99.9 stays at or under 0.6 s; overrun is a Timeout.
  - Peak memory stays under 20 GiB.
- CGAR validation: M ∈ {0, 25k, 100k} × W ∈ {1, 16}, requiring a monotone last-1,000-step rate.

**#3. Free pre-rotation plus an arrival-aware terminal cost.** This is cheap, independent of #1, and testable now on orientation_8192.
- Pre-rotation:
  - For an unprotected robot with a planned wait and no commitment, emit CR or CCR only if it strictly lowers the oriented cost-to-go.
  - Rotation in place cannot collide and touches no reservation.
  - It strictly decreases a potential, so it cannot dither.
- Terminal cost:
  - Score arrival at step d with a bonus.
  - Evaluate the remainder of the sequence against the task's next stop.
- Decisive validation:
  - Run with and without pre-rotation on seeds 0–5.
  - If the turn-blocked share (14.5%) does not fall, blocking turns are route corners and the change should be dropped.

## 4. Where the current diagnosis is unsupported

- **"Longer horizon than NMS"** is false per the NMS source.
- **"Exact oriented terminal costs appear insufficient"** is not shown by the archived runs. Those combined runs used the 512 MiB cache, which guided only about 36% of decisions. Wait for `sequence5_oriented_8192`.
- **Short screens mislead [M].**
  - Both orientation and sequence effects flip sign between t = 1,000 and t = 5,000.
  - Screen from a congested late-run snapshot instead.
  - The 100-step matched probe shares this weakness, plus 32 cores against 1 and CGAR-generated starting states.
- **"Commitments help"** was measured for spatial one-step PIBT. It does not transfer to oriented multi-step planning, where NMS uses none. Deferred acquisition removes one producer of expiries, and D1 and D2 remain.
- **Cache coverage is not the gap.** I agree: 98.9% of decisions are guided at 8 GiB **[M]**.
- **Do not generalize from the h = 3 collapse.** The pool has only 19 sequences **[C]**, and no compact evidence explains the result.
