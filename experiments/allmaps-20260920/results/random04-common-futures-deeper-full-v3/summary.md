# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_h20_control / seed0 | 2059 | 226.84 | 286.45 | 0.174 | 1000 |
| RANDOM-04 / trick_h25 / seed0 | 1956 | 237.94 | 284.68 | 0.174 | 1000 |
| RANDOM-04 / trick_h30 / seed0 | 1977 | 248.16 | 303.36 | 0.174 | 1000 |
| RANDOM-04 / trick_h20_r8 / seed0 | 1819 | 257.59 | 308.31 | 0.181 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
