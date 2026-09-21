# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_round2_control / seed2 | 1645 | 125.78 | 183.2 | 0.119 | 1000 |
| RANDOM-04 / trick_round4 / seed2 | 1659 | 184.49 | 242.12 | 0.117 | 1000 |
| RANDOM-04 / trick_round2_control / seed4 | 1512 | 102.78 | 136.91 | 0.116 | 1000 |
| RANDOM-04 / trick_round4 / seed4 | 1611 | 182.97 | 245.43 | 0.116 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
