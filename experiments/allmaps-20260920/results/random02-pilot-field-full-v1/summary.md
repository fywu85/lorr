# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_current_window_control / seed2 | 1215 | 278.96 | 355.8 | 0.136 | 440 |
| RANDOM-02 / trick_pilot_field_window / seed2 | 1061 | 282.42 | 360.44 | 0.140 | 551 |
| RANDOM-02 / trick_pilot_field_turn40 / seed2 | 1058 | 288.45 | 364.18 | 0.141 | 545 |
| RANDOM-02 / trick_pilot_field_no_window / seed2 | 1099 | 126.98 | 175.02 | 0.065 | 508 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
