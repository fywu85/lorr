I ran nothing. My 3×9 fixture was wrong, as your exhaustive check shows, so use your probe's examples instead.

## 1. The candidate-dependent baseline is a real accounting failure

**Accounting argument.**
- Let `a = (g, h_a)` be the arrival state and `e` the end state.
- The credit `D_next(a) − D_next(e) − X`, where X is the paid extras after service, measures efficiency after arrival.
- It does not depend on how bad `a` was. Two candidates with the same `e` and X would differ by `D_next(a1) − D_next(a2)`, so the worse arrival heading earns more credit.
- Let `δ(h_a) = D_next(g, h_a) − B ≥ 0`. It should be charged exactly once, and that formula refunds it.

**Realizable pair (grid-local row/col, north = row−1; hand-derived, please confirm).**
- Robot at (2,0) facing north. Current goal g = (1,1). Next errand at (1,6).
- **Path a, `FRFFF`:**
  - Arrives at t=2 facing east.
  - Ends at (1,3) facing east, where D_next = 3U.
- **Path b, `RFCFW`:**
  - Arrives at t=3 facing north.
  - The terminal wait acts as a virtual rotation to east, so D_next = 5U + extra.

| Baseline | Path a | Path b | Gap |
|---|---|---|---|
| Candidate-dependent, extra = 0 | −6U | −5U | U |
| Common B = 5U | −6U | −4U + extra | 2U |

Path a really is two cells further along the next leg, so the common-B gap of 2U is the truthful one. Adopt the common baseline.

## 2. Exact score and data policy

**Notation:**
- U is the unit cost. T is the turn cost, and extra = T − U.
- f is the forward cost and S is the distance scale.
- N = `locations[idx_next_loc + 1]`.
- t_a is the first index with `cells[t] == g`.

**Enabling rule, decided once per robot per tick.** Continuation is on for robot i only if all of these hold:
- The option is ON.
- i is not pinned, and i is on the exact oriented metric, not fallback and not guided.
- The task exists and has a next errand.
- N is not g.
- N's oriented table is resident, read through `turn_oracle_.peek`.
  - The `peek` runs in the serial pre-pass, after this tick's builds.
  - It causes no LRU, admission or build effects.
- `B = min_h D_next(g, h) < kInf`.
- Every arriving candidate's end value is finite.
  - A pre-scan of at most 129 candidates checks this.
  - If any value is infinite, continuation is off for that robot this tick.

If any condition fails, the robot's entire candidate set uses the unchanged `cost()`.

**Candidates of an enabled robot that never hit g:** bitwise native.

**Candidates of an enabled robot that hit g:**

```
d    = −4U + D*_next(e) − B
cost = ( d + extra·(turns over the whole path) + Σ over all moves of (f − U) ) · S − op·U
```

`D*_next(e)` follows the native terminal-wait rule:
- If the last action is W, take `min( D_next(e), D_next(e rotated ±1) + extra )`.
- If the last two actions are both W, also allow `D_next(e rotated 180°) + 2·extra`.
- These wait slots come after service automatically, because trailing waits imply t_a ≤ 2 or t_a ≤ 3 respectively.

**Properties:**
- Arriving with the best heading and sitting scores exactly the native −4U. The native arrival cliff is kept, and only the post-service slots change.
- A bad arrival heading costs δ once.
- The score is path-independent: the same end state and the same extras give the same score.
- Different arrival times: sitting candidates tie and are ordered by the native op term. Continuing candidates gain U for each productive slot after service.
- **All extras.**
  - Both the turn term and the toll term now cover the whole path.
  - They have to, because the post-service moves are scored against a potential that contains those extras.
  - With surcharge 0 the turn term vanishes.
- **Known-next service inside the window.**
  - There is no special case.
  - Ending on N gives `D_next = 0` and `d = −4U − B`. Holding at N beats leaving it.
  - A path that passes through N and then leaves is scored as if N were unserved. That is a documented approximation, and the candidate is dominated anyway.
