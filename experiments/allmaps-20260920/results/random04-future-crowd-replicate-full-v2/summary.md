# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_current_control / seed0 | 2237 | 258.74 | 303.8 | 0.174 | 1000 |
| RANDOM-04 / trick_terminal_pairs16 / seed0 | 2176 | 255.84 | 305.1 | 0.174 | 1000 |
| RANDOM-04 / trick_current_control / seed4 | 2215 | 258.32 | 304.97 | 0.174 | 1000 |
| RANDOM-04 / trick_terminal_pairs16 / seed4 | 2321 | 262.87 | 302.95 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
