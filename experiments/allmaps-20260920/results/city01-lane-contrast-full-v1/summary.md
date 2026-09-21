# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_lane16_control / seed2 | 8427 | 254.92 | 756.7 | 8.096 | 2345 |
| CITY-01 / trick_lane12 / seed2 | 8414 | 251.15 | 734.86 | 8.106 | 2369 |
| CITY-01 / trick_lane8 / seed2 | 8386 | 257.45 | 759.58 | 8.102 | 2340 |
| CITY-01 / trick_lane4 / seed2 | 7949 | 268.78 | 666.22 | 8.098 | 2421 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
