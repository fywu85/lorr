# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_squared_generic_control / seed0 | 1622 | 106.83 | 142.54 | 0.116 | 1000 |
| RANDOM-04 / trick_uniform_reference / seed0 | 1307 | 100.72 | 140.27 | 0.116 | 1000 |
| RANDOM-04 / trick_nms_arrows / seed0 | 1506 | 101.61 | 153.39 | 0.116 | 1000 |
| RANDOM-04 / trick_kk_forward / seed0 | 1569 | 103.48 | 140.17 | 0.116 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
