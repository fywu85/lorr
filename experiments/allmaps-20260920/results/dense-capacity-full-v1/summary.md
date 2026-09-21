# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / generic_capacity_control / seed0 | 1306 | 102.25 | 146.84 | 0.116 | 1000 |
| RANDOM-05 / generic_capacity_control / seed0 | 2036 | 100.53 | 299.12 | 0.193 | 2000 |
| RANDOM-04 / generic_no_pocket_certificate / seed0 | 1274 | 102.33 | 143.36 | 0.115 | 1000 |
| RANDOM-05 / generic_no_pocket_certificate / seed0 | 1953 | 101.74 | 288.48 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
