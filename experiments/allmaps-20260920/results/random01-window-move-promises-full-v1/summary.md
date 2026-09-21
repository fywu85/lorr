# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_control / seed4 | 693 | 335.12 | 489.79 | 0.132 | 383 |
| RANDOM-01 / trick_window_move_promise1 / seed4 | 687 | 335.91 | 460.78 | 0.132 | 375 |
| RANDOM-01 / trick_window_move_promise2 / seed4 | 688 | 334.92 | 520.47 | 0.132 | 397 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
