# Independent pickup-weight accounting

All six full trajectories pass the action-accounting identities, handoff/unfinished-task fixtures, source hashes and before/after raw-file hashes: 300 million robot steps in total. The exact control trajectories agree with V92. Completed 2026-09-20 16:36:57 UTC, GRID job 8900263.

| Weight and seed | Completed-task change | Empty-work change | Unfinished-task-work change |
|---|---:|---:|---:|
| 8, seed 0 | +40 | -27,184 | -8,016 |
| 8, seed 2 | +409 | -21,623 | -37,586 |
| 12, seed 0 | -69 | -33,072 | -23,184 |
| 12, seed 2 | +288 | -30,952 | -17,611 |

Weight 12 reduces both measured work categories yet loses throughput on seed 0. Changed cohorts, timing and traffic prevent interpreting these columns as additive causal savings or a score predictor.

[Accounting and raw hashes](accounting.json), [fixtures](fixtures.json), [full interpretation](../native-pickup-full-v97/summary.md).
