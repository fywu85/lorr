# Controlled CGAR factor matrix

Archived inputs, one planner seed, frozen predeclared factor profiles. Throughput comparisons require full horizons. Shared-host5s development,32decimalGB. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

| Instance / variant | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / generic_chain_rank_control | 1249 | 126.57 | 169.38 | 0.115 | 1000 |
| RANDOM-05 / generic_chain_rank_control | 1847 | 124.05 | 179.71 | 0.192 | 2000 |
| RANDOM-04 / generic_remaining_potential | 1243 | 126.75 | 169.42 | 0.115 | 1000 |
| RANDOM-05 / generic_remaining_potential | 1749 | 123.31 | 176.45 | 0.191 | 2000 |
| RANDOM-04 / generic_next_errand | 1207 | 125.41 | 177.7 | 0.115 | 1000 |
| RANDOM-05 / generic_next_errand | 1760 | 123.82 | 166.57 | 0.191 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
