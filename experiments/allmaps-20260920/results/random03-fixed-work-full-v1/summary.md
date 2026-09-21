# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_selected_control / seed0 | 1890 | 145.56 | 193.69 | 0.084 | 782 |
| RANDOM-03 / trick_global8m / seed0 | 1902 | 191.46 | 298.41 | 0.085 | 800 |
| RANDOM-03 / trick_global16m / seed0 | 1810 | 284.34 | 555.53 | 0.084 | 787 |
| RANDOM-03 / trick_four_proposals / seed0 | 1868 | 146.96 | 202.01 | 0.085 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
