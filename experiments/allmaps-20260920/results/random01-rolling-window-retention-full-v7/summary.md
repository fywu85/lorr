# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_retained_control / seed2 | 662 | 341.46 | 444.47 | 0.135 | 372 |
| RANDOM-01 / trick_keep0 / seed2 | 662 | 342.21 | 450.75 | 0.135 | 372 |
| RANDOM-01 / trick_keep10 / seed2 | 662 | 327.73 | 423.47 | 0.135 | 372 |
| RANDOM-01 / trick_keep19 / seed2 | 657 | 339.4 | 462.33 | 0.136 | 400 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
