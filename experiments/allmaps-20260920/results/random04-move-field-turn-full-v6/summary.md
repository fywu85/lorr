# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_move2_current_control / seed2 | 2298 | 258.59 | 300.22 | 0.174 | 1000 |
| RANDOM-04 / trick_move2_turn20 / seed2 | 1908 | 313.43 | 486.15 | 0.174 | 1000 |
| RANDOM-04 / trick_move2_pilot_field / seed2 | 2056 | 246.15 | 302.81 | 0.174 | 1000 |
| RANDOM-04 / trick_move2_pilot_field_turn20 / seed2 | 1987 | 258.83 | 308.84 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
