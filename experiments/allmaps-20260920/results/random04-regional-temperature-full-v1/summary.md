# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_temperature1000 / seed0 | 1801 | 183.28 | 252.23 | 0.117 | 1000 |
| RANDOM-04 / trick_temperature0 / seed0 | 1768 | 182.27 | 245.55 | 0.117 | 1000 |
| RANDOM-04 / trick_temperature100 / seed0 | 1636 | 182.75 | 234.41 | 0.116 | 1000 |
| RANDOM-04 / trick_temperature5000 / seed0 | 1885 | 183.29 | 231.01 | 0.117 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
