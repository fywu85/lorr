# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_seed_control / seed4 | 1197 | 130.49 | 178.19 | 0.065 | 411 |
| RANDOM-02 / trick_window_current / seed4 | 955 | 160.07 | 206.87 | 0.144 | 552 |
| RANDOM-02 / trick_uniform_seed / seed4 | flag_mismatch | n/a | n/a | 0.009 | n/a |
| RANDOM-02 / trick_window_uniform / seed4 | flag_mismatch | n/a | n/a | 0.009 | n/a |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
