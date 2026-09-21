# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_round2_control / seed0 | 1622 | 104.07 | 142.58 | 0.116 | 1000 |
| RANDOM-04 / trick_round4 / seed0 | 1743 | 183.63 | 231.06 | 0.116 | 1000 |
| RANDOM-04 / trick_round8 / seed0 | 1656 | 343.6 | 457.81 | 0.116 | 1000 |
| RANDOM-04 / trick_parts2_round4 / seed0 | 1610 | 184.7 | 236.87 | 0.117 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
