# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_generic_control / seed0 | 1484 | 135.15 | 192.79 | 0.083 | 800 |
| RANDOM-03 / trick_uniform_native / seed0 | 1150 | 128.81 | 189.68 | 0.084 | 800 |
| RANDOM-03 / trick_nms_arrows / seed0 | 1421 | 131.22 | 192.24 | 0.084 | 800 |
| RANDOM-03 / trick_kk_forward / seed0 | 1541 | 135.02 | 200.72 | 0.084 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
