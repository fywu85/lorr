# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_promise2_h20_control / seed2 | 2298 | 261.22 | 306.95 | 0.174 | 1000 |
| RANDOM-04 / trick_promise2_without_futures / seed2 | 2219 | 219.36 | 260.26 | 0.117 | 1000 |
| RANDOM-04 / trick_promise2_h20_control / seed4 | 2215 | 258.12 | 303.36 | 0.174 | 1000 |
| RANDOM-04 / trick_promise2_without_futures / seed4 | 2270 | 221.33 | 285.44 | 0.118 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
