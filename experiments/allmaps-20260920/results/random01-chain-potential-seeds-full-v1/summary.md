# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0, 2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_control / seed0 | 621 | 150.9 | 200.88 | 0.058 | 383 |
| RANDOM-01 / trick_chain_both / seed0 | 645 | 146.85 | 205.57 | 0.101 | 402 |
| RANDOM-01 / trick_control / seed2 | 635 | 146.15 | 197.35 | 0.058 | 355 |
| RANDOM-01 / trick_chain_both / seed2 | 647 | 146.98 | 202.09 | 0.101 | 386 |
| RANDOM-01 / trick_control / seed4 | 631 | 148.45 | 193.03 | 0.058 | 378 |
| RANDOM-01 / trick_chain_both / seed4 | 595 | 140.72 | 198.04 | 0.101 | 399 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
