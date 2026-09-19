# Review of CGAR build v11: weighted turns and parallel regional repair

I found no safety, race, lifetime or nondeterminism defect in the regional repair. I found one proven scoring defect in weighted turns, and it confounds the queued cost-2/4 full matrix. I ran nothing; every conclusion comes from reading the frozen sources and the recorded 50-step screens. I make no throughput claims.

Paths are relative to `runs/cgar-temporal-build-v11-20260918/sources/cgar/`.

- **Weighted turns:** the oriented-distance tables (oracle) and the exact-table scoring are correct. The adapter's fallback distance is not consistent with the turn surcharge (P1).
- **Regional repair:** the timeout contract holds. The gaps are observability (P2) and deadline margin.

## Proven defects

### P1. The fallback potential never credits the turns that path scoring charges
- `TemporalGeometry::cost` charges `extra = C−1` for every real turn (`cgar_planner/temporal_geometry.hpp:35,52`).
- With an exact table this is sound: executing a turn drops the potential by C, so each productive slot nets +1.
- Without a table, `compute_distance` returns `d_spatial + C × turns_needed_now` (`cgar_planner/temporal_adapter.hpp:60`).
- That potential contains no future corner, so a mid-path turn is charged with no matching drop. Worse, the potential jumps up by C on arriving at a corner.

Counterexample: one robot at A facing east, goal G, no oriented table cached, C=8.
```
A B #
# C #
# D #
# E #
# G #
```

| Choice | Score |
|---|---|
| Wait seed | 5·50 = 250 |
| `FRFFF` (op 111) | (1+7)·50 − 111 = 289 |
| `FRFFW` | 346 |
| `WFRFF` | 376 |
| `FRFWW` | 420 |
| `FWWWW` | 492 |

- Every candidate has negative gain. Construction runs at temperature 0, so the robot never moves. A lone robot also never moves in repair, because `consider()` requires `old_score_ > 0`.
- With the exact table, the wait seed scores 650 and `FRFFF` scores 289, so the robot moves.
- At C=4, a corner followed one cell later by a second corner flips the same way. Wait scores 300; the best moves score `FWWWW` 342 and `FRFCF` 365. With the exact table, wait scores 700.
- At C=1 the two formulas are identical, so unit cost is unaffected.

How much of the fleet this touches:
- Tables are built at one site only, with `turn_table_budget_ = 32` per step (`cgar_planner/cgar.cpp:934-937`, `:1256`).
- By step 50 at most 1,600 goals have tables, against up to 10,000 live goals. The cold screens for costs 2/4/8 therefore mostly exercised this fallback.
- The regression bound uses only the exact table, and checks only the lower bound (`tests/cgar_regression.cpp:383`, `:406-417`).
- The `[cgar-orientation] guided/fallback` counters print only every 200 steps, so the 50-step screens recorded none.

Minimal fix: use a per-robot metric.
- In the adapter, set `robot_turn_cost = (oriented && value(start) < kInf) ? turn_cost_ : 1`.
- Use it both at `temporal_adapter.hpp:60` and in the `cost` lambda at `:70-72`.
- Robots with tables behave exactly as in v11, and unit-cost trajectories are unchanged.

### P2. The regional regression passes if the layer does nothing, and the logs cannot show what it contributes
- The assertions are serial-equals-parallel, score not decreased, no collisions, and fixed paths unchanged (`tests/cgar_regression.cpp:596-613`). A search that never merges a change satisfies all of them.
- `TemporalRegionStats.accepted` counts accepted attempts even in regions later rolled back by the final-state rule (`cgar_planner/temporal_pibt.hpp:85-89`).
- Both diagnostic lines print the post-regional `search.score()` (`cgar_planner/temporal_adapter.hpp:193,198`), so the pre-regional score is lost.
- The cold prefixes diverge from the control (100 and 98 tasks versus 90), so production is not a no-op. Its effect cannot be measured from the logs.

Fix, none of which changes trajectories:
- Add `score_before`, `kept_regions`, `reverted_regions` and `frozen_crossers` to `TemporalRegionStats` (`temporal_regions.hpp:15-17`).
- Print `results[best]->score()` on the construction line.
- Assert that at least one of the 24 trials improves the score and that `candidates > 0`.

## Hypotheses (not proven)
- **H1. P1 may explain the rising entry times.**
  - The cold-screen maxima are 0.508 s (control), 0.646 s (cost 2), 0.841 s (cost 4), and an explicit timeout at cost 8.
  - Bucket-queue (Dial) table building costs the same at every C, so it cannot produce that trend.
  - More waiting under the fallback, leading to congestion and deeper displacement chains, could.
- **H2. Each regional search restarts hot.**
  - `temperature_ = 0.001` is normalised by the whole-fleet score (`temporal_pibt.hpp:70`, `:128-133`).
  - Roughly the first 5k of the 25k attempts accept most losses, and regions are kept or rolled back as a whole.
  - Once the P2 counters exist, test passing a starting temperature for the regional searches.
