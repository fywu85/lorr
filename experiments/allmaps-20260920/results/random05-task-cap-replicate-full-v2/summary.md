# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0, 2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_current_control / seed0 | 3043 | 580.34 | 720.12 | 0.240 | 2000 |
| RANDOM-05 / trick_task_cap700 / seed0 | 3033 | 588.3 | 704.84 | 0.240 | 2000 |
| RANDOM-05 / trick_current_control / seed2 | 3015 | 581.47 | 692.04 | 0.240 | 2000 |
| RANDOM-05 / trick_task_cap700 / seed2 | 3063 | 586.48 | 724.13 | 0.239 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
