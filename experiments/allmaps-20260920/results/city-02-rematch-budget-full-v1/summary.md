# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_budget1_control / seed0 | 16169 | 249.0 | 814.22 | 8.513 | 2564 |
| CITY-02 / trick_budget2 / seed0 | 16315 | 252.22 | 821.72 | 8.509 | 2540 |
| CITY-02 / trick_budget4 / seed0 | 16103 | 252.2 | 833.83 | 8.503 | 2562 |
| CITY-02 / trick_budget4_cadence1 / seed0 | 16029 | 248.45 | 811.23 | 8.493 | 2565 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
