# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_fleet3250_control / seed0 | 23977 | 241.71 | 837.87 | 10.572 | 4974 |
| GAME / trick_fleet3250_pickup8 / seed0 | 24287 | 243.77 | 829.35 | 10.529 | 4965 |
| GAME / trick_fleet3250_pickup12 / seed0 | 24326 | 242.83 | 827.91 | 10.553 | 4961 |
| GAME / trick_fleet3250_cadence1 / seed0 | 23992 | 243.03 | 824.92 | 10.565 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
