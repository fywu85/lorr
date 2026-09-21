# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_window_control / seed2 | 662 | 325.17 | 429.35 | 0.136 | 372 |
| RANDOM-01 / trick_uniform_turn20 / seed2 | 668 | 338.02 | 467.96 | 0.135 | 401 |
| RANDOM-01 / trick_uniform_turn40 / seed2 | 640 | 424.55 | 669.54 | 0.134 | 371 |
| RANDOM-01 / trick_current_turn40 / seed2 | 622 | 386.04 | 550.08 | 0.136 | 387 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
