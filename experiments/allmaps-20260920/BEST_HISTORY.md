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
