# Controlled CGAR factor matrix

Archived inputs, one planner seed, frozen predeclared factor profiles. Throughput comparisons require full horizons. Shared-host5s development,32decimalGB. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

| Instance / variant | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| SORTATION / trick_sortation_control | 146460 | 482.24 | 1035.9 | 12.371 | 671 |
| SORTATION / trick_sortation_native_bands0 | 149321 | 499.92 | 1126.08 | 12.251 | 665 |
| SORTATION / trick_sortation_native_bands1 | 150353 | 524.64 | 1151.94 | 12.473 | 660 |

Throughput is primary; fairness is a secondary reported metric. RANDOM-05 was excluded.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
