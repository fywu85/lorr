# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current_control / seed2 | 1982 | 484.85 | 635.56 | 0.166 | 748 |
| RANDOM-03 / trick_keep14_refresh / seed2 | 1718 | 413.1 | 604.21 | 0.166 | 800 |
| RANDOM-03 / trick_temperature40 / seed2 | 1642 | 423.46 | 601.94 | 0.164 | 800 |
| RANDOM-03 / trick_keep14_temperature40 / seed2 | 1801 | 438.39 | 634.15 | 0.166 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
