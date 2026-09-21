# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_control / seed0 | 2956 | 590.12 | 789.68 | 0.196 | 2000 |
| RANDOM-05 / trick_chain_mode1 / seed0 | 3027 | 585.78 | 744.3 | 0.239 | 2000 |
| RANDOM-05 / trick_chain_mode2 / seed0 | 2850 | 592.14 | 752.14 | 0.239 | 2000 |
| RANDOM-05 / trick_chain_mode3 / seed0 | 2958 | 592.73 | 714.02 | 0.239 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
