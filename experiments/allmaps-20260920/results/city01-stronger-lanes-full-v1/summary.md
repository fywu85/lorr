# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-01 / trick_current_control / seed2 | 8440 | 256.21 | 756.76 | 8.093 | 2342 |
| CITY-01 / trick_lane24 / seed2 | 8413 | 252.8 | 753.19 | 8.089 | 2339 |
| CITY-01 / trick_lane32 / seed2 | 8409 | 253.56 | 751.72 | 8.110 | 2322 |
| CITY-01 / trick_lane48 / seed2 | 8394 | 253.87 | 745.01 | 8.105 | 2325 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
