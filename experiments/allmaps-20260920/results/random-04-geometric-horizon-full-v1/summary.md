# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_selected_control / seed0 | 1743 | 288.33 | 373.36 | 0.118 | 1000 |
| RANDOM-04 / trick_geometric_bound / seed0 | 1740 | 182.91 | 229.82 | 0.116 | 1000 |
| RANDOM-04 / trick_geometric_mean / seed0 | 1755 | 182.59 | 240.52 | 0.116 | 1000 |
| RANDOM-04 / trick_geometric_p90 / seed0 | 1693 | 182.94 | 240.72 | 0.117 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
