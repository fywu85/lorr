Run status: failed with `error_max_budget_usd`. The CLI emitted the following
visible findings, but did not return a successful final result. Claims require
independent assessment. No tests were run by the reviewer.

## Findings, ranked by severity

I read the code only; nothing was run. Line numbers aren't recoverable from the snapshot, so I cite functions.

### 1. Confirmed by reading: candidates that pass the cached waypoint are scored as regressions

The cause is three pieces acting together:
- `GuideRoutes::build_window` seeds the reverse BFS from the single waypoint state.
- `update` rebuilds the window only when `begin >= waypoint`.
- `TemporalGeometry::cost` scores only a candidate's end state, through `GuideRoutes::distance` (BFS distance to the waypoint plus `suffix`).

Counterexample: a straight east corridor with lookahead 8, and a robot 3 actions before waypoint W.
- `FFFFF` ends at (W+2, E). The window distance back to W is CR, CR, F, F, CR, CR = 6, so it scores `suffix+6`. The true remaining distance is `suffix−2`.
- `FFFWW` scores `suffix`.

So whenever 1–4 actions remain to the waypoint, roughly half of free-running guided robot-steps, the top-ranked plan is "reach W, then park". The first action is usually still F, which is why single-robot fixtures pass. The consequences, all read from the code:
- **Phantom parking:** the plan reserves a lane cell at t=k..4, which blocks the top candidates of followers and crossing robots.
- **Wasted search in `TemporalPibt::build`:**
  - A displaced leader has many park-at-W variants, and each one conflicts.
  - Its real `FFFFF` then "loses" 6–8 units, so `consider()` rejects it, the attempt returns 2, and the work is discarded.
- **Sticky waypoint:**
  - If a robot waits on W, bypass candidates that rejoin the route beyond W are scored as detours back to W.
  - The window never rebuilds, because W is never reached.
- **Warm start reinforces it:** retained `F…W` suffixes are never re-rooted by `construct`.

I did not confirm that this costs throughput or magnifies conflict search; that part is a hypothesis. It is a direct mechanism for more `temporal_repair` work as reconnection pushes guide coverage toward 100%.

**Fix:**
- Narrow: rebuild when `!final && begin + 5 > waypoint`, and require lookahead ≥ 5 (12–16 amortizes the extra builds).
- Complete: use a bucketed multi-source BFS that seeds every in-box route state `j ≥ begin` with value `size−1−j`. This also scores bypass-and-rejoin correctly.

**Tests:**
- In a corridor, after advancing k=1..7 states, assert for m=1..5 that `distance(states[begin+m]) == size−1−(begin+m)`.
- Also assert `cost(FFFFF) < cost(FFFWW)`.
- Diagnostic: the share of guided robots whose selected path ends on its waypoint cell with trailing waits.
- Snapshot A/B with identical seeds: compare `TemporalStats.candidates`, `recursive_calls` and `budget_exhausted` under the original, current and fixed metric.

### 2. Hypothesis: reconnection removes the only route refresh

Before reconnection, a deviation invalidated the route and triggered a fresh congestion-aware A*. `reconnect` now succeeds almost always:
- The connector is at most the distance to the waypoint plus 2 turns, which is within 16.
- The measured rate is 2040 reconnects against 26 A* attempts.

As a result, a route chosen at goal assignment is never re-evaluated against current flow. A robot that deviated because its lane was blocked is sent back onto the same lane. The connector itself ignores flow counts and can add +1 on edges running against heavy traffic.

**Fix:**
- Add a per-route reconnect counter, and after K reconnects without a shrinking remaining length, fall through to `invalidate`.
- Or spend leftover `batch − attempted` slots on cursor-driven re-searches of the oldest routes. Keep the new path only if it is cheaper under current flows. Both are deterministic and count-conserving.

**Test:** A/B at identical temporal work over 1000+ steps, logging the sampled ratio (retained route cost under current flows) / (fresh A* cost).

