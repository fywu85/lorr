# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [10].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_current_control / seed10 | 3065 | 578.44 | 691.14 | 0.240 | 2000 |
| RANDOM-05 / trick_regional_h10 / seed10 | 2937 | 616.26 | 771.87 | 0.249 | 2000 |
| RANDOM-05 / trick_regional_h15 / seed10 | 2978 | 618.68 | 765.16 | 0.248 | 2000 |
| RANDOM-05 / trick_regional_h20 / seed10 | 2971 | 635.86 | 777.3 | 0.249 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
