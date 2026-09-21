# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_control / seed4 | 693 | 334.3 | 480.61 | 0.132 | 383 |
| RANDOM-01 / trick_delay1 / seed4 | 688 | 341.91 | 440.1 | 0.134 | 379 |
| RANDOM-01 / trick_delay3 / seed4 | 679 | 389.59 | 576.19 | 0.131 | 404 |
| RANDOM-01 / trick_delay7 / seed4 | 673 | 464.91 | 727.07 | 0.133 | 379 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
