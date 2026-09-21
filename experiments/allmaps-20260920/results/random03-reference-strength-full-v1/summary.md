# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current_control / seed2 | 1982 | 510.26 | 650.56 | 0.167 | 748 |
| RANDOM-03 / trick_reference_strength25 / seed2 | 1430 | 403.67 | 683.91 | 0.165 | 800 |
| RANDOM-03 / trick_reference_strength50 / seed2 | 1668 | 410.63 | 624.01 | 0.165 | 800 |
| RANDOM-03 / trick_reference_strength75 / seed2 | 1717 | 428.88 | 647.42 | 0.166 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
