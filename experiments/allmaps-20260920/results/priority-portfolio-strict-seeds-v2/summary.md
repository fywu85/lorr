# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0, 2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / generic_chain_rank_1worker / seed0 | 1249 | 126.19 | 174.97 | 0.114 | 1000 |
| RANDOM-05 / generic_chain_rank_1worker / seed0 | 1847 | 123.55 | 180.59 | 0.191 | 2000 |
| RANDOM-04 / generic_chain_rank_8workers / seed0 | 1330 | 101.67 | 146.15 | 0.116 | 1000 |
| RANDOM-05 / generic_chain_rank_8workers / seed0 | 1989 | 99.65 | 278.09 | 0.192 | 2000 |
| RANDOM-04 / generic_noise50_cold / seed0 | 1306 | 101.99 | 149.96 | 0.116 | 1000 |
| RANDOM-05 / generic_noise50_cold / seed0 | 2036 | 100.01 | 291.67 | 0.193 | 2000 |
| RANDOM-04 / generic_chain_rank_1worker / seed2 | 1198 | 126.7 | 160.87 | 0.115 | 1000 |
| RANDOM-05 / generic_chain_rank_1worker / seed2 | 1836 | 123.99 | 175.69 | 0.191 | 2000 |
| RANDOM-04 / generic_chain_rank_8workers / seed2 | 1296 | 101.35 | 148.26 | 0.115 | 1000 |
| RANDOM-05 / generic_chain_rank_8workers / seed2 | 1922 | 99.96 | 264.82 | 0.192 | 2000 |
| RANDOM-04 / generic_noise50_cold / seed2 | 1231 | 101.71 | 146.51 | 0.115 | 1000 |
| RANDOM-05 / generic_noise50_cold / seed2 | 2031 | 102.95 | 265.03 | 0.192 | 2000 |
| RANDOM-04 / generic_chain_rank_1worker / seed4 | 1260 | 125.56 | 179.96 | 0.115 | 1000 |
| RANDOM-05 / generic_chain_rank_1worker / seed4 | 1806 | 124.09 | 174.34 | 0.191 | 2000 |
| RANDOM-04 / generic_chain_rank_8workers / seed4 | 1295 | 101.68 | 134.09 | 0.115 | 1000 |
| RANDOM-05 / generic_chain_rank_8workers / seed4 | 1970 | 99.85 | 290.57 | 0.192 | 2000 |
| RANDOM-04 / generic_noise50_cold / seed4 | 1367 | 102.1 | 138.31 | 0.115 | 1000 |
| RANDOM-05 / generic_noise50_cold / seed4 | 2031 | 100.24 | 307.61 | 0.192 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
