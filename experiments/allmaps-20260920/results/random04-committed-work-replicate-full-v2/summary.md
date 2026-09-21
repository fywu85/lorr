# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0, 2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_current_control / seed0 | 2176 | 259.95 | 323.73 | 0.174 | 1000 |
| RANDOM-04 / trick_rounds8 / seed0 | 2082 | 452.45 | 534.53 | 0.174 | 1000 |
| RANDOM-04 / trick_workers16_rounds8 / seed0 | 2198 | 469.53 | 569.83 | 0.175 | 1000 |
| RANDOM-04 / trick_current_control / seed2 | 2304 | 263.02 | 296.4 | 0.174 | 1000 |
| RANDOM-04 / trick_rounds8 / seed2 | 2326 | 463.15 | 535.24 | 0.174 | 1000 |
| RANDOM-04 / trick_workers16_rounds8 / seed2 | 2275 | 467.63 | 547.42 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
