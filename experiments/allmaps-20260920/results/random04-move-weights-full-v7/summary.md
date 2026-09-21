# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_move2_rank_squared_control / seed2 | 2298 | 260.44 | 306.29 | 0.174 | 1000 |
| RANDOM-04 / trick_move2_rank_linear / seed2 | 1974 | 249.33 | 295.41 | 0.174 | 1000 |
| RANDOM-04 / trick_move2_equal_weights / seed2 | 1535 | 237.52 | 297.64 | 0.173 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
