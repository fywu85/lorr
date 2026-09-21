# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_current_control / seed2 | 1901 | 225.53 | 276.87 | 0.174 | 1000 |
| RANDOM-04 / trick_move_promise1 / seed2 | 2172 | 252.87 | 306.95 | 0.174 | 1000 |
| RANDOM-04 / trick_move_promise2 / seed2 | 2298 | 258.44 | 306.43 | 0.174 | 1000 |
| RANDOM-04 / trick_current_control / seed4 | 1843 | 226.55 | 281.74 | 0.174 | 1000 |
| RANDOM-04 / trick_move_promise1 / seed4 | 2110 | 251.5 | 320.4 | 0.174 | 1000 |
| RANDOM-04 / trick_move_promise2 / seed4 | 2215 | 258.18 | 303.74 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
