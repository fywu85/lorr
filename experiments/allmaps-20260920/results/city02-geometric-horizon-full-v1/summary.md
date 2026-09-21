# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_selected_control / seed0 | 16315 | 256.68 | 789.14 | 8.508 | 2540 |
| CITY-02 / trick_geometric_bound / seed0 | 16287 | 255.26 | 825.92 | 8.506 | 2530 |
| CITY-02 / trick_geometric_mean / seed0 | 16241 | 258.87 | 830.55 | 8.511 | 2541 |
| CITY-02 / trick_geometric_p90 / seed0 | 16212 | 259.59 | 821.07 | 8.508 | 2543 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
