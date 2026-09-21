# Fresh V5 order latency

Every row is selected by the frozen throughput protocol. Durations are simulation steps from task release; unfinished orders are censored at the horizon.

| Case | Tasks | Completed mean | p95 | Completed max | Oldest unfinished | Initial unfinished / unopened |
|---|---:|---:|---:|---:|---:|---|
| seed50009-ours | 3912 | 422.5 | 1067 | 1961 | 2000 | 101 / 71 |
| seed50009-baseline | 3901 | 423.5 | 1067 | 1960 | 2000 | 103 / 66 |
| seed50009-nms-repeat1 | 3095 | 399.3 | 1246 | 1994 | 2000 | 199 / 79 |
| seed50009-nms-repeat2 | 3090 | 400.4 | 1262 | 1998 | 2000 | 204 / 81 |
| seed50010-ours | 4047 | 413.5 | 1029 | 1980 | 2000 | 104 / 72 |
| seed50010-baseline | 4023 | 410.4 | 1020 | 1973 | 2000 | 106 / 74 |
| seed50010-nms-repeat1 | 3177 | 399.0 | 1273 | 1981 | 2000 | 178 / 82 |
| seed50010-nms-repeat2 | 3190 | 399.8 | 1267 | 1999 | 2000 | 182 / 77 |

All eight runs still have initial orders unfinished at step 2,000. Their eventual longest waiting time is unknown and at least 2,000 steps. Completed-only statistics are conditional on each solver’s completed task set. Throughput remains the primary objective.

[Full event and waiting audit](replay-audit.json).
