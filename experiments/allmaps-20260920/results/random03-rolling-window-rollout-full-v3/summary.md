# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current_control / seed0 | 1902 | 187.06 | 303.06 | 0.085 | 800 |
| RANDOM-03 / trick_window512_wait / seed0 | 1390 | 239.64 | 394.01 | 0.160 | 800 |
| RANDOM-03 / trick_window512_roll / seed0 | 1386 | 236.52 | 369.67 | 0.162 | 800 |
| RANDOM-03 / trick_window2048_roll / seed0 | 1604 | 342.79 | 489.25 | 0.163 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
