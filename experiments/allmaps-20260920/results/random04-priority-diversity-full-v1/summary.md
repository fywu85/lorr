# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_workers8_control / seed0 | 1999 | 187.17 | 246.96 | 0.117 | 1000 |
| RANDOM-04 / trick_workers16_equal / seed0 | 1873 | 202.73 | 251.9 | 0.118 | 1000 |
| RANDOM-04 / trick_workers32_equal / seed0 | 1985 | 203.93 | 260.5 | 0.120 | 1000 |
| RANDOM-04 / trick_workers32_extra / seed0 | 1982 | 226.17 | 291.01 | 0.120 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
