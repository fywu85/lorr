# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| SORTATION / trick_match64_global2m / seed0 | 150894 | 449.47 | 989.6 | 12.404 | 656 |
| SORTATION / trick_match64_global1m / seed0 | 150760 | 339.07 | 892.44 | 12.393 | 659 |
| SORTATION / trick_match64_global1m_peak1000 / seed0 | 150760 | 339.22 | 895.11 | 12.404 | 659 |
| SORTATION / trick_match64_global1m_peak5000 / seed0 | 150780 | 337.99 | 900.7 | 12.641 | 660 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
