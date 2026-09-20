# Fresh validation V4: secondary waiting metrics

Throughput selects the frozen candidate; waiting time is secondary. Durations are simulation steps from task release to the final waypoint. All three runs within each seed share starts and task definitions.

| Seed / solver | Tasks | Completed mean | Completed p95 | Completed max | Oldest unfinished | Initial unfinished / unopened |
|---|---:|---:|---:|---:|---:|---|
| seed50007-ours | 3680 | 430.7 | 1107 | 1949 | 2000 | 110 / 70 |
| seed50007-nms-repeat1 | 2907 | 411.3 | 1332 | 1997 | 2000 | 215 / 73 |
| seed50007-nms-repeat2 | 2870 | 411.8 | 1292 | 1992 | 2000 | 220 / 86 |
| seed50008-ours | 3641 | 435.5 | 1054 | 1948 | 2000 | 125 / 96 |
| seed50008-nms-repeat1 | 2930 | 419.7 | 1319 | 1991 | 2000 | 222 / 96 |
| seed50008-nms-repeat2 | 2918 | 417.6 | 1307 | 1983 | 2000 | 231 / 97 |

Every run leaves initial orders unfinished at step2,000. Their eventual maximum latency is unknown and at least2,000; completed-only statistics are censored. The candidate uses the declared horizon cutoff. Later release times depend on throughput, so initial-cohort counts accompany the completed statistics.

[Seed50007 audit](waiting-seed50007-audit.json), [seed50008 audit](waiting-seed50008-audit.json), [throughput comparison](audit.json).
