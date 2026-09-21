# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_scheduler_control / seed0 | 1353 | 100.46 | 139.04 | 0.116 | 1000 |
| RANDOM-04 / trick_direct_pickup4 / seed0 | 1481 | 98.98 | 147.77 | 0.117 | 1000 |
| RANDOM-04 / trick_match64 / seed0 | 1356 | 99.99 | 139.19 | 0.116 | 1000 |
| RANDOM-04 / trick_match64_direct_pickup4 / seed0 | 1338 | 100.52 | 145.42 | 0.116 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
