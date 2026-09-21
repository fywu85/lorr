# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_control / seed4 | 693 | 336.83 | 487.2 | 0.132 | 383 |
| RANDOM-01 / trick_turn40 / seed4 | 647 | 381.7 | 544.79 | 0.134 | 399 |
| RANDOM-01 / trick_wait20 / seed4 | 690 | 332.0 | 428.13 | 0.133 | 403 |
| RANDOM-01 / trick_turn40_wait20 / seed4 | 677 | 350.46 | 501.47 | 0.132 | 354 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
