# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_current_control / seed0 | 2059 | 226.57 | 281.33 | 0.174 | 1000 |
| RANDOM-04 / trick_task_cap600 / seed0 | 1941 | 232.88 | 284.28 | 0.175 | 1000 |
| RANDOM-04 / trick_task_cap650 / seed0 | 1758 | 228.47 | 286.05 | 0.174 | 1000 |
| RANDOM-04 / trick_task_cap675 / seed0 | 1911 | 226.66 | 268.38 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