### 3. Confirmed by reading: the candidate-work limit does not cover regional repair

`repair_temporal_regions` calls `search->repair(options.steps, check, 0, …)`, so the candidate limit is 0. Only the global worker receives `temporal_candidate_limit_`. If regions were enabled in the failing profile, the planned candidate-limit follow-up would move the overrun to `temporal_region_repair` rather than bound it. One attempt can also overshoot the limit by up to about 1000 recursions × 128 candidates before the limit is checked.

**Fix:** add `TemporalRegionOptions::candidate_limit`, fixed per region and round, and pass it to `repair`.

**Test:** assert `observed.candidates ≤ limit + one-attempt bound`, and that serial and parallel decisions stay identical.

### 4. Confirmed mechanism, performance only: limited searches are retried with no memory

In `update` step 2, when fewer robots are unguided than `batch`, every unguided robot is attempted every step. A search that exhausts `expansions` leaves no record, so the same far-goal robots burn 4096 expansions on each call. GUIDE_PATHS.md reports 321 of 512 attempts limited and about 0.19 s of preparation at the step-200 sample for the weight-1 opposing-cost profile. Reconnection lowers demand, so this becomes an every-step retry.

**Fix:** a deterministic backoff.
- Record `h(start)` at the limited attempt, and skip the robot until `h` has dropped by Δ or the goal changes.
- Alternatively, skip for a fixed number of steps.

**Test:** with `expansions=1`, a third `update` must not re-attempt robot 0. Log the repeat-limited fraction.

### 5. Hypothesis: route concentration

Three things combine:
- `Node::operator<` breaks final ties by lowest `state`.
- `load_cost` defaults to 0, so same-direction stacking is free.
- Weight-2 A* hugs the heuristic line.

Robots with similar displacements therefore pick the same L-shaped route, which produces long convoys. Convoy length drives `build` recursion depth, up to 1000 calls × 128 candidates per attempt. The reset `visited_[r]=0` on failure also allows a robot to be re-entered within the same attempt. GUIDE_PATHS.md reports that load cost 1 completes where load cost 0 times out.

**Fix:** a robot-salted deterministic tie-break, and/or a default `load_cost ≥ 1`.

**Test:** a `flow()` percentile histogram per profile, plus per-attempt `TemporalStats` on one shared snapshot.

### 6. Hypothesis: the warm start ignores guide changes

`TemporalWarmStart::selections` keeps a robot's suffix whenever its goal is unchanged, and `construct` skips any robot with `selected != 0`. After a reconnect or a new route, the retained suffix may have negative gain under the new metric. Only a random repair draw can fix it.

**Fix:** pass a "guide changed" mask (reconnected or newly solved robots) as extra initial resets through the existing reset closure.

**Test:** count retained selections whose cost exceeds the seed cost, with and without the mask.

## Invariants that hold

- **Path and orientation validity.**
  - Parent chains have strictly decreasing g, so they are acyclic.
  - The descent uses only F/CR/CCR with an exact −1 step.
  - Alignment turns are ±1, and `states[join]` to `states[join+1]` is a forward move.
  - Routes stay cell-simple apart from consecutive rotations.
- **Exact intended-edge count conservation.** Consumption, `invalidate` and `reconnect` are mutually consistent, and the retained edge `states[join]→states[join+1]` reuses the same flow index.
- **Stale goal and protected-agent invalidation.**
  - Invalidation happens before `reconnect`.
  - Pinned robots are never guided, and their seeds use the original metric.
- **Window coverage (spatial).** The robot is within `lookahead` of the waypoint and candidates within +5, and the symmetric core graph gives finite distances. Only the metric is wrong (finding 1).
- **Reconnect descent.** The "lost a finite descending action" throw is unreachable, because the waypoint cell is always stamped.
- **Deadline propagation.** Every mutation in `reconnect` follows the final `check()`, and search and window builds publish nothing partial.
