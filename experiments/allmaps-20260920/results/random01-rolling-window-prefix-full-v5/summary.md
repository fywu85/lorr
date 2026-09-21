# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current2048_control / seed2 | 642 | 250.91 | 316.93 | 0.132 | 401 |
| RANDOM-01 / trick_current2048_prefix / seed2 | 660 | 252.13 | 310.68 | 0.135 | 413 |
| RANDOM-01 / trick_current4096_prefix / seed2 | 662 | 327.6 | 431.85 | 0.134 | 372 |
| RANDOM-01 / trick_uniform2048_prefix / seed2 | 656 | 293.95 | 389.48 | 0.135 | 364 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
