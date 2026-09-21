# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_dispatch_control / seed0 | 15797 | 247.01 | 803.66 | 8.506 | 2623 |
| CITY-02 / trick_dispatch_pickup8 / seed0 | 16123 | 250.38 | 804.34 | 8.489 | 2598 |
| CITY-02 / trick_dispatch_pickup12 / seed0 | 16159 | 264.19 | 825.46 | 8.504 | 2547 |
| CITY-02 / trick_dispatch_pickup8_cadence1 / seed0 | 16107 | 250.81 | 810.39 | 8.509 | 2600 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
