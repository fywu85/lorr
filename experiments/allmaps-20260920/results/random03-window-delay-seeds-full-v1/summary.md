# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current_control / seed2 | 1900 | 491.59 | 618.21 | 0.164 | 766 |
| RANDOM-03 / trick_delay3 / seed2 | 1982 | 484.42 | 662.92 | 0.165 | 748 |
| RANDOM-03 / trick_current_control / seed4 | 1870 | 486.84 | 662.38 | 0.161 | 800 |
| RANDOM-03 / trick_delay3 / seed4 | 1948 | 482.09 | 631.69 | 0.158 | 762 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
