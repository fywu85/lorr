# Initial cross-instance transfer review

Read-only inspection on2026-09-20. [Source hashes](review-source-hashes.json).
The CGAR core is retained; references supply mechanisms to evaluate, not a
replacement solver to relabel. The separate RANDOM-05 agent's source and jobs
are untouched.

## Why Warehouse settings are not a universal baseline

NMS's active `Solution/Planner/PIBT/pibts.cpp`676–725 uses equal robot score
weights for Warehouse and Sortation, remaining-task-distance ranks elsewhere,
and squares those weights on GAME and RANDOM-04/05. Its global order is based
on those weights. CGAR's copied Warehouse profile instead uses equal weights
everywhere. CGAR already supports an optional remaining-chain order and unequal
rank weights; this is a ready generic experiment. Do not silently copy the
instance-specific exponent switches without a trick gate.

NMS's active `Objects/Environment/environment.cpp`151–161 chooses generated
`GraphGuidance(env)` for Warehouse, Sortation, GAME and CITY; only RANDOM uses
the saved `guidance_map_random.txt`. The graph generator uses different explicit
lane patterns on Sortation versus Warehouse, and a criss-cross grid on GAME/CITY.
These are plausible trick baselines, distinct from CGAR's learned flow. The
unrelated NMS `configs/*.json` LNS files are not evidence for this active path.
Our generic learned field should be compared with a uniform field before assuming
that a stronger lane penalty always helps sparse or irregular maps.

KK's active CMake target builds `wppl_planner`, not the separate `my_planner`
folder. `wppl_planner/src/planner.cpp` reads per-map/density weights and config,
then selects its windowed LNS/LaCAM planner. Its scheduler shares the planner's
metric, reconsiders unopened orders and has instance-specific scheduler options.
Transfer candidates are consistent assignment/movement costs and bounded joint
assignment, followed by longer rolling look-ahead that preserves CGAR's primary
and recovery constraints. Loading KK's fields would be an explicit trick.

RANDOM-05's separate progress log reports successful pipelined reactive planning,
kinematic predictions, multiple future continuations and persistent priority
search. These are useful general ideas for congested RANDOM-04. The separate
solver is not a CGAR result, and no code port or extension of that agent's campaign
is included here. Its map-tuned guidance and known-horizon triage remain tricks.

## Ordered experiments

1. Complete Warehouse strict1s verification and preserve its exact trajectory.
2. Finish the regional-work comparison already running across the other eight
   instances. The candidate count, not the repair-attempt count, captures the
   congestion-dependent cost. This experiment changes prescribed work and may
   reduce throughput; it is not a semantics-preserving speedup.
3. Under the same bounded budget, compare remaining-chain ranking, unopened
   matching, and direct-cost task assignment as separate factors. Throughput is
   primary; retain waiting-time outcomes when fairness worsens. Compare a uniform
   flow field to the learned one without also changing scheduler coverage.
4. Add explicit per-instance NMS/KK guidance baselines only behind new validated
   `--trick <instance>` gates. Start with Sortation's close structural relative
   to Warehouse, then CITY/GAME and RANDOM-01..04.
5. If guidance/assignment fail to close the dense gap, evaluate fixed-work
   multi-future search and longer rolling decisions around CGAR's existing kernel.
   Validate rotations, complete dependency chains, protected paths and timeout
   failures before throughput comparisons.

The historical [reference inventory](reference-inventory.json) needs refreshing:
large-map runs stop at1000steps, NMS has startup timeouts on three cases, all large
NMS/KK runs exceed32decimalGB, and smaller cases use mixed CPU models. New CGAR
scores cannot be called matched improvements against those rows. NMS is the
primary target, KK a secondary reference; published scores remain separate.
