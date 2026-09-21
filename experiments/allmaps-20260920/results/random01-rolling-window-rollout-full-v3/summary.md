# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_control / seed2 | 635 | 150.66 | 200.97 | 0.059 | 355 |
| RANDOM-01 / trick_window512_wait / seed2 | 623 | 211.18 | 248.42 | 0.129 | 389 |
| RANDOM-01 / trick_window512_roll / seed2 | 621 | 212.47 | 253.13 | 0.132 | 369 |
| RANDOM-01 / trick_window2048_roll / seed2 | 621 | 312.2 | 426.2 | 0.132 | 390 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
