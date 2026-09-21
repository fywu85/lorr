# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_selected_control / seed2 | 635 | 148.08 | 194.2 | 0.058 | 355 |
| RANDOM-01 / trick_geometric_mean / seed2 | 633 | 149.6 | 202.95 | 0.058 | 355 |
| RANDOM-01 / trick_selected_control / seed4 | 631 | 148.62 | 195.39 | 0.058 | 378 |
| RANDOM-01 / trick_geometric_mean / seed4 | 633 | 147.74 | 193.83 | 0.058 | 378 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
