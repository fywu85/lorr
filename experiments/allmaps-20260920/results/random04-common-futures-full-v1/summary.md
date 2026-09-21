# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_current_control / seed0 | 1999 | 186.0 | 252.79 | 0.118 | 1000 |
| RANDOM-04 / trick_future_r4_h10_b2 / seed0 | 1928 | 208.42 | 261.51 | 0.174 | 1000 |
| RANDOM-04 / trick_future_r8_h10_b2 / seed0 | 1890 | 221.77 | 274.4 | 0.180 | 1000 |
| RANDOM-04 / trick_future_r4_h15_b2 / seed0 | 2023 | 215.86 | 268.97 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
