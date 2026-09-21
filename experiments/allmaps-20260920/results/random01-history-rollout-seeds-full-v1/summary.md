# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0, 2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_control / seed0 | 671 | 362.87 | 524.65 | 0.133 | 408 |
| RANDOM-01 / trick_refresh_keep10 / seed0 | 685 | 340.51 | 423.41 | 0.133 | 374 |
| RANDOM-01 / trick_current_control / seed2 | 668 | 354.88 | 493.56 | 0.135 | 401 |
| RANDOM-01 / trick_refresh_keep10 / seed2 | 681 | 343.84 | 482.14 | 0.137 | 412 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
