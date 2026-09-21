# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [10].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_cap700_control / seed10 | 3130 | 586.69 | 692.49 | 0.239 | 2000 |
| RANDOM-05 / trick_task_cap625 / seed10 | 3042 | 587.14 | 708.7 | 0.239 | 2000 |
| RANDOM-05 / trick_task_cap650 / seed10 | 3084 | 586.99 | 719.04 | 0.239 | 2000 |
| RANDOM-05 / trick_task_cap675 / seed10 | 3106 | 586.95 | 722.0 | 0.239 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
