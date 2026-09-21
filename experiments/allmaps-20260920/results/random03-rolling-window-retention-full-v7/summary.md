# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_retained_control / seed0 | 1849 | 465.3 | 633.49 | 0.159 | 784 |
| RANDOM-03 / trick_keep0 / seed0 | 1939 | 490.17 | 586.09 | 0.161 | 757 |
| RANDOM-03 / trick_keep6 / seed0 | 1939 | 505.43 | 617.99 | 0.161 | 757 |
| RANDOM-03 / trick_keep19 / seed0 | 1873 | 451.99 | 611.23 | 0.159 | 778 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
