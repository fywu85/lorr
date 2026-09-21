# CGAR cross-instance selected throughput history

Throughput is primary. These full runs use shared EPYC9354hosts,4physical cores per case,5s development deadlines and32decimalGB. All are seed0; selected maxima are not multi-seed means. Timestamps are the completed result-file mtime in UTC. Warehouse keeps its existing detailed [history](../../WAREHOUSE_PROGRESS.md).

| Instance | Tasks | Completed UTC | Source | Profile | Evidence |
|---|---:|---|---|---|---|
| SORTATION | 146460 | 2026-09-20T23:52:12.265422+00:00 | [eb99380](https://github.com/fywu85/lorr/commit/eb9938090d7cfb331fdc124ce19b475626356502) | generic_regions_4m | [verification](results/region-budget-full-v1/verification.json) |
| CITY-01 | 7305 | 2026-09-20T23:24:50.131648+00:00 | [eb99380](https://github.com/fywu85/lorr/commit/eb9938090d7cfb331fdc124ce19b475626356502) | generic_regions_4m | [verification](results/region-budget-full-v1/verification.json) |
| CITY-02 | 14068 | 2026-09-20T23:25:50.512240+00:00 | [eb99380](https://github.com/fywu85/lorr/commit/eb9938090d7cfb331fdc124ce19b475626356502) | generic_regions_4m | [verification](results/region-budget-full-v1/verification.json) |
| GAME | 6519 | 2026-09-20T23:37:24.975038+00:00 | [eb99380](https://github.com/fywu85/lorr/commit/eb9938090d7cfb331fdc124ce19b475626356502) | generic_regions_4m | [verification](results/region-budget-full-v1/verification.json) |
| RANDOM-01 | 611 | 2026-09-20T23:54:35.058673+00:00 | [eb99380](https://github.com/fywu85/lorr/commit/eb9938090d7cfb331fdc124ce19b475626356502) | generic_match64_direct | [verification](results/generic-factors-random-full-v1/verification.json) |
| RANDOM-02 | 1084 | 2026-09-20T23:54:29.967399+00:00 | [eb99380](https://github.com/fywu85/lorr/commit/eb9938090d7cfb331fdc124ce19b475626356502) | generic_match64_direct | [verification](results/generic-factors-random-full-v1/verification.json) |
| RANDOM-03 | 1484 | 2026-09-20T23:54:59.575718+00:00 | [eb99380](https://github.com/fywu85/lorr/commit/eb9938090d7cfb331fdc124ce19b475626356502) | generic_match64_direct | [verification](results/generic-factors-random-full-v1/verification.json) |
| RANDOM-04 | 1249 | 2026-09-20T23:53:10.296974+00:00 | [eb99380](https://github.com/fywu85/lorr/commit/eb9938090d7cfb331fdc124ce19b475626356502) | generic_chain_rank | [verification](results/generic-factors-random-full-v1/verification.json) |

CITY-01 reconfirms the previous7305score; the other rows improve or supply a complete result for the current campaign. The earlier uncapped RANDOM-04 profile scored997; the new selected1249uses different bounded work. Preserve those distinct configurations.

[Machine-readable settings, latency, memory and trajectory fingerprints](selected-full-results.json).

## First full CGAR RANDOM-05 results

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 1534 | 2026-09-21T00:06:56.467303+00:00 | [eb99380](https://github.com/fywu85/lorr/commit/eb9938090d7cfb331fdc124ce19b475626356502) | generic_control | [verification](results/random05-baseline-full-v1/verification.json) |
| 1847 | 2026-09-21T00:06:57.808224+00:00 | [eb99380](https://github.com/fywu85/lorr/commit/eb9938090d7cfb331fdc124ce19b475626356502) | generic_chain_rank | [verification](results/random05-baseline-full-v1/verification.json) |

These are CGAR runs in separate directories. The independent RANDOM-05 solver and its records are untouched.

## Dense priority portfolio, first full seed

| Instance | Tasks | Completed UTC | Source | Profile | Evidence |
|---|---:|---|---|---|---|
| RANDOM-04 | 1330 | 2026-09-21T00:08:40.438857+00:00 | [5d3c5bf](https://github.com/fywu85/lorr/commit/5d3c5bf51147d9739de9c457a679d727e8492848) | generic_chain_rank_8workers | [verification](results/priority-portfolio-dense-full-v1/verification.json) |
| RANDOM-05 | 1989 | 2026-09-21T00:10:19.069986+00:00 | [5d3c5bf](https://github.com/fywu85/lorr/commit/5d3c5bf51147d9739de9c457a679d727e8492848) | generic_chain_rank_8workers | [verification](results/priority-portfolio-dense-full-v1/verification.json) |
| RANDOM-05 | 2036 | 2026-09-21T00:13:41.663822+00:00 | [5d3c5bf](https://github.com/fywu85/lorr/commit/5d3c5bf51147d9739de9c457a679d727e8492848) | generic_noise50_cold | [verification](results/priority-portfolio-dense-full-v1/verification.json) |

Persistence itself did not beat the selected controls. RANDOM-04 favors the eight-worker control; RANDOM-05 favors noise50without retention on this seed. Full strict1sseed0/2/4replication is predeclared and pending. Both objective changes (remaining potential and partial next errand) lost and stay off.

## Explicit SORTATION native lanes

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 149321 | 2026-09-21T00:32:05.568136+00:00 | [df94a52](https://github.com/fywu85/lorr/commit/df94a520c77fcb3a73a4a8c74b5b13cab542d1d6) | trick_sortation_native_bands0 | [verification](results/sortation-native-full-v1/verification.json) |
| 150353 | 2026-09-21T00:34:06.943933+00:00 | [df94a52](https://github.com/fywu85/lorr/commit/df94a520c77fcb3a73a4a8c74b5b13cab542d1d6) | trick_sortation_native_bands1 | [verification](results/sortation-native-full-v1/verification.json) |

Both are **TRICK**, seed0, full 5000 steps, four physical cores and enforced 5s development deadlines. The 150353 result is 2.66% above the unchanged 146460 control and 1.55% below the historical NMS target 152714. Mean/max decision time 524.64/1151.94 ms; it is not yet qualified at 1s. The control whole trajectory matches the previous generic source exactly.

## Strict one-second dense-map replication

All 18 full runs passed with planner seeds0/2/4. The seed0 whole trajectories are unchanged from 5s development. These are three planner seeds on each archived instance, not independent task/start streams.

| Instance | Tasks | Seed | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|
| RANDOM-04 | 1367 | 4 | 2026-09-21T00:42:58.452817+00:00 | [5d3c5bf](https://github.com/fywu85/lorr/commit/5d3c5bf51147d9739de9c457a679d727e8492848) | generic_noise50_cold |

Mean tasks (one worker / eight workers / eight with noise50): RANDOM-04 **1235.67 / 1307.00 / 1301.33**; RANDOM-05 **1829.67 / 1960.33 / 2032.67**. Every paired eight-worker run beats one worker. Noise consistently helps RANDOM-05 versus eight workers, but is mixed on RANDOM-04. The selected RANDOM-04 maximum is a seed4 result, not the best mean. Maximum decision time across all 18 runs is 307.61 ms. [Evidence](results/priority-portfolio-strict-seeds-v2/summary.md), [whole-trajectory confirmation and means](results/priority-portfolio-strict-seeds-v2/replication.json).

## First explicit RANDOM guidance transfer

| Instance | Tasks | Completed UTC | Source | Profile |
|---|---:|---|---|---|
| RANDOM-04 | 1353 | 2026-09-21T00:56:18.761005+00:00 | [8352226](https://github.com/fywu85/lorr/commit/83522266d6ae8c88241d11e91a125e30848cbca9) | trick_random_field, --trick RANDOM-04 |
| RANDOM-05 | 2457 | 2026-09-21T00:57:55.998209+00:00 | [8352226](https://github.com/fywu85/lorr/commit/83522266d6ae8c88241d11e91a125e30848cbca9) | trick_random_field, --trick RANDOM-05 |

RANDOM-05 sets the new overall selected CGAR best, **2457** versus2036on the same seed (+20.68%). RANDOM-04 reaches1353versus1306on seed0 (+3.60%); its generic seed4record1367 remains higher. These are full **strict1s TRICK** runs, four bound physical cores, shared EPYC9354; maximum field-profile decision times136.28/177.58ms. The uniform forward20/turn6 controls score1205/1941, so the nonuniform field contributes148/516tasks relative to the same scalar convention. One seed only; this is still19.44%below the historical NMS RANDOM-05 target3050. No cutoff or short-task trick was enabled. Both lanes-off whole trajectories exactly reproduce their earlier generic controls. [RANDOM-04](results/random04-guidance-full-v1/summary.md), [RANDOM-05](results/random05-guidance-full-v1/summary.md).

## Replicated guidance and scheduling transfer

The field versus generic means over planner seeds0/2/4 are **1299.00 vs1301.33**
on RANDOM-04 (-0.18%) and **2478.33 vs2032.67** on RANDOM-05 (+21.93%).
Thus the tuned field helps RANDOM-05 consistently but does not improve RANDOM-04
on average. All full runs passed strict1s. [Replication](random-transfer/guidance/three-seed-summary.json).

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| RANDOM-04 | 1481 | 0 | TRICK | 2026-09-21T01:04:28.116168+00:00 | [8352226](https://github.com/fywu85/lorr/commit/83522266d6ae8c88241d11e91a125e30848cbca9) | trick_direct_pickup4 |
| RANDOM-04 | 1503 | 0 | GENERIC | 2026-09-21T01:04:31.682939+00:00 | [8352226](https://github.com/fywu85/lorr/commit/83522266d6ae8c88241d11e91a125e30848cbca9) | generic_match64_direct_pickup4 |
| RANDOM-05 | 2537 | 2 | TRICK | 2026-09-21T01:04:44.326741+00:00 | [8352226](https://github.com/fywu85/lorr/commit/83522266d6ae8c88241d11e91a125e30848cbca9) | trick_random_field |
| RANDOM-05 | 2574 | 0 | TRICK | 2026-09-21T01:06:30.020067+00:00 | [8352226](https://github.com/fywu85/lorr/commit/83522266d6ae8c88241d11e91a125e30848cbca9) | trick_match64_direct_pickup4 |

The new scheduling profiles are still seed0 results. RANDOM-04 selects **generic1503** (direct pickup weight4 +64-group unopened matching), above the best field profile1481. RANDOM-05 selects **TRICK2574** (same scheduling combination plus the field), versus field control2457; generic matching/direct reaches2068. New selected maxima stayed below171ms. Matching alone with the field gives2544; direct pickup4alone gives2421. Interactions matter: the combined field/scheduling profile loses on RANDOM-04, so there is no universal preset claim. [Generic factors](results/dense-scheduler-generic-full-v1/summary.md), [RANDOM-04 field factors](results/random04-scheduler-field-full-v1/summary.md), [RANDOM-05 field factors](results/random05-scheduler-field-full-v1/summary.md). All control trajectories match prior runs; fairness remains secondary and recorded.

## Scheduling replication, all ten added runs valid

| Instance | Tasks | Seed | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|
| RANDOM-05 | 2608 | 2 | 2026-09-21T01:14:54.037778+00:00 | [8352226](https://github.com/fywu85/lorr/commit/83522266d6ae8c88241d11e91a125e30848cbca9) | trick_match64_direct_pickup4, --trick RANDOM-05 |

RANDOM-04 generic selected profile scores1503/1480/1494, mean1492.33 versus1301.33 control (+14.68%). RANDOM-05 selected trick scores2574/2608/2528, mean2570 versus2478.33 field control (+3.70%); matching-only scores2544/2475/2532, mean2517. Overall selected R05 mean is26.43% above the earlier generic mean2032.67. Every selected-profile pair improves its control, but matching-only is mixed. The new2608 best is14.49% below historical NMS3050. [Full three-seed summary](random-transfer/scheduling-three-seed-summary.json).
