# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_adapted_control / seed0 | 14851 | 250.47 | 799.86 | 8.551 | 1263 |
| CITY-02 / trick_adapted_dispatch / seed0 | 15797 | 249.4 | 809.78 | 8.504 | 2623 |
| CITY-02 / trick_adapted_short / seed0 | 14459 | 244.38 | 817.85 | 8.494 | 2672 |
| CITY-02 / trick_adapted_dispatch_short / seed0 | 15604 | 246.87 | 819.25 | 8.870 | 2641 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
