# Generic regional work budgets across eight instances

Full archived inputs, one planner seed, identical generic settings except regional candidate budget. Shared-host5s development,32decimalGB. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

| Instance / regional limit | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / generic_regions_4m | 588 | 143.76 | 196.57 | 0.057 | 186 |
| RANDOM-02 / generic_regions_4m | 988 | 133.91 | 194.08 | 0.064 | 260 |
| RANDOM-03 / generic_regions_4m | 1381 | 134.11 | 201.78 | 0.083 | 554 |
| RANDOM-04 / generic_regions_4m | 842 | 125.05 | 161.98 | 0.114 | 1000 |
| SORTATION / generic_regions_4m | 146460 | 480.6 | 1031.18 | 12.382 | 671 |
| CITY-01 / generic_regions_4m | 7305 | 269.79 | 683.29 | 5.813 | 1187 |
| CITY-02 / generic_regions_4m | 14068 | 281.28 | 725.92 | 7.786 | 1330 |
| GAME / generic_regions_4m | 6519 | 297.53 | 817.5 | 8.992 | 5000 |
| RANDOM-01 / generic_regions_8m | 588 | 143.86 | 195.51 | 0.057 | 186 |
| RANDOM-02 / generic_regions_8m | 985 | 136.81 | 225.11 | 0.065 | 261 |
| RANDOM-03 / generic_regions_8m | 1281 | 195.78 | 248.5 | 0.083 | 635 |
| RANDOM-04 / generic_regions_8m | 940 | 190.27 | 254.01 | 0.114 | 1000 |
| SORTATION / generic_regions_8m | 146189 | 474.9 | 1045.25 | 12.372 | 670 |
| CITY-01 / generic_regions_8m | 7305 | 268.21 | 670.4 | 5.812 | 1187 |
| CITY-02 / generic_regions_8m | 13982 | 317.84 | 732.03 | 7.783 | 1354 |
| GAME / generic_regions_8m | 6347 | 374.49 | 812.16 | 9.152 | 5000 |

Throughput is primary; fairness is a secondary reported metric. RANDOM-05 was excluded.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
