# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / generic_selected_control / seed0 | 611 | 142.7 | 200.9 | 0.058 | 537 |
| RANDOM-01 / generic_chain_ranks / seed0 | 607 | 154.23 | 201.14 | 0.058 | 548 |
| RANDOM-01 / generic_portfolio8 / seed0 | 613 | 74.75 | 136.31 | 0.059 | 539 |
| RANDOM-01 / generic_chain_portfolio8 / seed0 | 603 | 54.93 | 102.41 | 0.059 | 545 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
