# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_kk_turn6 / seed2 | 1803 | 183.42 | 245.28 | 0.117 | 1000 |
| RANDOM-04 / trick_kk_turn2 / seed2 | 1904 | 184.66 | 248.25 | 0.117 | 1000 |
| RANDOM-04 / trick_kk_turn6 / seed4 | 1935 | 184.75 | 231.62 | 0.117 | 1000 |
| RANDOM-04 / trick_kk_turn2 / seed4 | 1796 | 181.24 | 232.21 | 0.117 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
