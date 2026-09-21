# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_current_field_control / seed0 | 2574 | 97.82 | 168.07 | 0.194 | 2000 |
| RANDOM-05 / trick_uniform_reference / seed0 | 1980 | 102.15 | 158.68 | 0.193 | 2000 |
| RANDOM-05 / trick_nms_arrows / seed0 | 2337 | 99.63 | 158.98 | 0.193 | 2000 |
| RANDOM-05 / trick_nms_arrows_squared / seed0 | 2377 | 99.8 | 196.96 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
