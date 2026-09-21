# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [10].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_current_control / seed10 | 3065 | 579.45 | 694.83 | 0.241 | 2000 |
| RANDOM-05 / trick_task_cap600 / seed10 | 3032 | 590.53 | 706.71 | 0.239 | 2000 |
| RANDOM-05 / trick_task_cap700 / seed10 | 3130 | 585.66 | 695.18 | 0.240 | 2000 |
| RANDOM-05 / trick_task_cap750 / seed10 | 2985 | 585.51 | 705.26 | 0.239 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
