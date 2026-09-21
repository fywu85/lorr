# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_roll2048_control / seed2 | 621 | 310.94 | 434.87 | 0.132 | 390 |
| RANDOM-01 / trick_roll2048_ties / seed2 | 635 | 283.51 | 374.82 | 0.133 | 389 |
| RANDOM-01 / trick_current2048_ties / seed2 | 642 | 254.45 | 324.31 | 0.134 | 401 |
| RANDOM-01 / trick_current512_ties / seed2 | 642 | 199.29 | 237.88 | 0.130 | 395 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
