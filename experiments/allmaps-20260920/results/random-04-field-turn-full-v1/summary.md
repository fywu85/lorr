# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_round4_control / seed0 | 1743 | 209.6 | 316.7 | 0.118 | 1000 |
| RANDOM-04 / trick_standalone_turn6 / seed0 | 1568 | 179.58 | 254.13 | 0.117 | 1000 |
| RANDOM-04 / trick_standalone_turn12 / seed0 | 1449 | 178.77 | 251.5 | 0.116 | 1000 |
| RANDOM-04 / trick_kk_turn6 / seed0 | 1801 | 181.99 | 250.01 | 0.117 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
