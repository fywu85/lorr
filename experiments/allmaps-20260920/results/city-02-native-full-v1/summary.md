# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_control / seed0 | 14068 | 279.76 | 732.61 | 7.783 | 1330 |
| CITY-02 / trick_adapted_grid / seed0 | 14851 | 246.77 | 807.04 | 8.550 | 1263 |
| CITY-02 / trick_native_grid / seed0 | 14185 | 313.08 | 849.8 | 8.532 | 1308 |
| CITY-02 / trick_native_short / seed0 | 14265 | 333.7 | 846.37 | 8.524 | 2662 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
