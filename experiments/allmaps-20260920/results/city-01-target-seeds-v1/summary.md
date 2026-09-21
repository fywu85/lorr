# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4, 6].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_pickup12 / seed2 | 8424 | 251.93 | 763.46 | 8.098 | 2367 |
| CITY-01 / trick_pickup16 / seed2 | 8427 | 253.47 | 753.9 | 8.096 | 2345 |
| CITY-01 / trick_pickup12 / seed4 | 8414 | 251.0 | 760.45 | 8.107 | 2388 |
| CITY-01 / trick_pickup16 / seed4 | 8425 | 254.01 | 748.92 | 8.106 | 2330 |
| CITY-01 / trick_pickup12 / seed6 | 8422 | 253.72 | 758.52 | 8.095 | 2388 |
| CITY-01 / trick_pickup16 / seed6 | 8405 | 251.82 | 749.66 | 8.093 | 2341 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
