# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current2048_control / seed0 | 1829 | 358.48 | 483.11 | 0.162 | 800 |
| RANDOM-03 / trick_current2048_prefix / seed0 | 1678 | 341.05 | 498.48 | 0.162 | 800 |
| RANDOM-03 / trick_current4096_prefix / seed0 | 1849 | 430.13 | 591.2 | 0.159 | 784 |
| RANDOM-03 / trick_uniform2048_prefix / seed0 | 1737 | 361.98 | 474.24 | 0.160 | 793 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
