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
