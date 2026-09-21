# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [10].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_current_control / seed10 | 3065 | 593.16 | 714.39 | 0.239 | 2000 |
| RANDOM-05 / trick_paid / seed10 | 2544 | 519.01 | 580.62 | 0.239 | 2000 |
| RANDOM-05 / trick_strict / seed10 | 3018 | 576.17 | 688.26 | 0.239 | 2000 |
| RANDOM-05 / trick_paid_strict / seed10 | 2553 | 519.8 | 590.1 | 0.239 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
