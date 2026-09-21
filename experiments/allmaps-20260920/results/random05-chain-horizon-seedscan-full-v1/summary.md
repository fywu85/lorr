# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [6, 8, 10, 12].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_selected / seed6 | 2988 | 590.12 | 730.24 | 0.239 | 2000 |
| RANDOM-05 / trick_selected / seed8 | 3019 | 583.78 | 763.76 | 0.239 | 2000 |
| RANDOM-05 / trick_selected / seed10 | 3065 | 583.95 | 709.47 | 0.239 | 2000 |
| RANDOM-05 / trick_selected / seed12 | 2913 | 594.88 | 754.51 | 0.239 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
