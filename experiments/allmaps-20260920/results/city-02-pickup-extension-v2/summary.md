# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_pickup_control / seed0 | 16159 | 251.53 | 804.96 | 8.503 | 2547 |
| CITY-02 / trick_pickup16 / seed0 | 16162 | 249.19 | 816.02 | 8.513 | 2535 |
| CITY-02 / trick_pickup16_short / seed0 | 16112 | 249.94 | 798.32 | 8.505 | 2492 |
| CITY-02 / trick_pickup12_pickup_groups / seed0 | 16169 | 248.33 | 803.04 | 8.511 | 2564 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
