# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_selected_control / seed2 | 2608 | 100.11 | 160.42 | 0.193 | 2000 |
| RANDOM-05 / trick_region8m / seed2 | 2647 | 158.64 | 197.93 | 0.193 | 2000 |
| RANDOM-05 / trick_selected_control / seed4 | 2528 | 97.53 | 158.38 | 0.193 | 2000 |
| RANDOM-05 / trick_region8m / seed4 | 2684 | 158.96 | 213.69 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
