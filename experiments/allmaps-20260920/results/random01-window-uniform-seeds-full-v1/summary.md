# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_window_control / seed0 | 658 | 309.99 | 434.81 | 0.134 | 390 |
| RANDOM-01 / trick_uniform_turn20 / seed0 | 671 | 375.85 | 540.78 | 0.133 | 408 |
| RANDOM-01 / trick_current_window_control / seed4 | 648 | 337.41 | 489.11 | 0.136 | 395 |
| RANDOM-01 / trick_uniform_turn20 / seed4 | 682 | 342.98 | 440.69 | 0.133 | 400 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
