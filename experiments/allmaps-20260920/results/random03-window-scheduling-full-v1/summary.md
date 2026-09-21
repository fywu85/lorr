# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current_control / seed2 | 1982 | 467.8 | 602.23 | 0.165 | 748 |
| RANDOM-03 / trick_horizon_mean / seed2 | 1982 | 504.89 | 648.05 | 0.166 | 744 |
| RANDOM-03 / trick_rematch4_every_step / seed2 | 1872 | 455.25 | 623.61 | 0.163 | 779 |
| RANDOM-03 / trick_horizon_rematch4 / seed2 | 1883 | 449.09 | 632.19 | 0.166 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
