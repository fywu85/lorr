# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_uniform3250 / seed0 | 23977 | 250.33 | 842.29 | 10.561 | 4974 |
| GAME / trick_uniform3125 / seed0 | 23556 | 246.28 | 830.4 | 10.541 | 5000 |
| GAME / trick_uniform3375 / seed0 | 23938 | 246.75 | 846.13 | 10.533 | 4969 |
| GAME / trick_uniform3625 / seed0 | 19642 | 244.86 | 851.31 | 10.530 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
