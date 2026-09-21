# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_current_control / seed4 | 1197 | 130.73 | 180.06 | 0.066 | 411 |
| RANDOM-02 / trick_window512_wait / seed4 | 1032 | 202.83 | 253.33 | 0.138 | 542 |
| RANDOM-02 / trick_window512_roll / seed4 | 1049 | 201.81 | 250.65 | 0.137 | 522 |
| RANDOM-02 / trick_window2048_roll / seed4 | 1060 | 304.55 | 372.54 | 0.140 | 509 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
