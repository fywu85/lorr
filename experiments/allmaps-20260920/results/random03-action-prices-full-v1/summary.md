# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current_control / seed2 | 1982 | 481.87 | 624.52 | 0.166 | 748 |
| RANDOM-03 / trick_turn40 / seed2 | 1799 | 441.69 | 653.21 | 0.164 | 763 |
| RANDOM-03 / trick_wait20 / seed2 | 1806 | 440.07 | 637.97 | 0.166 | 800 |
| RANDOM-03 / trick_turn40_wait20 / seed2 | 1575 | 397.69 | 599.89 | 0.165 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
