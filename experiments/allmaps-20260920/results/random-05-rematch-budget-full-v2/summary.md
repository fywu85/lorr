# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_budget1_control / seed0 | 2684 | 158.53 | 231.95 | 0.193 | 2000 |
| RANDOM-05 / trick_budget2 / seed0 | 2745 | 158.77 | 223.73 | 0.193 | 2000 |
| RANDOM-05 / trick_budget4 / seed0 | 2723 | 158.87 | 227.79 | 0.193 | 2000 |
| RANDOM-05 / trick_budget4_cadence1 / seed0 | 2746 | 158.65 | 226.47 | 0.194 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
