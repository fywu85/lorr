# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_turn20_control / seed0 | 1902 | 191.16 | 301.73 | 0.085 | 800 |
| RANDOM-03 / trick_kk_turn6 / seed0 | 1779 | 181.64 | 322.17 | 0.084 | 800 |
| RANDOM-03 / trick_kk_turn12 / seed0 | 1774 | 187.77 | 313.11 | 0.084 | 800 |
| RANDOM-03 / trick_kk_turn32 / seed0 | 1674 | 180.63 | 319.58 | 0.084 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
