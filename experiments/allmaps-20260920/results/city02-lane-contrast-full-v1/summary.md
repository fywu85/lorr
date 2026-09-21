# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_lane16_control / seed0 | 16315 | 253.97 | 831.98 | 8.510 | 2540 |
| CITY-02 / trick_lane12 / seed0 | 15671 | 246.27 | 808.45 | 8.488 | 2595 |
| CITY-02 / trick_lane8 / seed0 | 15729 | 249.95 | 829.05 | 8.482 | 2592 |
| CITY-02 / trick_lane4 / seed0 | 14990 | 244.34 | 746.13 | 8.514 | 2614 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
