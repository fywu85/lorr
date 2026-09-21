# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_diversity32_control / seed0 | 2956 | 588.42 | 656.57 | 0.200 | 2000 |
| RANDOM-05 / trick_diversity32_parts8_round12 / seed0 | 2926 | 497.24 | 612.09 | 0.199 | 2000 |
| RANDOM-05 / trick_diversity32_parts8_round16 / seed0 | 2893 | 652.93 | 762.46 | 0.199 | 2000 |
| RANDOM-05 / trick_diversity32_parts4_round12 / seed0 | 2946 | 866.42 | 947.48 | 0.199 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
