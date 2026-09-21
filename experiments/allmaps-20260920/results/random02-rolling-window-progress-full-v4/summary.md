# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_roll2048_control / seed4 | 1060 | 305.17 | 382.57 | 0.137 | 509 |
| RANDOM-02 / trick_roll2048_ties / seed4 | 1073 | 331.12 | 398.18 | 0.139 | 492 |
| RANDOM-02 / trick_current2048_ties / seed4 | 1144 | 282.16 | 367.0 | 0.141 | 496 |
| RANDOM-02 / trick_current512_ties / seed4 | 844 | 188.31 | 240.6 | 0.139 | 578 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
