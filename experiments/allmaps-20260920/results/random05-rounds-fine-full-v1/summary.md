# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_round8 / seed0 | 2877 | 555.49 | 625.58 | 0.193 | 2000 |
| RANDOM-05 / trick_round6 / seed0 | 2741 | 424.13 | 485.9 | 0.193 | 2000 |
| RANDOM-05 / trick_round10 / seed0 | 2861 | 685.82 | 759.19 | 0.193 | 2000 |
| RANDOM-05 / trick_round12 / seed0 | 2915 | 817.73 | 895.64 | 0.194 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
