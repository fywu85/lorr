# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_pickup_control / seed0 | 8378 | 251.17 | 748.0 | 8.107 | 2515 |
| CITY-01 / trick_pickup10 / seed0 | 8408 | 251.91 | 752.58 | 8.098 | 2432 |
| CITY-01 / trick_pickup12 / seed0 | 8423 | 251.5 | 759.54 | 8.113 | 2403 |
| CITY-01 / trick_pickup16 / seed0 | 8423 | 252.09 | 765.83 | 8.115 | 2352 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
