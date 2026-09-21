# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_control / seed0 | 6519 | 300.22 | 822.72 | 8.993 | 5000 |
| GAME / trick_adapted_grid / seed0 | 10080 | 259.84 | 904.65 | 10.513 | 5000 |
| GAME / trick_native_grid / seed0 | 6784 | 268.62 | 921.51 | 10.498 | 5000 |
| GAME / trick_native_short / seed0 | 6758 | 267.0 | 930.4 | 10.513 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
