# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_previous_control / seed2 | 1798 | 197.56 | 308.43 | 0.085 | 800 |
| RANDOM-03 / trick_window_no_history / seed2 | 1900 | 470.5 | 598.51 | 0.166 | 766 |
| RANDOM-03 / trick_previous_control / seed4 | 1848 | 189.36 | 324.55 | 0.085 | 779 |
| RANDOM-03 / trick_window_no_history / seed4 | 1870 | 463.81 | 633.22 | 0.159 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
