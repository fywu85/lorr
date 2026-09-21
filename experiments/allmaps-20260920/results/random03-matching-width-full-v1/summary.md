# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_current_control / seed2 | 1982 | 462.27 | 597.18 | 0.165 | 748 |
| RANDOM-03 / trick_width64 / seed2 | 1938 | 460.14 | 622.92 | 0.166 | 759 |
| RANDOM-03 / trick_width128 / seed2 | 1938 | 459.46 | 633.79 | 0.167 | 759 |
| RANDOM-03 / trick_width256 / seed2 | 1938 | 444.85 | 598.71 | 0.165 | 759 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
