# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_selected / seed6 | 626 | 146.89 | 193.17 | 0.058 | 380 |
| RANDOM-01 / trick_selected / seed8 | 638 | 150.73 | 201.53 | 0.058 | 361 |
| RANDOM-01 / trick_selected / seed10 | 620 | 150.15 | 200.01 | 0.058 | 395 |
| RANDOM-01 / trick_selected / seed12 | 625 | 147.24 | 211.55 | 0.058 | 419 |
| RANDOM-01 / trick_selected / seed14 | 636 | 152.54 | 208.11 | 0.058 | 353 |
| RANDOM-01 / trick_selected / seed16 | 624 | 146.92 | 195.65 | 0.058 | 384 |
| RANDOM-01 / trick_selected / seed18 | 636 | 150.38 | 204.06 | 0.058 | 399 |
| RANDOM-01 / trick_selected / seed20 | 631 | 146.14 | 196.34 | 0.058 | 394 |
| RANDOM-01 / trick_selected / seed22 | 627 | 147.51 | 199.72 | 0.058 | 339 |
| RANDOM-01 / trick_selected / seed24 | 629 | 149.73 | 196.18 | 0.058 | 381 |
| RANDOM-01 / trick_selected / seed26 | 634 | 147.55 | 191.8 | 0.059 | 386 |
| RANDOM-01 / trick_selected / seed28 | 625 | 150.81 | 195.92 | 0.058 | 383 |
| RANDOM-01 / trick_selected / seed30 | 634 | 151.16 | 214.23 | 0.059 | 380 |
| RANDOM-01 / trick_selected / seed32 | 623 | 146.68 | 208.02 | 0.058 | 433 |
| RANDOM-01 / trick_selected / seed34 | 635 | 151.42 | 197.35 | 0.059 | 356 |
| RANDOM-01 / trick_selected / seed36 | 629 | 151.9 | 212.42 | 0.059 | 405 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
