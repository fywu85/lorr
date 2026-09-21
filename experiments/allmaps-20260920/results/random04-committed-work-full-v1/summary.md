# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_current_control / seed4 | 2321 | 265.44 | 307.75 | 0.174 | 1000 |
| RANDOM-04 / trick_workers16 / seed4 | 2278 | 275.22 | 322.75 | 0.175 | 1000 |
| RANDOM-04 / trick_rounds8 / seed4 | 2358 | 465.07 | 543.68 | 0.174 | 1000 |
| RANDOM-04 / trick_workers16_rounds8 / seed4 | 2398 | 475.42 | 548.1 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
