# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_current_control / seed2 | 8427 | 257.23 | 760.41 | 8.095 | 2345 |
| CITY-01 / trick_pickup_groups / seed2 | 8440 | 253.15 | 752.71 | 8.094 | 2342 |
| CITY-01 / trick_budget2 / seed2 | 8400 | 248.63 | 751.5 | 8.102 | 2355 |
| CITY-01 / trick_pickup_groups_budget2 / seed2 | 8431 | 254.23 | 757.96 | 8.093 | 2348 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
