# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_control / seed4 | 693 | 350.06 | 520.07 | 0.132 | 383 |
| RANDOM-01 / trick_temperature20 / seed4 | 680 | 350.06 | 463.24 | 0.133 | 396 |
| RANDOM-01 / trick_temperature80 / seed4 | 679 | 352.06 | 484.32 | 0.132 | 405 |
| RANDOM-01 / trick_temperature320 / seed4 | 677 | 387.43 | 518.48 | 0.131 | 376 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
