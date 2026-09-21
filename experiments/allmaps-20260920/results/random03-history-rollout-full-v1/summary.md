# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current_control / seed0 | 1939 | 694.02 | 862.42 | 0.160 | 757 |
| RANDOM-03 / trick_old_keep14 / seed0 | 1849 | 428.4 | 600.7 | 0.162 | 784 |
| RANDOM-03 / trick_refresh_keep14 / seed0 | 1933 | 487.3 | 621.44 | 0.165 | 744 |
| RANDOM-03 / trick_refresh_keep6 / seed0 | 1924 | 490.51 | 622.26 | 0.162 | 745 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
