# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_dispatch_control / seed0 | 8213 | 249.83 | 748.61 | 8.108 | 2553 |
| CITY-01 / trick_dispatch_pickup8 / seed0 | 8378 | 252.77 | 742.32 | 8.108 | 2515 |
| CITY-01 / trick_dispatch_match_every_step / seed0 | 8242 | 252.56 | 747.94 | 8.103 | 2577 |
| CITY-01 / trick_dispatch_short / seed0 | 8227 | 250.7 | 749.23 | 8.101 | 2574 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
