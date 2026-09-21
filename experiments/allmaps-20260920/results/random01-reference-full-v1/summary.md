# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_generic_control / seed0 | 611 | 143.78 | 206.45 | 0.058 | 537 |
| RANDOM-01 / trick_uniform_native / seed0 | 609 | 137.14 | 198.02 | 0.058 | 545 |
| RANDOM-01 / trick_nms_arrows / seed0 | 613 | 151.97 | 204.82 | 0.058 | 540 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
