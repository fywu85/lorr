# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_scheduler_control / seed2 | 2537 | 97.07 | 159.31 | 0.193 | 2000 |
| RANDOM-05 / trick_match64 / seed2 | 2475 | 97.77 | 160.64 | 0.193 | 2000 |
| RANDOM-05 / trick_match64_direct_pickup4 / seed2 | 2608 | 97.17 | 155.36 | 0.193 | 2000 |
| RANDOM-05 / trick_scheduler_control / seed4 | 2441 | 97.63 | 161.7 | 0.192 | 2000 |
| RANDOM-05 / trick_match64 / seed4 | 2532 | 97.6 | 156.44 | 0.193 | 2000 |
| RANDOM-05 / trick_match64_direct_pickup4 / seed4 | 2528 | 97.59 | 151.94 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
