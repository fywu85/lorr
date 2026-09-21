# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_random_control / seed2 | 1231 | 102.09 | 145.25 | 0.116 | 1000 |
| RANDOM-04 / trick_random_field / seed2 | 1266 | 99.39 | 143.35 | 0.116 | 1000 |
| RANDOM-04 / trick_random_control / seed4 | 1367 | 102.21 | 137.28 | 0.116 | 1000 |
| RANDOM-04 / trick_random_field / seed4 | 1278 | 99.86 | 147.32 | 0.115 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
