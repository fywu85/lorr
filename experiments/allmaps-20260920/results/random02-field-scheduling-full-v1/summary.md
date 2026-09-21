# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_nms_control / seed0 | 1123 | 129.16 | 172.31 | 0.066 | 526 |
| RANDOM-02 / trick_nms_pickup4 / seed0 | 1160 | 130.02 | 185.37 | 0.066 | 441 |
| RANDOM-02 / trick_nms_pickup8 / seed0 | 1084 | 131.0 | 177.14 | 0.065 | 412 |
| RANDOM-02 / trick_nms_portfolio8 / seed0 | 1065 | 70.13 | 112.45 | 0.066 | 549 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
