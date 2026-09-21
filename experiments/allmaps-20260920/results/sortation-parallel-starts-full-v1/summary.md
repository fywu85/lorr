# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| SORTATION / trick_one2m_control / seed0 | 150894 | 450.87 | 998.94 | 12.409 | 656 |
| SORTATION / trick_two1m / seed0 | 150830 | 342.51 | 888.48 | 12.801 | 655 |
| SORTATION / trick_four500k / seed0 | 150685 | 287.88 | 855.57 | 12.378 | 660 |
| SORTATION / trick_four1m / seed0 | 150798 | 342.04 | 902.67 | 12.634 | 656 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
