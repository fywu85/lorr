# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_control / seed0 | 1902 | 189.41 | 304.37 | 0.084 | 800 |
| RANDOM-03 / trick_chain_mode1 / seed0 | 1491 | 174.11 | 329.59 | 0.127 | 800 |
| RANDOM-03 / trick_chain_mode2 / seed0 | 1677 | 178.19 | 320.66 | 0.128 | 800 |
| RANDOM-03 / trick_chain_mode3 / seed0 | 1203 | 163.74 | 316.96 | 0.127 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
