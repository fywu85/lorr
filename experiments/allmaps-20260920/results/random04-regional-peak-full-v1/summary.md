# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_temperature1000_peak0 / seed0 | 1947 | 288.39 | 364.0 | 0.120 | 1000 |
| RANDOM-04 / trick_temperature1000_peak1 / seed0 | 1589 | 181.75 | 239.26 | 0.116 | 1000 |
| RANDOM-04 / trick_temperature5000_peak0 / seed0 | 1924 | 183.92 | 256.04 | 0.117 | 1000 |
| RANDOM-04 / trick_temperature5000_peak1 / seed0 | 1999 | 187.2 | 238.42 | 0.117 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
