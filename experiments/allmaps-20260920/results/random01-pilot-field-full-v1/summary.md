# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_window_control / seed2 | 662 | 325.35 | 423.09 | 0.134 | 372 |
| RANDOM-01 / trick_pilot_field_window / seed2 | 563 | 353.74 | 500.96 | 0.134 | 441 |
| RANDOM-01 / trick_pilot_field_turn40 / seed2 | 567 | 338.06 | 461.36 | 0.137 | 442 |
| RANDOM-01 / trick_pilot_field_no_window / seed2 | 627 | 150.98 | 201.16 | 0.058 | 390 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
