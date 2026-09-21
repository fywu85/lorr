# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_seed_control / seed2 | 635 | 146.34 | 191.81 | 0.059 | 355 |
| RANDOM-01 / trick_window_current / seed2 | 629 | 180.15 | 213.41 | 0.131 | 382 |
| RANDOM-01 / trick_uniform_seed / seed2 | flag_mismatch | n/a | n/a | 0.009 | n/a |
| RANDOM-01 / trick_window_uniform / seed2 | flag_mismatch | n/a | n/a | 0.009 | n/a |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
