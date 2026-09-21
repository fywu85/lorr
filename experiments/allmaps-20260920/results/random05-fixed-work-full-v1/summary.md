# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_selected_control / seed0 | 2574 | 97.67 | 163.9 | 0.193 | 2000 |
| RANDOM-05 / trick_global2m_each / seed0 | 2544 | 131.75 | 164.96 | 0.193 | 2000 |
| RANDOM-05 / trick_global4m_each / seed0 | 2559 | 166.53 | 198.36 | 0.193 | 2000 |
| RANDOM-05 / trick_region8m / seed0 | 2684 | 158.8 | 227.23 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
