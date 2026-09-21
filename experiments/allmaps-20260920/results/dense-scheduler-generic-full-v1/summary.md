# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / generic_scheduler_control / seed0 | 1306 | 102.78 | 151.53 | 0.116 | 1000 |
| RANDOM-05 / generic_scheduler_control / seed0 | 2036 | 100.53 | 300.04 | 0.192 | 2000 |
| RANDOM-04 / generic_direct_pickup4 / seed0 | 1343 | 102.62 | 148.4 | 0.115 | 1000 |
| RANDOM-05 / generic_direct_pickup4 / seed0 | 2038 | 100.34 | 281.72 | 0.192 | 2000 |
| RANDOM-04 / generic_match64 / seed0 | 1309 | 102.73 | 146.62 | 0.115 | 1000 |
| RANDOM-05 / generic_match64 / seed0 | 2062 | 100.87 | 289.09 | 0.192 | 2000 |
| RANDOM-04 / generic_match64_direct_pickup4 / seed0 | 1503 | 102.53 | 150.43 | 0.116 | 1000 |
| RANDOM-05 / generic_match64_direct_pickup4 / seed0 | 2068 | 100.4 | 280.45 | 0.192 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
