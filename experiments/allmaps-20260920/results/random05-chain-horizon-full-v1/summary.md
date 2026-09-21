# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_chain_control / seed0 | 3027 | 591.51 | 756.34 | 0.239 | 2000 |
| RANDOM-05 / trick_chain_horizon / seed0 | 3027 | 584.84 | 736.63 | 0.239 | 2000 |
| RANDOM-05 / trick_chain_horizon_mean / seed0 | 3043 | 587.07 | 742.14 | 0.239 | 2000 |
| RANDOM-05 / trick_chain_horizon_p90 / seed0 | 3037 | 585.14 | 733.11 | 0.239 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
