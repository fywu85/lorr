# Explicit field comparisons

Declared before running the new field-option binary. All runs use full archived
horizons, four bound physical EPYC cores per case, strict 1,000 ms decisions and
32 decimal GB. This is a source-preserving CGAR comparison with explicit tricks.

CITY-01/CITY-02/GAME retain their selected scheduler and planner and change only
the opposing lane price from 16 to 12, 8 or 4. Preferred forward edges and turns
remain 4. The 4 arm is uniform; no edges or physical actions are removed. The
motivation is under 2% measured waiting in the cities: detours may matter more
than additional congestion avoidance. GAME is a separate transfer test.

RANDOM reference 3 exposes the existing PILOT flow field on the sparse/medium
instances. Its bytes are exactly the earlier independently exported integer
asset (source 233f5bf; average flow, power 3, betweenness 0.75, seed 15, one flip,
output penalty 2.4). It does not fit a new field or use hidden task streams.
The floating original differs by at most 0.05 original cost units per edge after
rounding. The reference sparse/medium planner uses turn 2, so these comparisons
retain CGAR's integer turn 20 except the explicitly declared turn-40 arms.

RANDOM-03 is a 2x2 field/window comparison against the existing 1,902 no-window
control and 1,939 protected-prefix window with history disabled. RANDOM-01/02 compare their selected
windows to the PILOT field, then test turn price and a no-window ablation.
These are hypotheses, not additive gains or measured throughput results.
