# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_temperature1000_peak0 / seed2 | 2865 | 612.48 | 676.43 | 0.193 | 2000 |
| RANDOM-05 / trick_temperature1000_peak1 / seed2 | 2781 | 613.44 | 703.1 | 0.193 | 2000 |
| RANDOM-05 / trick_temperature1000_peak0 / seed4 | 2836 | 561.68 | 624.73 | 0.193 | 2000 |
| RANDOM-05 / trick_temperature1000_peak1 / seed4 | 2827 | 561.0 | 631.4 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
