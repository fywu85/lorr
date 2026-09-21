# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_previous_control / seed0 | 1902 | 196.02 | 311.78 | 0.085 | 800 |
| RANDOM-03 / trick_prefix_4x8192 / seed0 | timeout | n/a | n/a | 0.125 | n/a |
| RANDOM-03 / trick_prefix_8x4096 / seed0 | 1790 | 635.49 | 869.36 | 0.199 | 800 |
| RANDOM-03 / trick_locked_8x4096 / seed0 | 1725 | 654.91 | 833.04 | 0.201 | 794 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
