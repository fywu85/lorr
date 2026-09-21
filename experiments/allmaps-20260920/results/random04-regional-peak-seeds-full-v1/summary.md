# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_temperature1000_peak0 / seed2 | 1904 | 187.83 | 246.18 | 0.118 | 1000 |
| RANDOM-04 / trick_temperature1000_peak1 / seed2 | 1869 | 189.07 | 239.79 | 0.117 | 1000 |
| RANDOM-04 / trick_temperature5000_peak0 / seed2 | 1949 | 186.69 | 253.34 | 0.117 | 1000 |
| RANDOM-04 / trick_temperature5000_peak1 / seed2 | 1938 | 204.98 | 253.43 | 0.117 | 1000 |
| RANDOM-04 / trick_temperature1000_peak0 / seed4 | 1796 | 183.96 | 236.25 | 0.117 | 1000 |
| RANDOM-04 / trick_temperature1000_peak1 / seed4 | 1721 | 183.28 | 226.38 | 0.117 | 1000 |
| RANDOM-04 / trick_temperature5000_peak0 / seed4 | 1987 | 185.57 | 257.28 | 0.117 | 1000 |
| RANDOM-04 / trick_temperature5000_peak1 / seed4 | 1876 | 185.03 | 243.89 | 0.118 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
