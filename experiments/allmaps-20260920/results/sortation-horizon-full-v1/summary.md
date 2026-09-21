# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| SORTATION / trick_current_control / seed0 | 150894 | 451.04 | 988.29 | 12.396 | 656 |
| SORTATION / trick_horizon_mean / seed0 | 153963 | 455.92 | 995.89 | 12.403 | 551 |
| SORTATION / trick_horizon_p90 / seed0 | 154160 | 451.61 | 986.97 | 12.404 | 541 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
