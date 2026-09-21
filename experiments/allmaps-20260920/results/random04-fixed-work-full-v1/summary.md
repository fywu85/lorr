# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_selected_control / seed0 | 1622 | 105.07 | 145.51 | 0.117 | 1000 |
| RANDOM-04 / trick_global2m_each / seed0 | 1556 | 134.73 | 167.08 | 0.116 | 1000 |
| RANDOM-04 / trick_global4m_each / seed0 | 1601 | 173.72 | 221.86 | 0.116 | 1000 |
| RANDOM-04 / trick_region8m / seed0 | 1582 | 174.35 | 241.98 | 0.116 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
