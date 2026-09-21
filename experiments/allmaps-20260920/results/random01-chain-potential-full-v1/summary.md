# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [8].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_control / seed8 | 638 | 151.04 | 204.06 | 0.058 | 361 |
| RANDOM-01 / trick_chain_score / seed8 | 636 | 148.83 | 204.5 | 0.101 | 439 |
| RANDOM-01 / trick_order2_control / seed8 | 633 | 146.84 | 193.44 | 0.058 | 379 |
| RANDOM-01 / trick_chain_both / seed8 | 644 | 146.94 | 202.62 | 0.102 | 412 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
