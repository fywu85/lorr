# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_fleet3250_control / seed2 | 23917 | 243.81 | 833.13 | 10.536 | 4983 |
| GAME / trick_fleet3250_pickup12 / seed2 | 24360 | 249.57 | 843.58 | 10.571 | 4958 |
| GAME / trick_fleet3250_control / seed4 | 23888 | 244.77 | 844.15 | 10.546 | 4973 |
| GAME / trick_fleet3250_pickup12 / seed4 | 24447 | 247.03 | 826.52 | 10.541 | 4961 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
