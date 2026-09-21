# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_selected_control / seed0 | 15574 | 267.2 | 896.72 | 10.550 | 5000 |
| GAME / trick_uniform2750 / seed0 | 21742 | 240.83 | 835.91 | 10.562 | 5000 |
| GAME / trick_tabu2750 / seed0 | 21648 | 243.84 | 819.27 | 10.508 | 5000 |
| GAME / trick_tabu4000 / seed0 | 17771 | 243.6 | 870.84 | 12.243 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
