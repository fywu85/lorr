# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current_control / seed0 | 1939 | 484.38 | 582.28 | 0.160 | 757 |
| RANDOM-03 / trick_delay1 / seed0 | 1864 | 462.91 | 624.38 | 0.161 | 778 |
| RANDOM-03 / trick_delay3 / seed0 | 1967 | 484.98 | 611.68 | 0.161 | 752 |
| RANDOM-03 / trick_delay7 / seed0 | 1644 | 425.21 | 633.29 | 0.160 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
