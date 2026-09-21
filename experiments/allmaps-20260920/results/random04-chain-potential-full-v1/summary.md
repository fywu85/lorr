# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_control / seed0 | 1999 | 190.42 | 241.73 | 0.118 | 1000 |
| RANDOM-04 / trick_chain_mode1 / seed0 | 1971 | 183.42 | 224.22 | 0.160 | 1000 |
| RANDOM-04 / trick_chain_mode2 / seed0 | 1951 | 185.66 | 245.38 | 0.160 | 1000 |
| RANDOM-04 / trick_chain_mode3 / seed0 | 1775 | 179.63 | 233.93 | 0.160 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
