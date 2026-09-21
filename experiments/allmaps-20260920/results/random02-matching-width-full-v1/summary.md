# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_current_control / seed2 | 1215 | 277.98 | 360.31 | 0.138 | 440 |
| RANDOM-02 / trick_width64 / seed2 | 1194 | 282.1 | 385.11 | 0.140 | 409 |
| RANDOM-02 / trick_width128 / seed2 | 1154 | 284.28 | 353.44 | 0.140 | 449 |
| RANDOM-02 / trick_width256 / seed2 | 1154 | 277.09 | 342.86 | 0.138 | 449 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
