# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_current_control / seed4 | 2321 | 261.59 | 297.33 | 0.174 | 1000 |
| RANDOM-04 / trick_reference_strength25 / seed4 | 1680 | 235.63 | 303.39 | 0.174 | 1000 |
| RANDOM-04 / trick_reference_strength50 / seed4 | 2111 | 246.9 | 295.15 | 0.174 | 1000 |
| RANDOM-04 / trick_reference_strength75 / seed4 | 2063 | 249.42 | 307.4 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
