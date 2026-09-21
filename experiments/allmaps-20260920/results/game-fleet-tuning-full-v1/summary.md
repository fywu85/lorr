# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_uniform2750 / seed0 | 21742 | 250.11 | 856.31 | 10.560 | 5000 |
| GAME / trick_uniform2000 / seed0 | 17341 | 253.4 | 810.31 | 10.583 | 5000 |
| GAME / trick_uniform2250 / seed0 | 18910 | 243.94 | 796.06 | 10.565 | 5000 |
| GAME / trick_uniform2500 / seed0 | 20392 | 239.41 | 825.23 | 10.591 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
