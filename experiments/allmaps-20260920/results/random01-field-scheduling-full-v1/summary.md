# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_nms_control / seed0 | 613 | 152.49 | 203.68 | 0.058 | 540 |
| RANDOM-01 / trick_nms_pickup4 / seed0 | 621 | 152.44 | 201.28 | 0.058 | 383 |
| RANDOM-01 / trick_nms_pickup8 / seed0 | 617 | 146.29 | 201.39 | 0.058 | 317 |
| RANDOM-01 / trick_nms_portfolio8 / seed0 | 585 | 54.61 | 102.25 | 0.058 | 511 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
