# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_fleet3250_control / seed0 | 23977 | 244.79 | 846.13 | 10.554 | 4974 |
| GAME / trick_fleet3250_bound / seed0 | 23948 | 250.61 | 849.84 | 10.553 | 4971 |
| GAME / trick_fleet3250_mean / seed0 | 23867 | 252.63 | 855.64 | 10.551 | 4985 |
| GAME / trick_fleet3250_p90 / seed0 | 23819 | 252.58 | 857.92 | 10.553 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
