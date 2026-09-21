# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_scheduler_control / seed0 | 2457 | 97.85 | 166.34 | 0.193 | 2000 |
| RANDOM-05 / trick_direct_pickup4 / seed0 | 2421 | 97.82 | 165.31 | 0.192 | 2000 |
| RANDOM-05 / trick_match64 / seed0 | 2544 | 97.95 | 167.73 | 0.193 | 2000 |
| RANDOM-05 / trick_match64_direct_pickup4 / seed0 | 2574 | 97.8 | 169.54 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
