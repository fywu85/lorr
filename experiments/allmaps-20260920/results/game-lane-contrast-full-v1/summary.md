# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_lane16_control / seed4 | 24447 | 247.28 | 835.31 | 10.554 | 4961 |
| GAME / trick_lane12 / seed4 | 24290 | 245.18 | 829.12 | 11.932 | 4963 |
| GAME / trick_lane8 / seed4 | 16587 | 243.78 | 843.98 | 10.545 | 5000 |
| GAME / trick_lane4 / seed4 | 9756 | 233.46 | 765.49 | 10.542 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
