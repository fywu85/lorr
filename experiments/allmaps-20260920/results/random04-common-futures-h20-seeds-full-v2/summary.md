# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_h15_control / seed2 | 1960 | 219.65 | 278.76 | 0.174 | 1000 |
| RANDOM-04 / trick_h20 / seed2 | 1901 | 228.03 | 283.68 | 0.174 | 1000 |
| RANDOM-04 / trick_h15_control / seed4 | 1913 | 219.35 | 282.59 | 0.174 | 1000 |
| RANDOM-04 / trick_h20 / seed4 | 1843 | 229.12 | 293.95 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
