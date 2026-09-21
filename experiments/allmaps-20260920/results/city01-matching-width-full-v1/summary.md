# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_current_control / seed2 | 8440 | 255.66 | 759.11 | 8.093 | 2342 |
| CITY-01 / trick_width64 / seed2 | 8440 | 255.73 | 751.63 | 8.094 | 2342 |
| CITY-01 / trick_width128 / seed2 | 8435 | 254.08 | 754.81 | 8.130 | 2332 |
| CITY-01 / trick_width256 / seed2 | 8434 | 257.11 | 789.7 | 8.093 | 2343 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
