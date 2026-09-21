# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [10].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_current_control / seed10 | timeout | n/a | n/a | 0.094 | n/a |
| RANDOM-05 / trick_future_r4_h10_b2 / seed10 | 3065 | 608.73 | 733.41 | 0.249 | 2000 |
| RANDOM-05 / trick_future_r8_h10_b2 / seed10 | 3065 | 621.48 | 746.03 | 0.256 | 2000 |
| RANDOM-05 / trick_future_r4_h15_b2 / seed10 | 3065 | 618.23 | 746.68 | 0.249 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
