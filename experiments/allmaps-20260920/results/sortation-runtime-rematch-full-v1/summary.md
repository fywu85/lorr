# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| SORTATION / trick_two1m_control / seed0 | 150830 | 343.45 | 902.55 | 12.686 | 655 |
| SORTATION / trick_budget2 / seed0 | 150753 | 341.83 | 898.76 | 12.411 | 657 |
| SORTATION / trick_budget4 / seed0 | 150759 | 340.86 | 901.13 | 12.432 | 661 |
| SORTATION / trick_budget2_cadence1 / seed0 | 150865 | 353.61 | 894.99 | 12.383 | 657 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
