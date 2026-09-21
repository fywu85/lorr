# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_region8m_control / seed0 | 2684 | 159.27 | 215.2 | 0.194 | 2000 |
| RANDOM-05 / trick_region16m / seed0 | 2704 | 281.17 | 629.43 | 0.193 | 2000 |
| RANDOM-05 / trick_region32m / seed0 | 2805 | 523.92 | 587.52 | 0.193 | 2000 |
| RANDOM-05 / trick_region8m_rounds4 / seed0 | 2806 | 290.58 | 343.55 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
