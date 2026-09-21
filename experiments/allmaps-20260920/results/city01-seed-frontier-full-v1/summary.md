# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [6, 8, 10, 12].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_current_profile / seed6 | 8453 | 252.74 | 764.78 | 8.097 | 2333 |
| CITY-01 / trick_current_profile / seed8 | 8433 | 251.11 | 761.41 | 8.091 | 2325 |
| CITY-01 / trick_current_profile / seed10 | 8437 | 253.58 | 764.4 | 8.113 | 2346 |
| CITY-01 / trick_current_profile / seed12 | 8427 | 252.82 | 768.3 | 8.110 | 2312 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
