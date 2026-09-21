# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_current_window_control / seed2 | 1215 | 279.97 | 363.97 | 0.140 | 440 |
| RANDOM-02 / trick_uniform_turn40 / seed2 | 1080 | 310.56 | 380.49 | 0.140 | 482 |
| RANDOM-02 / trick_current_turn40 / seed2 | 1104 | 286.25 | 373.69 | 0.138 | 482 |
| RANDOM-02 / trick_current_wait20 / seed2 | 1184 | 265.13 | 328.43 | 0.136 | 450 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
