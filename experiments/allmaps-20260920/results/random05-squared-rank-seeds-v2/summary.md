# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_linear_control / seed2 | 2608 | 96.24 | 161.48 | 0.193 | 2000 |
| RANDOM-05 / trick_squared_rank / seed2 | 2571 | 96.68 | 189.25 | 0.193 | 2000 |
| RANDOM-05 / trick_linear_control / seed4 | 2528 | 96.8 | 158.14 | 0.193 | 2000 |
| RANDOM-05 / trick_squared_rank / seed4 | 2583 | 96.63 | 162.47 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
