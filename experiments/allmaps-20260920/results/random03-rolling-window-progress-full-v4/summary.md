# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_roll2048_control / seed0 | 1604 | 361.55 | 500.4 | 0.165 | 800 |
| RANDOM-03 / trick_roll2048_ties / seed0 | 1418 | 348.61 | 484.31 | 0.163 | 800 |
| RANDOM-03 / trick_current2048_ties / seed0 | 1829 | 354.52 | 476.84 | 0.162 | 800 |
| RANDOM-03 / trick_current512_ties / seed0 | 1505 | 256.58 | 378.2 | 0.162 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
