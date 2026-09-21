# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4, 6].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_pickup8 / seed2 | 8367 | 251.34 | 754.31 | 8.101 | 2508 |
| CITY-01 / trick_pickup8 / seed4 | 8374 | 251.62 | 759.86 | 8.102 | 2452 |
| CITY-01 / trick_pickup8 / seed6 | 8386 | 250.85 | 754.11 | 8.114 | 2445 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
