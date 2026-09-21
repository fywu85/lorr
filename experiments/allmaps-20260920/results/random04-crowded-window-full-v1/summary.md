# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_root_control / seed0 | 1999 | 187.72 | 241.52 | 0.118 | 1000 |
| RANDOM-04 / trick_window10_keep0 / seed0 | 1913 | 206.67 | 260.26 | 0.178 | 1000 |
| RANDOM-04 / trick_window20_keep0 / seed0 | 1734 | 215.59 | 277.24 | 0.196 | 1000 |
| RANDOM-04 / trick_window20_keep10 / seed0 | 1944 | 234.17 | 278.26 | 0.199 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
