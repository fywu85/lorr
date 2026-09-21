# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_control / seed4 | 693 | 322.19 | 465.59 | 0.132 | 383 |
| RANDOM-01 / trick_horizon_mean / seed4 | 691 | 333.67 | 488.05 | 0.132 | 383 |
| RANDOM-01 / trick_rematch4_every_step / seed4 | 685 | 358.19 | 473.62 | 0.133 | 377 |
| RANDOM-01 / trick_horizon_rematch4 / seed4 | 686 | 329.75 | 427.05 | 0.132 | 377 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
