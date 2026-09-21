# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_linear_control / seed2 | 1480 | 101.58 | 137.52 | 0.116 | 1000 |
| RANDOM-04 / trick_squared_rank / seed2 | 1645 | 102.64 | 139.87 | 0.116 | 1000 |
| RANDOM-04 / trick_linear_control / seed4 | 1494 | 101.45 | 139.71 | 0.116 | 1000 |
| RANDOM-04 / trick_squared_rank / seed4 | 1512 | 101.76 | 140.93 | 0.116 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
