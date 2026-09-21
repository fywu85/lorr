# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_control / seed0 | 7305 | 269.41 | 673.4 | 5.812 | 1187 |
| CITY-01 / trick_adapted_grid / seed0 | 7755 | 250.42 | 764.89 | 8.100 | 1134 |
| CITY-01 / trick_native_grid / seed0 | 6815 | 246.06 | 799.67 | 8.102 | 1264 |
| CITY-01 / trick_native_short / seed0 | 6669 | 245.8 | 813.17 | 8.114 | 2720 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
