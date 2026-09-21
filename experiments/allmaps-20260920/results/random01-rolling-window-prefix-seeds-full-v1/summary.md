# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_previous_control / seed0 | 645 | 151.9 | 223.84 | 0.101 | 402 |
| RANDOM-01 / trick_window_prefix / seed0 | 658 | 324.15 | 455.89 | 0.133 | 390 |
| RANDOM-01 / trick_previous_control / seed4 | 595 | 144.39 | 203.23 | 0.101 | 399 |
| RANDOM-01 / trick_window_prefix / seed4 | 648 | 337.48 | 495.89 | 0.133 | 395 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
