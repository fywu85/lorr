# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current_control / seed0 | 1902 | 188.38 | 306.36 | 0.085 | 800 |
| RANDOM-03 / trick_pilot_field / seed0 | 1569 | 176.38 | 309.55 | 0.084 | 800 |
| RANDOM-03 / trick_current_window / seed0 | 1939 | 505.44 | 612.54 | 0.161 | 757 |
| RANDOM-03 / trick_pilot_field_window / seed0 | 1870 | 449.97 | 587.16 | 0.157 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
