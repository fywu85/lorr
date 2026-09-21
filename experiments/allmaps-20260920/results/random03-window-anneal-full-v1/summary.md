# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current_control / seed0 | 1939 | 505.5 | 613.49 | 0.161 | 757 |
| RANDOM-03 / trick_temperature20 / seed0 | 1937 | 504.1 | 645.57 | 0.160 | 766 |
| RANDOM-03 / trick_temperature80 / seed0 | 1867 | 466.86 | 637.16 | 0.164 | 794 |
| RANDOM-03 / trick_temperature320 / seed0 | 1890 | 479.82 | 600.47 | 0.160 | 764 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
