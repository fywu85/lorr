# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| SORTATION / trick_sortation_global2000000 / seed0 | 4190 | 441.11 | 967.46 | 8.035 | 200 |
| SORTATION / trick_sortation_global1000000 / seed0 | 4171 | 380.23 | 877.15 | 8.042 | 200 |
| SORTATION / trick_sortation_global500000 / seed0 | 4186 | 350.16 | 846.35 | 8.213 | 200 |
| SORTATION / trick_sortation_global250000 / seed0 | 4167 | 336.39 | 830.08 | 8.021 | 200 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
