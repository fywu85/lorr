# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_random_control / seed2 | 2031 | 100.29 | 261.69 | 0.192 | 2000 |
| RANDOM-05 / trick_random_field / seed2 | 2537 | 97.07 | 153.6 | 0.192 | 2000 |
| RANDOM-05 / trick_random_control / seed4 | 2031 | 100.29 | 316.1 | 0.192 | 2000 |
| RANDOM-05 / trick_random_field / seed4 | 2441 | 97.17 | 150.6 | 0.192 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
