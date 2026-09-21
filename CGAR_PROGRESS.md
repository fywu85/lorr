# CGAR competition progress

Updated 2026-09-21 UTC. The active research scope is all ten LoRR2024 instances;
NMS is the primary target. Throughput comes first and fairness is a secondary,
reported metric. The latest focused work transfers ideas from the independent
RANDOM-05 solver into CGAR without modifying that solver's working tree.

| Instance | Selected full-run tasks | Evidence status |
|---|---:|---|
| WAREHOUSE | 155173 | TRICK; strict1s seeds0/2, max945/958ms |
| SORTATION | 150353 | TRICK; seed0,5s development, max1152ms; runtime work remains |
| CITY-01 | 7305 | Generic; full5s development |
| CITY-02 | 14068 | Generic; full5s development |
| GAME | 6519 | Generic; full5s development |
| RANDOM-01 | 611 | Generic; full5s development |
| RANDOM-02 | 1084 | Generic; full5s development |
| RANDOM-03 | 1484 | Generic; full5s development |
| RANDOM-04 | 1503 | Generic; strict1s, direct pickup4 +64-group matching, seed0 |
| RANDOM-05 | 2608 | TRICK; strict1s, tuned field +direct pickup4 +matching, seed2 |

These are individual maxima with different explicit configurations, not one
universal preset. [Timestamped best history and source commits](experiments/allmaps-20260920/BEST_HISTORY.md),
[exact selected records](experiments/allmaps-20260920/selected-full-results.json),
[published NMS targets](experiments/allmaps-20260920/TARGETS.md),
[Warehouse history](WAREHOUSE_PROGRESS.md). Published targets are not fresh matched
local controls. RANDOM-05's selected2608 remains14.49% below the historical3050 target.

The dense-map transfers are measured, not inferred from the independent solver:

* General priority portfolios improved both maps. Fresh priority noise helped
  RANDOM-05 across three planner seeds; retaining old priority vectors was weaker.
* The explicitly flagged integer guidance field improved RANDOM-05's mean from
  2032.67 to2478.33 (+21.93%). It did not improve RANDOM-04's mean.
* Direct pickup weighting plus existing unopened-task matching improved all
  three tested pairs. RANDOM-04 scores1503/1480/1494, mean1492.33 versus1301.33
  control (+14.68%). RANDOM-05 scores2574/2608/2528, mean2570 versus2478.33 field
  control (+3.70%), or26.43% above the earlier generic mean2032.67.

All these dense replications use full horizons, enforced1000ms decisions and
four bound physical cores per process on shared EPYC9354 hosts. The selected
profiles average about100ms, peak below171ms and use under0.2GB RAM. There were
no solver timeouts or simulator errors in the reported accepted matrices.
Planner seeds vary on the same archived task/start stream; they are not
independent input streams. [Exact dense configurations and evidence](experiments/allmaps-20260920/random-transfer/BEST.md).

CGAR's primary/recovery protections and complete-work timeout behavior remain.
The motion kernel still includes the documented NMS-derived temporal PIBT code.
The borrowed RANDOM field and scheduling settings do not replace CGAR with the
standalone solver. All field use requires `--trick RANDOM-04` or `--trick RANDOM-05`.
There is no implicit map dispatch or known-horizon rule in the dense selected profiles.

Fairness limitations remain explicit. In the2036-task RANDOM-05 control, the
capacity policy permanently excludes427 of431 never-assigned tasks because
one or more stops lie outside the certified core. Disabling that certificate
reduced throughput on both maps; it remains enabled. The structural exclusion
still exists in the selected configuration. [Certificate/event audit](experiments/allmaps-20260920/random-transfer/capacity-audit-v1/audit.json),
[negative ablation](experiments/allmaps-20260920/results/dense-capacity-full-v1/summary.md).

The first general movement-promise prototype has now been tested in full runs.
The2574-task run has105055 immediate inverse-turn pairs away from task/goal
changes (18.22% of eligible turn transitions). A movement promise must yield to
CGAR's protected paths and preserve collision validity; this diagnostic alone
does not establish a throughput gain. [Source review and transfer roadmap](experiments/allmaps-20260920/random-transfer/REVIEW.md).

The after-turn promise prototype and the explicit removal of forced-oldest
admissions both lost their paired seed0 controls. Generic RANDOM-04 changed
1503->1388 with promises. Field-guided RANDOM-04 scored1481/1392/1346/1334
(control/promises/short preference/both); RANDOM-05 scored2574/2548/2539/2546.
All ten runs were valid under1s, and disabled controls preserved whole trajectories.
Selected bests are unchanged. [Complete negative result](experiments/allmaps-20260920/random-transfer/MOVEMENT_PROMISE_DESIGN.md).

The parallel trick track now includes explicit NMS CITY/GAME guidance providers,
verified against the unchanged archived constructor and independent formulas.
Their full CGAR tests/build and strict1s field/admission comparisons are underway.
They require `--trick CITY-01`, `--trick CITY-02` or `--trick GAME`; no throughput
gain is claimed before the complete measurements.
[Declared CITY/GAME comparison](experiments/allmaps-20260920/city-game/README.md).
