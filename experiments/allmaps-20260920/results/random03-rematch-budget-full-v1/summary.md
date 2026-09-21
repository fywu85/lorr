# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_budget1_control / seed0 | 1902 | 187.46 | 310.69 | 0.085 | 800 |
| RANDOM-03 / trick_budget2 / seed0 | 1776 | 181.65 | 310.44 | 0.085 | 800 |
| RANDOM-03 / trick_budget4 / seed0 | 1804 | 182.16 | 296.15 | 0.086 | 800 |
| RANDOM-03 / trick_budget4_cadence1 / seed0 | 1824 | 193.4 | 301.86 | 0.085 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
