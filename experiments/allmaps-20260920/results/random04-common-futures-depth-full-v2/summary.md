# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_h15_control / seed0 | 2023 | 216.3 | 276.96 | 0.174 | 1000 |
| RANDOM-04 / trick_h15_b4 / seed0 | 2002 | 233.23 | 287.79 | 0.174 | 1000 |
| RANDOM-04 / trick_h20_b2 / seed0 | 2059 | 224.22 | 283.86 | 0.174 | 1000 |
| RANDOM-04 / trick_h15_chain_score / seed0 | 1899 | 211.05 | 256.33 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
