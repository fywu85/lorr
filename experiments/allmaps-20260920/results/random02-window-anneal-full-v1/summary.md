# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_current_control / seed2 | 1215 | 271.78 | 354.41 | 0.138 | 440 |
| RANDOM-02 / trick_temperature20 / seed2 | 1159 | 287.33 | 363.74 | 0.138 | 449 |
| RANDOM-02 / trick_temperature80 / seed2 | 1212 | 291.58 | 362.08 | 0.138 | 433 |
| RANDOM-02 / trick_temperature320 / seed2 | 1180 | 284.0 | 354.65 | 0.138 | 440 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
