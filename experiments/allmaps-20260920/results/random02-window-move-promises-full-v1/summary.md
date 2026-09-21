# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_current_control / seed2 | 1215 | 278.12 | 350.72 | 0.139 | 440 |
| RANDOM-02 / trick_window_move_promise1 / seed2 | 1197 | 258.15 | 333.48 | 0.136 | 422 |
| RANDOM-02 / trick_window_move_promise2 / seed2 | 1172 | 321.13 | 379.98 | 0.141 | 445 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
