# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_current_control / seed2 | 1215 | 270.03 | 344.66 | 0.137 | 440 |
| RANDOM-02 / trick_delay1 / seed2 | 1182 | 289.43 | 361.51 | 0.139 | 414 |
| RANDOM-02 / trick_delay3 / seed2 | timeout | n/a | n/a | 0.126 | n/a |
| RANDOM-02 / trick_delay7 / seed2 | 1187 | 310.36 | 394.09 | 0.138 | 443 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
