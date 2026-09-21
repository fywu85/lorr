# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_retained_control / seed2 | 1215 | 291.7 | 372.19 | 0.137 | 440 |
| RANDOM-02 / trick_keep0 / seed2 | 1215 | 278.87 | 356.77 | 0.137 | 440 |
| RANDOM-02 / trick_keep6 / seed2 | 1215 | 290.66 | 368.86 | 0.141 | 440 |
| RANDOM-02 / trick_keep19 / seed2 | 1197 | 296.12 | 378.79 | 0.142 | 452 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
