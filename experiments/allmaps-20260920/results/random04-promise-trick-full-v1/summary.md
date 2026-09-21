# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_control / seed0 | 1481 | 138.61 | 226.56 | 0.119 | 1000 |
| RANDOM-04 / trick_after_turn / seed0 | 1392 | 129.68 | 212.17 | 0.119 | 1000 |
| RANDOM-04 / trick_short_preference / seed0 | 1346 | 121.78 | 175.24 | 0.119 | 1000 |
| RANDOM-04 / trick_after_turn_short / seed0 | 1334 | 118.06 | 171.16 | 0.118 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
