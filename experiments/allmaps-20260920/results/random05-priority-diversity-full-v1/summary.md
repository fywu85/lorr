# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_workers8_control / seed0 | 2877 | 563.04 | 639.07 | 0.194 | 2000 |
| RANDOM-05 / trick_workers16_equal / seed0 | 2901 | 618.09 | 684.01 | 0.195 | 2000 |
| RANDOM-05 / trick_workers32_equal / seed0 | 2956 | 588.99 | 766.89 | 0.196 | 2000 |
| RANDOM-05 / trick_workers32_extra / seed0 | 2921 | 610.77 | 777.84 | 0.195 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
