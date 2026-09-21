# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_seed_control / seed0 | 1902 | 188.36 | 301.59 | 0.084 | 800 |
| RANDOM-03 / trick_window_current / seed0 | 1256 | 206.46 | 331.87 | 0.158 | 800 |
| RANDOM-03 / trick_uniform_seed / seed0 | flag_mismatch | n/a | n/a | 0.009 | n/a |
| RANDOM-03 / trick_window_uniform / seed0 | flag_mismatch | n/a | n/a | 0.009 | n/a |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
