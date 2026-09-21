# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_current_control / seed2 | 8440 | 256.21 | 756.89 | 8.094 | 2342 |
| CITY-01 / trick_bound / seed2 | 8410 | 254.66 | 755.34 | 8.095 | 2340 |
| CITY-01 / trick_mean / seed2 | 8427 | 255.18 | 762.74 | 8.099 | 2344 |
| CITY-01 / trick_p90 / seed2 | 8357 | 256.09 | 770.28 | 8.093 | 2357 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
