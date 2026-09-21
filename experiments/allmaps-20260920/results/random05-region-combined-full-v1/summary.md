# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_round4_control / seed0 | 2806 | 291.04 | 341.26 | 0.194 | 2000 |
| RANDOM-05 / trick_round4_region16m / seed0 | 2810 | 533.3 | 599.48 | 0.193 | 2000 |
| RANDOM-05 / trick_round8_region8m / seed0 | 2877 | 553.99 | 628.73 | 0.194 | 2000 |
| RANDOM-05 / trick_round4_budget2 / seed0 | 2778 | 290.34 | 340.55 | 0.193 | 2000 |
| RANDOM-05 / trick_round4_budget4_cadence1 / seed0 | 2815 | 290.46 | 355.73 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
