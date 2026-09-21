# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_control / seed2 | 2900 | 591.63 | 750.68 | 0.196 | 2000 |
| RANDOM-05 / trick_chain_mode1 / seed2 | 3010 | 589.16 | 869.28 | 0.239 | 2000 |
| RANDOM-05 / trick_control / seed4 | 2889 | 590.1 | 764.32 | 0.196 | 2000 |
| RANDOM-05 / trick_chain_mode1 / seed4 | 3022 | 589.11 | 733.93 | 0.239 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
