# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_adapted_control / seed0 | 7755 | 249.17 | 756.09 | 8.101 | 1134 |
| CITY-01 / trick_adapted_rank / seed0 | 7739 | 250.59 | 754.52 | 8.092 | 1134 |
| CITY-01 / trick_adapted_dispatch / seed0 | flag_mismatch | n/a | n/a | 0.024 | n/a |
| CITY-01 / trick_adapted_rank_dispatch / seed0 | flag_mismatch | n/a | n/a | 0.024 | n/a |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
