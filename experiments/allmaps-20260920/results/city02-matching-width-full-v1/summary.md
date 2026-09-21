# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_current_control / seed0 | 16315 | 253.14 | 814.3 | 8.509 | 2540 |
| CITY-02 / trick_width64 / seed0 | 16315 | 253.01 | 825.84 | 8.511 | 2540 |
| CITY-02 / trick_width128 / seed0 | 16260 | 251.88 | 814.36 | 8.508 | 2546 |
| CITY-02 / trick_width256 / seed0 | 16203 | 250.66 | 818.2 | 8.508 | 2545 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
