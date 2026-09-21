# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_uniform2750 / seed0 | 21742 | 262.0 | 879.28 | 10.569 | 5000 |
| GAME / trick_uniform3000 / seed0 | 23010 | 242.48 | 845.77 | 10.482 | 5000 |
| GAME / trick_uniform3250 / seed0 | 23977 | 241.68 | 842.09 | 10.556 | 4974 |
| GAME / trick_uniform3500 / seed0 | 22556 | 244.08 | 841.17 | 10.540 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
