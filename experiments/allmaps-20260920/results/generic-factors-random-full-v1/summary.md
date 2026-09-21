# Controlled CGAR factor matrix

Archived inputs, one planner seed, frozen predeclared factor profiles. Throughput comparisons require full horizons. Shared-host5s development,32decimalGB. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

| Instance / variant | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / generic_control | 588 | 143.4 | 192.44 | 0.058 | 186 |
| RANDOM-02 / generic_control | 988 | 133.77 | 188.73 | 0.065 | 260 |
| RANDOM-03 / generic_control | 1381 | 133.82 | 199.62 | 0.083 | 554 |
| RANDOM-04 / generic_control | 842 | 125.26 | 157.54 | 0.114 | 1000 |
| RANDOM-01 / generic_chain_order | 587 | 138.5 | 197.0 | 0.057 | 190 |
| RANDOM-02 / generic_chain_order | 992 | 133.19 | 184.3 | 0.065 | 234 |
| RANDOM-03 / generic_chain_order | 1395 | 137.35 | 182.36 | 0.083 | 529 |
| RANDOM-04 / generic_chain_order | 909 | 124.91 | 170.18 | 0.114 | 1000 |
| RANDOM-01 / generic_chain_rank | 582 | 146.73 | 197.63 | 0.057 | 190 |
| RANDOM-02 / generic_chain_rank | 1020 | 133.17 | 190.93 | 0.064 | 230 |
| RANDOM-03 / generic_chain_rank | 1435 | 137.43 | 195.6 | 0.084 | 729 |
| RANDOM-04 / generic_chain_rank | 1249 | 125.73 | 169.74 | 0.114 | 1000 |
| RANDOM-01 / generic_match64 | 601 | 145.42 | 224.84 | 0.058 | 182 |
| RANDOM-02 / generic_match64 | 1021 | 132.17 | 189.34 | 0.065 | 233 |
| RANDOM-03 / generic_match64 | 1368 | 133.4 | 200.26 | 0.083 | 568 |
| RANDOM-04 / generic_match64 | 938 | 125.02 | 162.86 | 0.115 | 1000 |
| RANDOM-01 / generic_direct_cost | 587 | 140.79 | 197.96 | 0.057 | 545 |
| RANDOM-02 / generic_direct_cost | 1052 | 131.81 | 189.47 | 0.065 | 568 |
| RANDOM-03 / generic_direct_cost | 1433 | 135.1 | 196.28 | 0.083 | 800 |
| RANDOM-04 / generic_direct_cost | 936 | 124.3 | 161.93 | 0.114 | 1000 |
| RANDOM-01 / generic_match64_direct | 611 | 140.74 | 203.35 | 0.058 | 537 |
| RANDOM-02 / generic_match64_direct | 1084 | 132.09 | 187.54 | 0.065 | 552 |
| RANDOM-03 / generic_match64_direct | 1484 | 135.79 | 203.47 | 0.083 | 800 |
| RANDOM-04 / generic_match64_direct | 982 | 125.07 | 167.72 | 0.114 | 1000 |

Throughput is primary; fairness is a secondary reported metric. RANDOM-05 was excluded.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
