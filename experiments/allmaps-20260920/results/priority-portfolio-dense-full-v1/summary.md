# Controlled CGAR factor matrix

Archived inputs, one planner seed, frozen predeclared factor profiles. Throughput comparisons require full horizons. Shared-host5s development,32decimalGB. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

| Instance / variant | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / generic_chain_rank_1worker | 1249 | 127.73 | 169.93 | 0.116 | 1000 |
| RANDOM-05 / generic_chain_rank_1worker | 1847 | 124.0 | 178.26 | 0.192 | 2000 |
| RANDOM-04 / generic_chain_rank_8workers | 1330 | 101.89 | 141.56 | 0.115 | 1000 |
| RANDOM-05 / generic_chain_rank_8workers | 1989 | 99.77 | 276.31 | 0.192 | 2000 |
| RANDOM-04 / generic_noise50_cold | 1306 | 102.08 | 147.25 | 0.116 | 1000 |
| RANDOM-05 / generic_noise50_cold | 2036 | 100.24 | 291.41 | 0.192 | 2000 |
| RANDOM-04 / generic_noise50_persistent | 1193 | 102.45 | 138.23 | 0.116 | 1000 |
| RANDOM-05 / generic_noise50_persistent | 2003 | 100.12 | 292.12 | 0.192 | 2000 |
| RANDOM-04 / generic_noise200_persistent | 1187 | 101.71 | 148.89 | 0.115 | 1000 |
| RANDOM-05 / generic_noise200_persistent | 1954 | 100.58 | 244.09 | 0.192 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
