# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_uniform2750 / seed2 | 21720 | 241.95 | 820.52 | 10.561 | 5000 |
| GAME / trick_uniform3250 / seed2 | 23917 | 240.97 | 843.58 | 10.541 | 4983 |
| GAME / trick_uniform2750 / seed4 | 21752 | 238.93 | 825.44 | 10.570 | 5000 |
| GAME / trick_uniform3250 / seed4 | 23888 | 243.44 | 841.57 | 10.548 | 4973 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
