# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_control / seed4 | 682 | 358.72 | 463.26 | 0.133 | 400 |
| RANDOM-01 / trick_refresh_keep6 / seed4 | 665 | 365.21 | 472.69 | 0.133 | 399 |
| RANDOM-01 / trick_refresh_keep10 / seed4 | 693 | 337.88 | 488.91 | 0.133 | 383 |
| RANDOM-01 / trick_refresh_keep14 / seed4 | 685 | 329.5 | 431.26 | 0.133 | 446 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
