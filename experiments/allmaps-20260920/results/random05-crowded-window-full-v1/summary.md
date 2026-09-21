# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [10].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_current_control / seed10 | 3065 | 584.05 | 709.59 | 0.240 | 2000 |
| RANDOM-05 / trick_chain_off / seed10 | 2844 | 591.25 | 752.06 | 0.196 | 2000 |
| RANDOM-05 / trick_window20_keep0 / seed10 | 2829 | 625.25 | 797.99 | 0.272 | 2000 |
| RANDOM-05 / trick_window20_keep10 / seed10 | 2921 | 639.62 | 786.47 | 0.277 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
