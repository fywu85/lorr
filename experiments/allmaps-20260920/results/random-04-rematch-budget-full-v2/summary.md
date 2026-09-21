# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_budget1_control / seed0 | 1622 | 104.34 | 144.53 | 0.116 | 1000 |
| RANDOM-04 / trick_budget2 / seed0 | 1587 | 104.0 | 139.64 | 0.117 | 1000 |
| RANDOM-04 / trick_budget4 / seed0 | 1562 | 102.89 | 149.62 | 0.116 | 1000 |
| RANDOM-04 / trick_budget4_cadence1 / seed0 | 1615 | 104.17 | 140.67 | 0.116 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
