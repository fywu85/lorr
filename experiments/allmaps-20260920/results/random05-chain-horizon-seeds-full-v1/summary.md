# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_chain_control / seed2 | 3010 | 589.28 | 755.22 | 0.239 | 2000 |
| RANDOM-05 / trick_chain_horizon_mean / seed2 | 3015 | 587.51 | 702.01 | 0.239 | 2000 |
| RANDOM-05 / trick_chain_control / seed4 | 3022 | 589.79 | 730.49 | 0.239 | 2000 |
| RANDOM-05 / trick_chain_horizon_mean / seed4 | 3018 | 588.66 | 707.73 | 0.239 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
