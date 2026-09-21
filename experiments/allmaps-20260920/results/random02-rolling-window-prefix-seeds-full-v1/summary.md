# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0, 2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_previous_control / seed0 | 1188 | 131.79 | 197.09 | 0.065 | 405 |
| RANDOM-02 / trick_window_prefix / seed0 | 1180 | 283.68 | 342.84 | 0.143 | 440 |
| RANDOM-02 / trick_previous_control / seed2 | 1189 | 131.88 | 192.05 | 0.065 | 443 |
| RANDOM-02 / trick_window_prefix / seed2 | 1215 | 280.6 | 359.38 | 0.142 | 440 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
