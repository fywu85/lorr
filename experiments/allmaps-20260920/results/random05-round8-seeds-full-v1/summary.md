# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_round4_control / seed2 | 2809 | 290.23 | 327.38 | 0.193 | 2000 |
| RANDOM-05 / trick_round8 / seed2 | 2865 | 554.13 | 611.17 | 0.193 | 2000 |
| RANDOM-05 / trick_round4_control / seed4 | 2718 | 290.13 | 325.72 | 0.193 | 2000 |
| RANDOM-05 / trick_round8 / seed4 | 2836 | 554.38 | 621.42 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
