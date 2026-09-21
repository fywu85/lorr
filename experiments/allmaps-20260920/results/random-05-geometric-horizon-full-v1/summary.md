# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_selected_control / seed0 | timeout | n/a | n/a | 0.049 | n/a |
| RANDOM-05 / trick_geometric_bound / seed0 | 2806 | 288.21 | 343.42 | 0.193 | 2000 |
| RANDOM-05 / trick_geometric_mean / seed0 | 2806 | 287.79 | 335.11 | 0.194 | 2000 |
| RANDOM-05 / trick_geometric_p90 / seed0 | 2819 | 287.44 | 343.25 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
