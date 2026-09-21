# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_current_control / seed2 | 2298 | 262.34 | 311.62 | 0.174 | 1000 |
| RANDOM-04 / trick_terminal_pairs8 / seed2 | 2268 | 263.36 | 308.0 | 0.174 | 1000 |
| RANDOM-04 / trick_terminal_pairs16 / seed2 | 2304 | 262.62 | 304.05 | 0.174 | 1000 |
| RANDOM-04 / trick_terminal_pairs32 / seed2 | 2292 | 263.64 | 313.58 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
