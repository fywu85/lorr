# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [10].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_current_control / seed10 | 3065 | 586.87 | 702.67 | 0.240 | 2000 |
| RANDOM-05 / trick_chain_window10_keep0 / seed10 | 3008 | 601.47 | 795.9 | 0.258 | 2000 |
| RANDOM-05 / trick_chain_window20_keep0 / seed10 | 2896 | 629.37 | 766.6 | 0.272 | 2000 |
| RANDOM-05 / trick_chain_window20_keep10 / seed10 | 2985 | 635.89 | 790.87 | 0.272 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