- **Repeated-location errands (N equal to g):** native. Holding one more tick is the truthful behaviour, and this avoids an edge case in how services are counted.
- **Later errands and finishing robots:** never used. Nothing unrevealed enters the score.
- **Cache and publication.**
  - Table pointers live for one tick only.
  - Every resident table is on the current metric, because a flush is global.
  - There is nothing to invalidate.
- **Protected robots:** untouched. They are pinned, they have a single choice, and the PIBT pass does not see this score.
- **Audits and transactions:** `cost + op·U` remains the physical score, so both stay valid.
- **OFF path:** implement the new score as a separate function and leave `cost()` and `forward_surcharge` byte-identical.
- **Work:** extra table lookups happen only for candidates that arrive, so the work stays fixed and small.

**Approximations to state explicitly:**
- The pre-service potential omits the heading-specific two-leg term. There is no `D_g^h` table for it.
- The arrival bonus differs between robots with a known next errand and finishing robots.
- Behaviour depends on which next-errand tables happen to be resident.

**Hypothesis (untested):** truthful reservations after service reduce wrong first actions by other robots. Nothing here shows how often that occurs in the warehouse or what it is worth in throughput.

## 3. Independent-oracle fixtures to pass before the 200-step screen

1. **Layered oracle.**
   - **Oracle construction.**
     - On tiny directed-toll grids, run an independent repeated relaxation over states (cell, heading, served flag).
     - Enumerate explicitly the option of replacing each terminal wait by a virtual rotation.
   - **Coverage.** Every arriving candidate in the pool, with U in {1, 4, 8} and extra in {0, 1, 2}.
   - **Assertion.** `(cost + op·U)/S + 4U + B` must equal the oracle's D_next(e) plus the paid extras.
2. Non-arriving candidates are bitwise native.
3. With the option OFF, every candidate is bitwise native.
4. **Heading pair.** For the pair in section 1, the physical gap is exactly 2U. Record the rejected baseline's U gap in the documentation.
5. **Your probe's example A.**
   - Under the new score, the exhaustive joint minimum must make both first actions forward.
   - The joint plan must be collision-free.
   - With A's next errand removed, the result must equal the native minimum of −1040.
6. **Second service.** With the next errand two cells beyond g, reaching it gives `d = −4U − B`, and holding at N is preferred.
7. **Native fall-through.** In each of these cases every candidate of the robot is native:
   - N equal to g;
   - the next table absent;
   - the robot on the fallback metric;
   - any infinite arriving end value.
8. **Pinned and recovery cases.** Rerun the primary, recovery and capacity regressions with the option ON.
9. **Serial and parallel preparation** give identical scores with the option ON.

**Run sequence:**
- 200-step deadline screen.
- Full seeds 0 and 2 with exact controls, with the post-service audit active in both arms.
  - Expect the known-only bucket to move. That is a mechanism check, not success.
- Read the totals, the tails, age p90, and loaded turns, waits and reversals from the replay.
- Six seeds only if seeds 0 and 2 warrant it.

## 4. Simpler alternative if the above proves too fragile

- **Neutral tail.**
  - In the score, let the first goal hit fix `d = −4U` for every continuation.
  - This replaces the native last-hit rule `d = −t·U`.
  - Extras are charged through the first hit, as now.
- **Dependencies.** None: it needs no tables and no next errand.
- **Mechanism.** It removes the `(4 − t_a)·U` incentive to hold the goal, so the joint search can move an arriving robot aside at no score cost.
- **Risk.** Motion after service becomes arbitrary and is driven by op ties. The post-service audit buckets would show this.
- **Status.** This is a separate hypothesis and should not be folded into the continuation experiment.

**Status of this turn:** no bugs found. The items in sections 2–4 are approximations or hypotheses, as labelled there.
