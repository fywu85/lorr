# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_current_control / seed4 | 693 | 322.48 | 463.37 | 0.132 | 383 |
| RANDOM-01 / trick_width64 / seed4 | 693 | 337.67 | 490.1 | 0.133 | 383 |
| RANDOM-01 / trick_width128 / seed4 | 693 | 338.01 | 483.54 | 0.132 | 383 |
| RANDOM-01 / trick_width256 / seed4 | 693 | 323.22 | 463.74 | 0.133 | 383 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
