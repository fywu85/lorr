# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_budget1_control / seed2 | 16171 | 254.2 | 815.42 | 8.513 | 2562 |
| CITY-02 / trick_budget2 / seed2 | 16199 | 246.67 | 815.84 | 8.950 | 2543 |
| CITY-02 / trick_budget1_control / seed4 | 15675 | 252.18 | 847.9 | 8.509 | 2568 |
| CITY-02 / trick_budget2 / seed4 | 16117 | 248.35 | 812.79 | 8.505 | 2556 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
