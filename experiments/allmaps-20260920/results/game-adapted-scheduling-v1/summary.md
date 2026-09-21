# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_adapted_control / seed0 | 10080 | 262.94 | 892.37 | 10.512 | 5000 |
| GAME / trick_adapted_short / seed0 | 10540 | 257.2 | 892.29 | 10.538 | 5000 |
| GAME / trick_adapted_direct / seed0 | 10765 | 258.14 | 886.22 | 10.554 | 5000 |
| GAME / trick_adapted_direct_match64 / seed0 | 11146 | 258.32 | 893.9 | 10.524 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
