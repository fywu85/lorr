# CGAR cross-instance selected throughput history

Throughput is primary. Resource limits, planner seeds, and deadlines vary by the explicitly recorded phase below. Current selected runs all enforce1000ms and32decimalGB; eight physical cores for WAREHOUSE/SORTATION and four for other instances. Selected maxima are not multi-seed means. Timestamps are UTC completion times: earlier entries use result-file mtime; newer entries use suite metadata `finished_utc`, as recorded in the selected-result JSON. Warehouse keeps its existing detailed [history](../../WAREHOUSE_PROGRESS.md).

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

## 2026-09-21 dense follow-ups: bests unchanged

Source [aa63d48](https://github.com/fywu85/lorr/commit/aa63d48cdb75f61774474faad025f049cc631f76),
all14 added full seed0 cases valid under1s. The after-turn promise, explicit
short-task preference, their combination, and neutral service-tail scoring all
lost their respective paired controls. None updates the best table: RANDOM-04
remains generic1503, RANDOM-05 remains TRICK2608 on seed2. Complete disabled
control trajectories matched their earlier source exactly. [Promise/admission
results](random-transfer/promise-summary.json), [service-tail check](random-transfer/neutral-service-summary.json).

CITY-01/02 and GAME now have explicit NMS guidance providers with full regression
proof; their full1s throughput comparisons are running. No pending result is
entered as a new best. [Declared comparisons](city-game/README.md),
[parallel general/trick roadmap](TRICK_ROADMAP.md).

## First strict CITY guidance improvement

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| CITY-01 | 7755 | 0 | TRICK | 2026-09-21T02:11:25.104932+00:00 | [596b609](https://github.com/fywu85/lorr/commit/596b609351f4653dcb404e264edb4f20de98d7eb) | trick_adapted_grid, --trick CITY-01 |

The adapted4/16 NMS field improves7305->7755 (+6.16%), still7.90% below published
NMS8420. All four full3000-step cases pass strict1s and32decimalGB on four cores.
The selected profile's maximum decision is764.89ms. Its HRRN and forced-oldest
admissions remain enabled; no new fairness relaxation is bundled into this win.
Native20/200/pure-potential scores6815; native+short preference6669. Therefore the
raw native convention is not the selected CITY transfer. The lanes-off control
exactly reproduces the previous generic whole trajectory. One planner seed.
[Full comparison](results/city-01-native-full-v1/summary.md),
[selection/control proof and exact config](city-game/city-01-selected.json).

## Strict CITY-02 guidance improvement

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| CITY-02 | 14851 | 0 | TRICK | 2026-09-21T02:11:18.277003+00:00 | [596b609](https://github.com/fywu85/lorr/commit/596b609351f4653dcb404e264edb4f20de98d7eb) | trick_adapted_grid, --trick CITY-02 |

The adapted field improves14068->14851 (+5.57%), still11.53% below published
NMS16787. All four full3000-step cases pass1s and32decimalGB on four cores.
The selected maximum is807.04ms. Native field14185 and native+short14265 also
beat this seed's control, but remain below the adapted field. Selected HRRN and
forced-oldest admissions stay enabled; whole control trajectory unchanged.
[Full results](results/city-02-native-full-v1/summary.md),
[selection and exact config](city-game/city-02-selected.json).

## Strict GAME guidance improvement

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| GAME | 10080 | 0 | TRICK | 2026-09-21T02:21:10.956432+00:00 | [596b609](https://github.com/fywu85/lorr/commit/596b609351f4653dcb404e264edb4f20de98d7eb) | trick_adapted_grid, --trick GAME |

Adapted guidance improves6519->10080 (+54.62%), still56.69% below published
NMS23274. All four full5000-step cases pass strict1s and32decimalGB. The selected
maximum is904.65ms, RSS10.51GB, four physical cores. The control reproduces the
previous whole trajectory. Native6784 and native+short6758 underperform the
adaptation. HRRN and forced-oldest admission remain; outstanding-task age p90
is still5000steps. One planner seed, no matched NMS claim.
[Full results](results/game-native-full-v1/summary.md),
[selection and exact config](city-game/game-selected.json).

## CITY-01 scheduling improvement on adapted guidance

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| CITY-01 | 8213 | 0 | TRICK | 2026-09-21T02:41:14.968305+00:00 | [85c3f0f](https://github.com/fywu85/lorr/commit/85c3f0fff5acc76b1ec7db7155fa48ebb0cedf12) | trick_adapted_dispatch, --trick CITY-01 |

Direct pickup4 +HRRN0 +64-group unopened matching improves7755->8213 (+5.91%),
12.43% above the original7305 and2.46% below published NMS8420. Full3000steps,
strict1s, max758.46ms, RSS8.11GB. The exact7755 control is reproduced by the same
source/binary. Ranking alone7739 and ranking+dispatch8204 do not beat their paired
equal-weight controls. Two original dispatch profiles used an incompatible
generic selector and were rejected before simulation; corrected explicit-selector
profiles are independently frozen and verified. Outstanding-task age p90 rises
1134->2553steps; throughput is primary, with this fairness tradeoff recorded.
[Selection and exact configuration](city-game/city-01-dispatch-selected.json).

## Explicit rank-weighting records on RANDOM-04

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| RANDOM-04 | 1622 | 0 | TRICK | 2026-09-21T02:44:32.539872+00:00 | [a573aa5](https://github.com/fywu85/lorr/commit/a573aa5c0aae29de296c38ab1b679d5d8602800e) | trick_squared_rank, lanesOFF |
| RANDOM-04 | 1645 | 2 | TRICK | 2026-09-21T02:49:22.119755+00:00 | [a573aa5](https://github.com/fywu85/lorr/commit/a573aa5c0aae29de296c38ab1b679d5d8602800e) | trick_squared_rank, lanesOFF |

Full1000steps, strict1s, four physical cores and<0.2GB. Squared rank tasks
1622/1645/1512 versus1503/1480/1494 over seeds0/2/4, mean1593.00 vs1492.33 (+6.75%).
All three pairs improve, though the seed4 gain is small. Old controls reproduce
exact whole trajectories. New overall best1645 seed2 is35.41% below historical
NMS2547; best generic1503 remains separate. No known horizon or new field was
added. Stronger priority weighting is explicitly a trick and is not a fairness
guarantee. [Selection/resources/waiting](rank-trick/random04-selected.json).

## CITY-01 follow-up: 8378 tasks

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| CITY-01 | 8378 | 0 | TRICK | 2026-09-21T02:57:33.518693+00:00 | [85c3f0f](https://github.com/fywu85/lorr/commit/85c3f0fff5acc76b1ec7db7155fa48ebb0cedf12) | trick_dispatch_pickup8 |

Full3000steps and strict1s, four physical cores. Previous8213 control exactly reproduced. Improvement2.01%, still0.50% below published NMS8420. Selected max742.32ms, RSS8.11GB, outstanding-task age p90=2515steps. [Full comparison](results/city-01-dispatch-tuning-v1/summary.md).

## CITY-02 follow-up: 15797 tasks

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| CITY-02 | 15797 | 0 | TRICK | 2026-09-21T02:57:28.288454+00:00 | [85c3f0f](https://github.com/fywu85/lorr/commit/85c3f0fff5acc76b1ec7db7155fa48ebb0cedf12) | trick_adapted_dispatch |

Full3000steps and strict1s, four physical cores. Previous14851 control exactly reproduced. Improvement6.37%, still5.90% below published NMS16787. Selected max809.78ms, RSS8.50GB, outstanding-task age p90=2623steps. [Full comparison](results/city-02-adapted-scheduling-v1/summary.md).

## GAME scheduling on the adapted field

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| GAME | 11146 | 0 | TRICK | 2026-09-21T03:03:53.328457+00:00 | [596b609](https://github.com/fywu85/lorr/commit/596b609351f4653dcb404e264edb4f20de98d7eb) | trick_adapted_direct_match64 |

Full5000steps, strict1s, max893.90ms. Direct pickup4/HRRN0/matching64 improves
10080->11146 (+10.58%), still52.11% below published NMS23274. All four full
variants pass; short preference10540 and direct-without-matching10765 are weaker.
Whole control trajectory unchanged. The winner retains forced-oldest admission.
[Selected config/resources/waiting](city-game/game-dispatch-selected.json).

## GAME priority-weighting record

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| GAME | 14664 | 0 | TRICK | 2026-09-21T03:05:32.672834+00:00 | [a573aa5](https://github.com/fywu85/lorr/commit/a573aa5c0aae29de296c38ab1b679d5d8602800e) | trick_adapted_squared_chain |

Adapted-field equal weights10080, linear current-goal ranks13314, squared ranks
14471, squared remaining-chain ranks14664. Full5000steps, all four strict1s and
whole control unchanged. Selected max896.57ms, still37.00% below published NMS
23274. The previous11146 scheduling branch is a different configuration; its
benefit must be measured again on these ranks. One planner seed.
[Selection and resources/waiting](city-game/game-rank-selected.json).

## First sparse RANDOM reference fields

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| RANDOM-01 | 613 | 0 | TRICK | 2026-09-21T03:13:40.081729+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | trick_nms_arrows |
| RANDOM-02 | 1123 | 0 | TRICK | 2026-09-21T03:13:26.330631+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | trick_nms_arrows |
| RANDOM-03 | 1541 | 0 | TRICK | 2026-09-21T03:13:57.251324+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | trick_kk_forward |

Full600/600/800steps, strict1s, four cores, all eleven cases valid and all three
generic controls whole-trajectory identical. NMS arrows give613/1123/1421; KK
forward gives1120/1541 on RANDOM02/03. Native uniform controls609/1081/1150 show
that metric/scoring convention alone does not explain the field gains. Select
613/1123/1541 over prior611/1084/1484. RANDOM01 gains only2tasks on this one seed,
so no robust improvement claim. RANDOM03 is a forward-only KK adaptation with
uniform turn20, not the original per-cell stay costs.
[Selections and controls](random-reference/sparse-selected.json).

## Further CITY records and a general RANDOM-03 improvement

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| CITY-01 | 8386 | 6 | TRICK | 2026-09-21T03:16:47.853847+00:00 | [85c3f0f](https://github.com/fywu85/lorr/commit/85c3f0fff5acc76b1ec7db7155fa48ebb0cedf12) | trick_pickup8 |
| CITY-02 | 16159 | 0 | TRICK | 2026-09-21T03:17:45.633819+00:00 | [85c3f0f](https://github.com/fywu85/lorr/commit/85c3f0fff5acc76b1ec7db7155fa48ebb0cedf12) | trick_dispatch_pickup12 |
| RANDOM-03 | 1613 | 0 | GENERAL | 2026-09-21T03:21:00.367426+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | generic_chain_ranks |

All full horizons, strict1s, four physical cores per run, below32decimalGB.
CITY01 seeds0/2/4/6 are8378/8367/8374/8386; the selected seed6 is34tasks below
published8420, a best-seed record rather than a matched configuration gain.
CITY02 pickup12 reaches16159 from15797 (+2.29%), max825.46ms; the full control
is identical. RANDOM03 general remaining-chain ranks reach1613 from1484
(+8.69%), max191.37ms, overtaking the1541 field result. The same rank change
slightly loses on RANDOM01/02, so this is not a universal improvement.
RANDOM01 generic portfolio ties613 with mean74.75ms and max136.31ms; prefer
that generic configuration at equal throughput while retaining the earlier
field record. [General comparison](sparse-search/full-comparison.json),
[CITY01 selection](city-game/city-01-pickup8-seeds-selected.json),
[CITY02 selection](city-game/city-02-pickup12-selected.json).

## Combining general changes with explicit fields

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| RANDOM-01 | 621 | 0 | TRICK | 2026-09-21T03:33:30.391404+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | trick_nms_pickup4 |
| RANDOM-02 | 1160 | 0 | TRICK | 2026-09-21T03:33:17.028428+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | trick_nms_pickup4 |
| RANDOM-03 | 1890 | 0 | TRICK | 2026-09-21T03:33:55.104927+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | trick_kk_chain_pickup4 |

RANDOM01/02 pickup4 on NMS arrows reaches621/1160, compared with613/1123.
RANDOM03 chain ranks with KK forward guidance reaches1841, or1890with pickup4,
compared with the1613 generic chain-rank control (+17.17%). All full600/600/800
steps pass1s; selected maxima201.28/185.37/197.97ms. All whole controls reproduce.
Keep generic613/1084/1613 separately. These are one-seed selected configurations;
published NMS639/1221/2334 remains ahead and is not a matched local comparison.
[Selection and resources/waiting](sparse-search/field-interaction-selected.json).

## CITY-01 reaches its historical target; SORTATION qualifies under one second

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| CITY-01 | 8423 | 0 | TRICK | 2026-09-21T03:44:40.064675+00:00 | [85c3f0f](https://github.com/fywu85/lorr/commit/85c3f0fff5acc76b1ec7db7155fa48ebb0cedf12) | trick_pickup12 |
| CITY-02 | 16169 | 0 | TRICK | 2026-09-21T03:47:14.560995+00:00 | [85c3f0f](https://github.com/fywu85/lorr/commit/85c3f0fff5acc76b1ec7db7155fa48ebb0cedf12) | trick_pickup12_pickup_groups |
| GAME | 15574 | 0 | TRICK | 2026-09-21T03:37:42.272217+00:00 | [a573aa5](https://github.com/fywu85/lorr/commit/a573aa5c0aae29de296c38ab1b679d5d8602800e) | trick_squared_adapted_dispatch |
| RANDOM-03 | 1902 | 0 | TRICK | 2026-09-21T03:40:42.326525+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | trick_global8m |
| SORTATION | 150333 | 0 | TRICK | 2026-09-21T03:43:08.098400+00:00 | [df94a52](https://github.com/fywu85/lorr/commit/df94a520c77fcb3a73a4a8c74b5b13cab542d1d6) | trick_sortation_global2000000 |

CITY01 reaches8423, three above published NMS8420; both pickup12 and16 score
8423 on seed0. Select12 at the lower measured peak759.54ms. This is a historical
threshold match, not a matched local superiority claim. CITY02 pickup grouping
gives16169, only10 above16159 on one seed. GAME dispatch combined with squared
chain ranks gives15574 vs14664 (+6.21%), max911.11ms. Native fields still lose
10429/10471. Whole CITY/GAME controls are identical.

RANDOM03 global8M gives1902 vs1890 (+0.63%);16M=1810 and four proposals=1868.
This small seed0 record does not establish robust scaling. The separate1890
bundle replicates as1763/1684 on seeds2/4, versus generic1794/1614; all-three
means1779.00 vs1673.67 (+6.29%), two of three positive. Generic best1794seed2
remains separately documented.

SORTATION global2M with8physical cores finishes150333, mean452.13ms, max972.92ms,
RSS12.46GB. Global1M finishes150284, max887.27ms. Both full5000-step cases pass
strict1s. The previous150353 development maximum used5s and peaked1152ms; keep
it as an unqualified development record. Select150333 for the strict profile,
a20-task reduction (0.0133%) and1.56% below published NMS152714.
[Exact selected records](latest-selected-20260921-0355.json).

## More regional repair improves the RANDOM-05 best

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| RANDOM-05 | 2684 | 0 | TRICK | 2026-09-21T03:54:58.365892+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | trick_region8m |

Eight million regional candidates per batch improve the unchanged2574 control
to2684 (+4.27%), above the previous2608 seed2 maximum. Full2000steps, strict1s,
mean158.80ms, max227.23ms, four cores,0.193GB. More global work loses2544/2559.
RANDOM04's regional/global expansions all lose its1622seed0 control. Replication
and larger regional budgets are underway; no broad scaling claim yet.
[Selected resources and exact control](fixed-work-scaling/random05-region-selected.json).

## GAME fleet selection and further full-run gains

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| GAME | 21742 | 0 | TRICK | 2026-09-21T04:14:09.803081+00:00 | [f644acc](https://github.com/fywu85/lorr/commit/f644accc7e00ceda7a455b7a45ed6eee4f970f8d) | trick_uniform2750 |
| RANDOM-05 | 2704 | 0 | TRICK | 2026-09-21T04:08:34.604466+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | trick_region16m |
| RANDOM-05 | 2805 | 0 | TRICK | 2026-09-21T04:16:40.222099+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | trick_region32m |
| RANDOM-05 | 2806 | 0 | TRICK | 2026-09-21T04:08:53.770216+00:00 | [acaf634](https://github.com/fywu85/lorr/commit/acaf634b537ff655715aae28ed43056e0c62a1de) | trick_region8m_rounds4 |
| RANDOM-02 | 1188 | 0 | TRICK | 2026-09-21T04:10:03.556947+00:00 | [aad422f](https://github.com/fywu85/lorr/commit/aad422ff1160e1daae02eccf575209da1e4edd3a) | trick_budget4_cadence1 |
| CITY-02 | 16315 | 0 | TRICK | 2026-09-21T04:21:34.227113+00:00 | [aad422f](https://github.com/fywu85/lorr/commit/aad422ff1160e1daae02eccf575209da1e4edd3a) | trick_budget2 |
| CITY-01 | 8424 | 2 | TRICK | 2026-09-21T04:18:51.719140+00:00 | [85c3f0f](https://github.com/fywu85/lorr/commit/85c3f0fff5acc76b1ec7db7155fa48ebb0cedf12) | trick_pickup12 |
| CITY-01 | 8427 | 2 | TRICK | 2026-09-21T04:18:56.359058+00:00 | [85c3f0f](https://github.com/fywu85/lorr/commit/85c3f0fff5acc76b1ec7db7155fa48ebb0cedf12) | trick_pickup16 |

GAME uniform2750 raises15574 to21742 (+39.60%), compared with21648 for the
KK-tabu2750 adaptation and17771 for tabu4000. Full5000steps, strict1s,
max835.91ms, RSS10.562GB. All6500robots remain physically present;3750receive
no new tasks. The independent mask and assignment audit passes. At1000steps
this variant was15.46% worse; the full gain appears late. Do not screen this
mechanism using that prefix. The disabled whole control is identical to its
previous source. This is a fairness-affecting TRICK, not a generic gain.

RANDOM05 additional regional work gives2704 at16M,2805 at32M, and2806 with
8M across four rounds (max343.55ms). Finite repeated unopened-task matching
separately gives2745/2723/2746 for budgets2/4/4+cadence1 versus2684control.
RANDOM02 gets1188 with budget4+cadence1 versus1160. CITY02 budget2 gets16315
versus16169. RANDOM04 all variants lose1622; retain1645seed2overall.
General mechanisms were evaluated on explicit field/rank presets; these are
not claims that every generic configuration improves. Entire controls match.

CITY01 pickup12 scores8423/8424/8414/8422 on seeds0/2/4/6; pickup16 scores
8423/8427/8425/8405. The8427seed2 is the selected maximum, only7tasks above
historical NMS8420. No matched local superiority or20%margin claim.

The first analysis attempts retained historical constants of one retarget and
eight regional batches. Corrected checks use the frozen declared budgets,
validate actual schedules and repeat cooldown, and retain the post-pickup
ownership prohibition. Original failed attempts and corrected receipts remain.
[Exact records and control checks](latest-selected-20260921-0425.json).

## RANDOM-04 regional round comparison

| Tasks | Seed | Track | Completed UTC | Source | Profile |
|---:|---:|---|---|---|---|
| 1743 |0|TRICK|2026-09-21T04:29:10.811360+00:00|[aad422f](https://github.com/fywu85/lorr/commit/aad422ff1160e1daae02eccf575209da1e4edd3a)|trick_round4|

Four rounds give1743 versus1622 with two (+7.46%). Eight rounds give1656;
two parts/four rounds1610. Full1000steps, strict1s; max231.06ms.
The whole control is identical. This is the new seed0maximum, above1645seed2.
[Verification](results/random04-region-rounds-full-v1/verification.json).

## RANDOM-05 eight regional rounds

| Tasks | Seed | Track | Completed UTC | Source | Profile |
|---:|---:|---|---|---|---|
| 2877 |0|TRICK|2026-09-21T04:44:57.007159+00:00|[aad422f](https://github.com/fywu85/lorr/commit/aad422ff1160e1daae02eccf575209da1e4edd3a)|trick_round8_region8m|

Eight rounds of8M give2877 versus2806 with four (+2.53%), mean553.99ms,
max628.73ms, RSS0.194GB.16M/four rounds2810; two-retarget/four rounds2778;
four-retarget/cadence1/four rounds2815. All five complete2000-step runs pass
strict1s and the unchanged control matches. Current best is5.67%below historical
NMS3050. [Verification](results/random05-region-combined-full-v1/verification.json).

## Replicated GAME fleet and RANDOM-04 field interaction

| Instance | Tasks | Seed | Track | Completed UTC | Source | Profile |
|---|---:|---:|---|---|---|---|
| GAME | 21752 | 4 | TRICK | 2026-09-21T04:44:47.638620+00:00 | [f644acc](https://github.com/fywu85/lorr/commit/f644accc7e00ceda7a455b7a45ed6eee4f970f8d) | trick_uniform2750 |
| RANDOM-04 | 1801 | 0 | TRICK | 2026-09-21T04:48:05.260731+00:00 | [aad422f](https://github.com/fywu85/lorr/commit/aad422ff1160e1daae02eccf575209da1e4edd3a) | trick_kk_turn6 |

GAME fleet2750 scores21742/21720/21752 versus15574/15822/15947, all three
planner seeds improve. Mean gain is37.75%. Smaller fleets2000/2250/2500 lose with
17341/18910/20392. Higher fleet sizes3000/3250/3500 are now predeclared.
The new GAME maximum21752 uses seed4; source f644acc still has the preserved
build attestation. RANDOM04 KK forward field plus uniform turn6 and four regions
reaches1801, while standalone field turns6/12 lose1568/1449. This is a CGAR
adaptation, not KK's exact variable stay costs. RANDOM03 turn6/12/32 all lose its
1902 turn20control, so keep20. Every changed factor retains its full control.
[New records](latest-selected-20260921-0455.json), [GAME replication](game-fleet/three-seed-summary.json).

## RANDOM-04 lower turn price on the KK forward field

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 1947 | 2026-09-21T04:54:00.188843+00:00 | [aad422f](https://github.com/fywu85/lorr/commit/aad422ff1160e1daae02eccf575209da1e4edd3a) | TRICK, turn2, seed0 | [verification](results/random04-kk-turn-tuning-full-v1/verification.json) |

The turn6 control reproduces the earlier whole trajectory (1801); turn2 gains8.11%. Turn4/8/12 score1795/1741/1340. Full1000steps, four physical cores, enforced1000ms; mean187.04ms, max245.83ms, RSS0.117GB. The read-only regional peak audit also reproduces the complete1801control trajectory. This is a single-seed maximum, not a matched NMS result.

## GAME exceeds its published NMS throughput target

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 23977 | 2026-09-21T05:11:35.079941+00:00 | [f644acc](https://github.com/fywu85/lorr/commit/f644accc7e00ceda7a455b7a45ed6eee4f970f8d) | TRICK, uniform3250, seed0 | [verification](results/game-fleet-higher-full-v1/verification.json) |

Full5000steps, four physical cores, strict1000ms; max842.09ms. Uniform2750 control21742 is trajectory-identical to its earlier run. Limits3000/3250/3500 score23010/23977/22556. The selected score is3.02% above the historical NMS23274 target, not a matched local comparison. The independent fleet-mask and assignment audit passes. 3250 of6500robots receive new tasks; all remain movable. This deliberate fairness tradeoff is reported, not described as starvation-free.

## RANDOM-04 full-run improvement

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 1999 | 2026-09-21T05:26:11.060867+00:00 | [90df94f](https://github.com/fywu85/lorr/commit/90df94f0d1ca1e380acd1b4a9cf1a7cc851c4638) | TRICK, trick_temperature5000_peak1, seed0 | [verification](results/random04-regional-peak-full-v1/verification.json) |

Prior control whole trajectory matches. Strict1000ms, max238.42ms, RSS0.117GB. Peak retention at1000ppm lost1947->1589; at5000ppm gained1924->1999. This interaction is experimental, not a general win. All four cases complete.

## SORTATION full-run improvement

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 150894 | 2026-09-21T05:26:25.237738+00:00 | [51aab58](https://github.com/fywu85/lorr/commit/51aab5868457000c376560ba8f0166fc684df7b5) | TRICK, trick_age_pickup8_match64, seed0 | [verification](results/sortation-dispatch-full-v1/verification.json) |

Prior control whole trajectory matches. Strict1000ms, max993.90ms, RSS12.423GB. Matching64 improves150333->150894. Another dispatch arm timed out at step0 and has no accepted score; the failure is retained. The selected run has little timing margin and needs replication/runtime work.

## RANDOM-02 scheduling replication

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 1197 | 2026-09-21T05:29:31.471043+00:00 | [aad422f](https://github.com/fywu85/lorr/commit/aad422ff1160e1daae02eccf575209da1e4edd3a) | TRICK, cadence1/budget4, seed4 | [verification](results/random02-rematch-seeds-full-v1/verification.json) |

Same frozen configuration as1188seed0. Control versus candidate on seeds0/2/4:1160/1113/1135 versus1188/1189/1197. All positive; full600steps, strict1000ms, four physical cores. [Paired summary](rematch-budget/random02-three-seed-summary.json).

## RANDOM-05 full-run improvement

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 2898 | 2026-09-21T05:43:30.815484+00:00 | [90df94f](https://github.com/fywu85/lorr/commit/90df94f0d1ca1e380acd1b4a9cf1a7cc851c4638) | TRICK, trick_temperature1000_peak1, seed0 | [verification](results/random05-regional-peak-full-v1/verification.json) |

Exact prior control trajectory reproduced. Full horizon, four physical cores, strict1000ms. Max 672.45ms, RSS0.194GB. Peak retention improves2877 to2898 at the same prescribed work. This is one seed; the prior8round gain has separate three-seed support.

## RANDOM-01 full-run improvement

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 626 | 2026-09-21T05:42:58.012467+00:00 | [90df94f](https://github.com/fywu85/lorr/commit/90df94f0d1ca1e380acd1b4a9cf1a7cc851c4638) | TRICK, trick_geometric_mean, seed0 | [verification](results/random01-geometric-horizon-full-v1/verification.json) |

Exact prior control trajectory reproduced. Full horizon, four physical cores, strict1000ms. Max 200.44ms, RSS0.058GB. Geometric horizon bound / mean margin / p90 score625/626/623 versus621control. The known600-step end is explicitly a trick; started tasks are preserved.

## RANDOM-05 twelve repair rounds

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 2915 | 2026-09-21T05:44:08.636849+00:00 | [aad422f](https://github.com/fywu85/lorr/commit/aad422ff1160e1daae02eccf575209da1e4edd3a) | TRICK, round12, seed0 | [verification](results/random05-rounds-fine-full-v1/verification.json) |

Exact eight-round control repeats2877. Six/ten/twelve rounds score2741/2861/2915. The selected maximum passes strict1000ms, max895.64ms, four physical cores. Higher work is not monotonically better. This run does not use peak retention.

## RANDOM-01 seed replication exceeds the horizon candidate

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 635 | 2026-09-21T05:49:17.842678+00:00 | [90df94f](https://github.com/fywu85/lorr/commit/90df94f0d1ca1e380acd1b4a9cf1a7cc851c4638) | TRICK, no horizon, seed2 | [verification](results/random01-geometric-horizon-seeds-full-v1/verification.json) |

The original profile scores621/635/631 on seeds0/2/4; the horizon mean-margin candidate scores626/633/633. The best selected run therefore has no known-horizon trick. Other existing field/scheduler tricks remain explicitly enabled. This is seed variation, not a new algorithmic gain.

## GAME 24326 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 24326 | 2026-09-21T06:01:20.145817+00:00 | [90df94f](https://github.com/fywu85/lorr/commit/90df94f0d1ca1e380acd1b4a9cf1a7cc851c4638) | TRICK, trick_fleet3250_pickup12, seed0 | [verification](results/game-fleet-dispatch-full-v1/verification.json) |

Pickup weight12 improves23977 to24326 with the3250 active-fleet trick. Single seed; replication pending. Full horizon, four physical cores, strict1000ms; max827.91ms, RSS10.553GB. Published NMS is a historical target, not a matched run.

## RANDOM-05 2956 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 2956 | 2026-09-21T05:57:19.356336+00:00 | [90df94f](https://github.com/fywu85/lorr/commit/90df94f0d1ca1e380acd1b4a9cf1a7cc851c4638) | TRICK, trick_workers32_equal, seed0 | [verification](results/random05-priority-diversity-full-v1/verification.json) |

Thirty-two starts at125k candidate cap versus eight at500k: same4M nominal totalcap, but construction and completed-attempt overshoot differ. Regional rounds8 unchanged. One seed; no peak retention. Full horizon, four physical cores, strict1000ms; max766.89ms, RSS0.196GB. Published NMS is a historical target, not a matched run.

## RANDOM-01 638 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 638 | 2026-09-21T05:54:19.412273+00:00 | [90df94f](https://github.com/fywu85/lorr/commit/90df94f0d1ca1e380acd1b4a9cf1a7cc851c4638) | TRICK, trick_selected, seed8 | [verification](results/random01-seed-scan-full-v1/verification.json) |

Best of16 declared planner seeds6,8,...,36 using the unchanged no-horizon profile. Seed selection, not a new algorithmic gain. Full horizon, four physical cores, strict1000ms; max201.53ms, RSS0.058GB. Published NMS is a historical target, not a matched run.

## RANDOM-01 general chain guidance on the trick profile

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 644 | 2026-09-21T06:28:20.192302+00:00 | [7783736](https://github.com/fywu85/lorr/commit/7783736fec8dd929316fdaa2bd6d0b4fdd92c94c) | TRICK, chain mode3/order2, seed8 | [verification](results/random01-chain-potential-full-v1/verification.json) |

Original control638, score-only636, order2-only633, exact chain score+order644. Control reproduces the entire prior trajectory. The new mechanism is general and default-off; the selected field/scheduler remains an explicit trick. Above published NMS639 on this input/seed, not a matched competitor comparison. Max202.62ms, RSS0.102GB, strict1000ms, full600steps, four physical cores. Replication pending; the same mechanism lost on RANDOM02/03/04.

## Follow-up ablations, 2026-09-21

Known-horizon admission lost on GAME (23977control;23948bound,23867mean,23819p90)
and CITY02 (16315control;16287bound,16241mean,16212p90). Both original control
trajectories repeat exactly. Do not include these options in the selected profiles.

RANDOM05 peak retention scores2898/2781/2827 versus2877/2865/2836 on seeds0/2/4:
mean2835.33 versus2859.33 (-0.84%). The earlier single-seed gain does not support
a default change. Eight-core partitioning: parts4/round12 control2915 repeats its
original four-core trajectory; parts8/round12=2874, parts8/round16=2946, plus peak=2918.
The wider partition reduces maximum step time and supports more rounds, but its
best2946 remains below the separate2956four-core diversity record. Composition
is a new declared experiment, not an assumed additive improvement.

SORTATION reduced-work alternative150780 is114tasks below150894, with measured
maximum900.70ms versus989.60ms in the matched matrix. Preserve both records; timing
headroom in these runs is not a universal worst-case guarantee.

## RANDOM-01 chain-guidance replication

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 647 | 2026-09-21T06:35:07.086945+00:00 | [7783736](https://github.com/fywu85/lorr/commit/7783736fec8dd929316fdaa2bd6d0b4fdd92c94c) | TRICK, chain mode3/order2, seed2 | [verification](results/random01-chain-potential-seeds-full-v1/verification.json) |

Controls621/635/631 versus candidates645/647/595 on seeds0/2/4: both means629. All control trajectories reproduce; all six full runs pass strict1000ms. Seed4 loses36, so no average-gain claim. Together with644seed8, three observed seeds exceed published639; these remain historical comparisons. The647record has max202.09ms and four physical cores.

## GAME pickup-weight replication

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 24447 | 2026-09-21T06:41:11.628701+00:00 | [90df94f](https://github.com/fywu85/lorr/commit/90df94f0d1ca1e380acd1b4a9cf1a7cc851c4638) | TRICK,3250active/pickup12,seed4 | [verification](results/game-fleet-dispatch-seeds-full-v1/verification.json) |

Candidates24326/24360/24447 versus controls23977/23917/23888 on seeds0/2/4. All positive; full5000steps, strict1000ms, four physical cores. Fleet-admission fairness tradeoff remains explicit.

## RANDOM-05 full-chain scoring

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 3027 | 2026-09-21T06:42:55.693457+00:00 | [7783736](https://github.com/fywu85/lorr/commit/7783736fec8dd929316fdaa2bd6d0b4fdd92c94c) | TRICK, chain score only, seed0 | [verification](results/random05-chain-potential-full-v1/verification.json) |

Control2956 repeats its full trajectory. Score-only3027 (+2.40%), exact-priority-only2850, both2958. Full2000steps, four physical cores, strict1000ms. Max744.30ms, RSS0.239GB. Retain the original priority order; this is a single seed pending replication.

## RANDOM-05 known-horizon composition

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
|3043|2026-09-21T07:42:16.947356+00:00|[7783736](https://github.com/fywu85/lorr/commit/7783736fec8dd929316fdaa2bd6d0b4fdd92c94c)|trick_chain_horizon_mean, seed0, --trick RANDOM-05|[verification](results/random05-chain-horizon-full-v1/verification.json)|

The prior3027whole trajectory repeats. Bare known-horizon admission is also3027;
prospective mean margins3043; p90margins3037. This is an explicit2000step run-length
trick for new assignments. Held and started tasks remain protected. Peak entry
742.14ms, full2000steps, four physical cores, strict1s/32decimalGB. One seed;
replication pending. Outstanding-task agep90 remains2000: no starvation-free claim.

## RANDOM-01 protected-forecast window record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 662 | 2026-09-21T08:03:09.477723+00:00 | [339718b3](https://github.com/fywu85/lorr/commit/339718b3bc0364afb1f780033304b5f88d6d3ce9) | trick_current4096_prefix, seed 2, `--trick RANDOM-01` | [verification](results/random01-rolling-window-prefix-full-v5/verification.json) |

Complete CGAR first actions stay protected; compatible future forecasts can be repaired.
This increases the selected frontier from 647 to 662. Four physical cores, full 600 steps,
strict 1000 ms; maximum 431.85 ms. The matched long-window control
reproduces its previous entire trajectory. This is a selected-seed result;
replication against the previous frontier profile is pending.

## RANDOM-02 protected-forecast window record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 1202 | 2026-09-21T08:02:50.155207+00:00 | [339718b3](https://github.com/fywu85/lorr/commit/339718b3bc0364afb1f780033304b5f88d6d3ce9) | trick_current2048_prefix, seed 4, `--trick RANDOM-02` | [verification](results/random02-rolling-window-prefix-full-v5/verification.json) |

Complete CGAR first actions stay protected; compatible future forecasts can be repaired.
This increases the selected frontier from 1197 to 1202. Four physical cores, full 600 steps,
strict 1000 ms; maximum 361.25 ms. The matched long-window control
reproduces its previous entire trajectory. This is a selected-seed result;
replication against the previous frontier profile is pending.

## RANDOM-02 1215 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 1215 | 2026-09-21T08:13:20.292239+00:00 | [339718b3](https://github.com/fywu85/lorr/commit/339718b3bc0364afb1f780033304b5f88d6d3ce9) | trick_window_prefix, seed 2, `--trick RANDOM-02` | [verification](results/random02-rolling-window-prefix-seeds-full-v1/verification.json) |

Previous selected maximum 1202. Full 600 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 359.38 ms. Protected-forecast rolling-window replication; seed selection is explicit. Across seeds 0/2/4 the candidate scores 1180/1215/1202 against 1188/1189/1197. One seed loses; no uniform improvement claim.

## RANDOM-05 3065 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 3065 | 2026-09-21T08:08:28.600941+00:00 | [7783736f](https://github.com/fywu85/lorr/commit/7783736fec8dd929316fdaa2bd6d0b4fdd92c94c) | trick_selected, seed 10, `--trick RANDOM-05` | [verification](results/random05-chain-horizon-seedscan-full-v1/verification.json) |

Previous selected maximum 3043. Full 2000 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 709.47 ms. Best of the declared planner-seed scan 6/8/10/12: 2988/3019/3065/2913. Same frozen horizon-and-chain profile, no new algorithm change. Above published NMS 3050 on this archived input and selected seed, not a matched competitor run. Target remains 3355; starvation is not eliminated.

## RANDOM-03 1939 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 1939 | 2026-09-21T08:29:05.388485+00:00 | [339718b3](https://github.com/fywu85/lorr/commit/339718b3bc0364afb1f780033304b5f88d6d3ce9) | trick_keep0, seed 0, `--trick RANDOM-03` | [verification](results/random03-rolling-window-retention-full-v7/verification.json) |

Previous selected maximum 1902. Full 800 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 586.09 ms. CGAR-seeded fixed-work window with protected first actions and history disabled. The previous window control 1849 repeats its full trajectory. Keep0 and keep6 both score1939, above the earlier no-window frontier1902; keep19 gives1873. This selected-seed gain awaits replication, and is not a claim that retaining plans is universally harmful.

## RANDOM-01 668 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 668 | 2026-09-21T09:00:36.367232+00:00 | [cd145411](https://github.com/fywu85/lorr/commit/cd145411374c2009c98c82e5050ab7c2cc7676a1) | trick_uniform_turn20, seed 2, `--trick RANDOM-01` | [verification](results/random01-window-costs-full-v8/verification.json) |

Previous selected maximum 662. Full 600 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 467.96 ms. Uniform forward costs with unchanged4096attempts per island improve this selected seed. No average-gain claim; two additional paired seeds queued.

## RANDOM-04 2023 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 2023 | 2026-09-21T09:03:43.316338+00:00 | [965756fd](https://github.com/fywu85/lorr/commit/965756fdbf5111441d2f40bf97d3b604f8a85a0b) | trick_future_r4_h15_b2, seed 0, `--trick RANDOM-04` | [verification](results/random04-common-futures-full-v1/verification.json) |

Previous selected maximum 1999. Full 1000 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 268.97 ms. General complete common-future selection, four roots, two branches and15action forecasts; existing KK/rank/matching tricks retained. Shorter10action forecasts lose. Selected seed result; replication pending.

## RANDOM-01 671 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 671 | 2026-09-21T09:08:37.367615+00:00 | [cd145411](https://github.com/fywu85/lorr/commit/cd145411374c2009c98c82e5050ab7c2cc7676a1) | trick_uniform_turn20, seed 0, `--trick RANDOM-01` | [verification](results/random01-window-uniform-seeds-full-v1/verification.json) |

Previous selected maximum 668. Full 600 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 540.78 ms. Full fixed-work comparison under strict1s. RANDOM01uniform gain replicates across3seeds. RANDOM04H20is a new selected-seed depth gain; H15gain replicates across3seeds. Record maxima separately from average effects.

## RANDOM-01 682 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 682 | 2026-09-21T09:08:17.668518+00:00 | [cd145411](https://github.com/fywu85/lorr/commit/cd145411374c2009c98c82e5050ab7c2cc7676a1) | trick_uniform_turn20, seed 4, `--trick RANDOM-01` | [verification](results/random01-window-uniform-seeds-full-v1/verification.json) |

Previous selected maximum 671. Full 600 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 440.69 ms. Full fixed-work comparison under strict1s. RANDOM01uniform gain replicates across3seeds. RANDOM04H20is a new selected-seed depth gain; H15gain replicates across3seeds. Record maxima separately from average effects.

## RANDOM-04 2059 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 2059 | 2026-09-21T09:10:16.031488+00:00 | [965756fd](https://github.com/fywu85/lorr/commit/965756fdbf5111441d2f40bf97d3b604f8a85a0b) | trick_h20_b2, seed 0, `--trick RANDOM-04` | [verification](results/random04-common-futures-depth-full-v2/verification.json) |

Previous selected maximum 2023. Full 1000 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 283.86 ms. Full fixed-work comparison under strict1s. RANDOM01uniform gain replicates across3seeds. RANDOM04H20is a new selected-seed depth gain; H15gain replicates across3seeds. Record maxima separately from average effects.

## CITY-01 8440 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 8440 | 2026-09-21T09:17:37.954132+00:00 | [cd145411](https://github.com/fywu85/lorr/commit/cd145411374c2009c98c82e5050ab7c2cc7676a1) | trick_pickup_groups, seed 2, `--trick CITY-01` | [verification](results/city01-rematch-transfer-full-v2/verification.json) |

Previous selected maximum 8427. Full 3000 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 752.71 ms. Complete strict1s selected-seed improvement. CITY01pickup groups and RANDOM01refreshed keep10 are experimental compositions; replication pending. Deeper common futures lose on RANDOM04 and change no actions on RANDOM05.

## RANDOM-01 693 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 693 | 2026-09-21T09:21:08.553734+00:00 | [4154b482](https://github.com/fywu85/lorr/commit/4154b48222ab272f5209b4fa8160cf80182fe612) | trick_refresh_keep10, seed 4, `--trick RANDOM-01` | [verification](results/random01-history-rollout-full-v1/verification.json) |

Previous selected maximum 682. Full 600 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 488.91 ms. Complete strict1s selected-seed improvement. CITY01pickup groups and RANDOM01refreshed keep10 are experimental compositions; replication pending. Deeper common futures lose on RANDOM04 and change no actions on RANDOM05.

## RANDOM-03 1967 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 1967 | 2026-09-21T10:06:08.090418+00:00 | [c302547d](https://github.com/fywu85/lorr/commit/c302547df0651a2d662b269aa6d9a668addc14f2) | trick_delay3, seed 0, `--trick RANDOM-03` | [verification](results/random03-window-delay-full-v1/verification.json) |

Previous selected maximum 1939. Full 800 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 611.68 ms. Delay-directed repair reaches1967onRANDOM03seed0, strict1s4cores. Replication pending; off by default. AllR01completearms and R02delay1/7 lose; R02delay3timeoutexcluded.

## RANDOM-03 1982 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 1982 | 2026-09-21T10:15:32.778021+00:00 | [c302547d](https://github.com/fywu85/lorr/commit/c302547df0651a2d662b269aa6d9a668addc14f2) | trick_delay3, seed 2, `--trick RANDOM-03` | [verification](results/random03-window-delay-seeds-full-v1/verification.json) |

Previous selected maximum 1967. Full 800 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 662.92 ms. Delay3windowrepair replicates on seeds0/2/4:1967/1982/1948vs1939/1900/1870,+3.293%aggregate,allpositive. Newselectedmaximum1982seed2. Local-log-spool controls repeat full earlier trajectories.

## RANDOM-05 3130 full-run record

| Tasks | Completed UTC | Source | Profile | Evidence |
|---:|---|---|---|---|
| 3130 | 2026-09-21T11:49:47.972951+00:00 | [0962671f](https://github.com/fywu85/lorr/commit/0962671fee93801d104a0f35d723274ecc9994c7) | trick_task_cap700, seed 10, `--trick RANDOM-05` | [verification](results/random05-task-cap-full-v1/verification.json) |

Previous selected maximum 3065. Full 2000 steps; 4 physical cores, strict 1,000 ms and 32 decimal GB. Maximum 695.18 ms. RANDOM-05 seed10 cap700 reaches3130 vs exact3065 control; explicit trick with held tasks preserved, full strict1s max695.18ms. Selected-seed gain; seeds0/2 replication declared.
