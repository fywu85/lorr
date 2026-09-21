# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_current_control / seed0 | 8423 | 260.71 | 770.37 | 8.118 | 2352 |
| CITY-01 / trick_pickup_groups / seed0 | 8430 | 252.89 | 767.12 | 8.095 | 2320 |
| CITY-01 / trick_current_control / seed4 | 8425 | 252.92 | 762.92 | 8.107 | 2330 |
| CITY-01 / trick_pickup_groups / seed4 | 8434 | 253.3 | 758.4 | 8.102 | 2290 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
