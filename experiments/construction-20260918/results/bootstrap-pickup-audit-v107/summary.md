# Initial Warehouse pickup pairing: measurable but modest physical-distance residual

The two verified p90 / pickup-weight-8 runs have **identical initial positions and assignments**, so the probe computes one unique initial fixture. It keeps the selected 10,000-task set fixed and excludes robot 0 plus the 82 robots already at their pickup. The remaining **9,917 robots** are partitioned into **310 disjoint spatial groups of at most 32**, using geometry without Warehouse-specific grouping coordinates.

| Metric within those fixed groups | Original pairing | Best pairing for that metric | Change |
|---|---:|---:|---:|
| Native guidance cost | 7,593,417 | 7,275,691 | -317,726 (-4.18%) |
| Shortest physical pickup length under the native-optimal pairing | 350,479 | 346,563 | -3,916 (-1.12%) |
| Shortest physical pickup length under its own optimal pairing | 350,479 | 339,381 | -11,098 (-3.17%) |

The existing matching acceptance threshold (four forward-cost units and 10%) retains changes involving **1,113 robots**, with **153,294 native-cost reduction** and only **2,721 reduction in shortest physical pickup length**. Native guidance discourages disruptive opposing traffic, so optimizing physical distance alone is not a validated better policy. These distances assume no robot interference and arrival heading is free. None of these values is saved executed work or predicted throughput. Grouping limits the search; it is not a global assignment bound. The probe does not reconstruct arbitrary live recovery state or assert that every offline permutation could be safely enabled.

The actual initial-task cohort is picked up after a mean 47.83 / 47.98 steps (p90 96, maxima 165 / 191) in seeds 0 / 2. These are task-level elapsed times, potentially involving unopened retargets; they must not be summed as original-holder travel.

The offline dual-metric field and matching computation took **9.145 seconds on four bound physical cores**, with 3.061 billion settled oriented states. Four complete fields match an independent heap Dijkstra implementation cell for cell, and the installed native field fingerprint matches the production asset. Both full input hashes / canonical trajectories and all copied production headers are verified. The recorded child peak RSS is 423 MB including compilation; it is not a solver memory measurement. This is an offline diagnostic, with no production source or scheduling change.

**Decision:** deprioritize an all-robot complete-field startup matching pass. This diagnostic is too expensive for the one-second target and exposes a modest route-length residual within its measured scope. Official preprocessing runs before initial tasks and states are synchronized (`BaseSystem::initialize`), so this task-dependent work cannot simply be moved there. A future sparse startup method would need a separate, bounded feasibility and full-throughput test; the offline result does not justify a broad implementation now.

Verified 2026-09-20T18:55:22.222948+00:00. [Full result and per-group evidence](initial-0-result.json), [input/source checks](verification.json), [allocation](allocation.json), [frozen request](request.json). Requires explicit `--trick WAREHOUSE` even in this diagnostic. The current 155,120 record is unchanged.
