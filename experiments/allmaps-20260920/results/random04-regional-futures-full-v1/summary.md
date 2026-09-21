# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_global_h20_control / seed0 | 2059 | 225.82 | 287.97 | 0.174 | 1000 |
| RANDOM-04 / trick_regional_h20 / seed0 | 1853 | 225.83 | 292.72 | 0.174 | 1000 |
| RANDOM-04 / trick_regional_h15 / seed0 | 1947 | 218.09 | 278.08 | 0.174 | 1000 |
| RANDOM-04 / trick_regional_h10 / seed0 | 1905 | 210.87 | 268.01 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
