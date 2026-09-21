# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_selected_control / seed2 | 15822 | 269.63 | 909.59 | 10.535 | 5000 |
| GAME / trick_uniform2750 / seed2 | 21720 | 240.02 | 827.92 | 10.571 | 5000 |
| GAME / trick_selected_control / seed4 | 15947 | 263.28 | 893.44 | 10.507 | 5000 |
| GAME / trick_uniform2750 / seed4 | 21752 | 240.43 | 823.14 | 10.565 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