- **H3. Weighted guidance can steer the protected primary off the spatial descent.**
  - The primary's candidates follow the weighted table (`cgar.cpp:947`). Stall detection and recovery use the spatial potential (`:1336-1344`, `:1157-1187`).
  - Progress still holds, because the spatial `best` decreases monotonically. Stall-triggered transactions may rise.
  - Compare `txns` and `txn_moves` against the control.
- **H4. The two-round profile has thin deadline margin.**
  - Two rounds of 25k attempts peaked at 0.895 s in 50 steps. The work is fixed-count, so one slow step anywhere in a 5,000-step run rejects the whole run.
- **H5. A latent false abort under ranked weights.**
  - In round 0, `before = initial.score()` has been accumulated incrementally, while the merged score is a fresh sum (`temporal_regions.hpp:96,114`).
  - Equal weights are exact integers, so every queued profile is safe.
  - Under ranked weights the drift is around 1e-7 against a 1e-6 tolerance, so a false abort is possible but unlikely.
  - Fix: build the baseline `merged` from `selected` before round 0.

## What I checked and found correct
- **Partition and wrap.** The `owner` map is a true partition. Shifting wraps region 0 into two disconnected strips, which does not affect safety.
- **Geometry.** Edge ids are unique, and `neighbor` never wraps across the map border. Boundary-crossing edges can only be used by frozen robots.
- **Freeze rule and candidate restriction.** A root's start cell, current path and every candidate all lie in one region (`temporal_regions.hpp:53-62`). The filter also applies to displaced robots (`temporal_pibt.hpp:145-146`). The wait seed is always inside the region.
- **Merge.** Writers are disjoint and there is a fixed-robot check. A full rebuild of cell and edge reservations detects any pairwise conflict, and the adapter then runs its own independent check (`temporal_adapter.hpp:155-169`). Actions are published only afterwards (`:170-187`).
- **Threads.** Workers only read shared state and write to distinct result slots. All workers join on every path, and the first stored error is rethrown. A timeout is fatal through `_Exit(124)` (`src/Entry.cpp:52-58`). No elapsed-time partial result is ever selected.
- **Determinism.** Seeds are drawn for all parts, including empty ones. Results do not depend on thread scheduling.
- **Bucket-queue Dijkstra.** It uses C+1 circular buckets, skips stale entries, has no zero-weight edges, and caches nothing before the completion check (`cgar.cpp:444-466`).
- **Exact-table scoring.** cost ≥ D0 − 5 follows from the triangle inequality, including virtual rotations during terminal waits.
- **Unit cost.** The formulas and the BFS branch reduce exactly to v8. No extra random draws happen when regions are off.
- **Protected intents.** The protected-intent lines of the adapter are byte-identical to v8 (`temporal_adapter.hpp:11-35`).

## Multi-core use
These are timing facts from the cold screens, not a throughput claim.
- One round adds about 112 ms of wall time and about 357 ms of CPU per step.
- The regional phase keeps about 3.2 of 4 cores busy.
- That is 100k attempts in roughly the wall time of 30k.
- Regional gains add across regions. The best-of-4 portfolio gained only 0.55%.
- Whole-process utilisation is only about 1.5–1.8 cores. Base planning, candidate generation, 50k global attempts, the per-round `choice_regions` pass and the merge rebuild are all serial.
- If the P2 counters show regions being kept, the productive next profile moves work into regions. An example is 25k global attempts plus two rounds of 25k. Add per-phase wall times to the diagnostics first.

## Missing adversarial tests
1. A progress-credit test. Some candidate must beat the wait seed whenever a free optimal route exists, at costs 1/2/4/8, against both the exact table and the production fallback. This fails today.
2. A non-vacuity assertion for the regional fixtures (P2).
3. With rounds=1, assert that boundary-crossing robots are unchanged and that every changed path lies wholly in its start region.
4. `Cgar::plan` with regions enabled, comparing region threads 1 and 4 over a multi-step episode. Add a real-map prefix hash to `equivalence.json`, which has no regional entry.
5. The unit-cost v8 versus v11 trajectory hash, which is still queued.

## Minor
- `fixed` is declared after `results` (`temporal_regions.hpp:66-69`), so it is destroyed first. That is harmless today because the `TemporalPibt` destructor never dereferences it.
- The partition choice compares floating-point logarithms (`:33-36`); cross-multiplied integers would be cleaner.
- `priorities` mixes the weighted and fallback distance scales (`temporal_adapter.hpp:73`).
- The regression needs at least 4 CPUs in its affinity mask, so it would fail at initialisation on a one-core allocation.

## Most informative next bounded validation
1. Add tests 1 and 2.
2. Apply the P1 per-robot metric and the P2 counters.
3. Re-run the three turn-cost cold screens at 200 steps, so that `guided` and `fallback` are logged.
4. Run one 200-step regional screen that reports kept and reverted regions and the score change per round.

If the entry times for costs 2/4/8 fall back toward 0.51 s, H1 is confirmed and the turn matrix is no longer confounded. Hold job 8898338 until the P1 fix is in. Without the fix, a null result cannot distinguish "weighted turns don't help" from "the fallback bias hurt".
