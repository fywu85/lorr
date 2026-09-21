# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_current_control / seed2 | 1938 | 187.77 | 240.49 | 0.118 | 1000 |
| RANDOM-04 / trick_future_r4_h15_b2 / seed2 | 1960 | 217.22 | 268.85 | 0.174 | 1000 |
| RANDOM-04 / trick_current_control / seed4 | 1876 | 183.67 | 248.41 | 0.117 | 1000 |
| RANDOM-04 / trick_future_r4_h15_b2 / seed4 | 1913 | 218.17 | 276.19 | 0.173 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
