# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / trick_selected_control / seed0 | 621 | 161.98 | 207.96 | 0.059 | 383 |
| RANDOM-01 / trick_cadence1_budget1 / seed0 | 617 | 145.74 | 199.78 | 0.058 | 343 |
| RANDOM-01 / trick_cadence1_budget2 / seed0 | 617 | 145.79 | 193.51 | 0.058 | 343 |
| RANDOM-01 / trick_cadence1_budget4 / seed0 | 617 | 145.15 | 199.98 | 0.058 | 343 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
