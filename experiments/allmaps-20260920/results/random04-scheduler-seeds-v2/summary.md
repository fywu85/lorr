# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / generic_scheduler_control / seed2 | 1231 | 102.14 | 141.0 | 0.115 | 1000 |
| RANDOM-04 / generic_match64_direct_pickup4 / seed2 | 1480 | 102.54 | 137.96 | 0.116 | 1000 |
| RANDOM-04 / generic_scheduler_control / seed4 | 1367 | 101.8 | 138.85 | 0.115 | 1000 |
| RANDOM-04 / generic_match64_direct_pickup4 / seed4 | 1494 | 102.53 | 145.83 | 0.116 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
