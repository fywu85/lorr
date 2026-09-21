# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_adapted_equal_control / seed0 | 10080 | 258.81 | 905.43 | 10.512 | 5000 |
| GAME / trick_adapted_linear / seed0 | 13314 | 261.93 | 903.79 | 10.558 | 5000 |
| GAME / trick_adapted_squared / seed0 | 14471 | 262.45 | 896.2 | 10.486 | 5000 |
| GAME / trick_adapted_squared_chain / seed0 | 14664 | 263.74 | 896.57 | 10.614 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
