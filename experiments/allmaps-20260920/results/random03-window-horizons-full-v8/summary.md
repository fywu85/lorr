# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_horizon20_control / seed0 | 1939 | 505.28 | 621.93 | 0.159 | 757 |
| RANDOM-03 / trick_horizon10 / seed0 | 1917 | 281.49 | 390.47 | 0.145 | 758 |
| RANDOM-03 / trick_horizon15 / seed0 | 1757 | 322.7 | 478.22 | 0.155 | 800 |
| RANDOM-03 / trick_horizon25 / seed0 | 1893 | 676.6 | 835.92 | 0.168 | 768 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
