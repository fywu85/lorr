# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_control / seed4 | 693 | 325.35 | 473.11 | 0.132 | 383 |
| RANDOM-01 / trick_iterations8192 / seed4 | 676 | 510.41 | 697.39 | 0.134 | 396 |
| RANDOM-01 / trick_iterations16384 / seed4 | timeout | n/a | n/a | 0.124 | n/a |
| RANDOM-01 / trick_group8 / seed4 | 691 | 446.9 | 599.2 | 0.134 | 411 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
