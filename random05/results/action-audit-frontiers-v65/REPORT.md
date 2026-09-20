# Full trajectory and task-work audit of the current frontiers

All four full runs pass independent replay checks for movements, vertex/edge conflicts, task assignment uniqueness, waypoint visits and the prohibition on reassigning opened tasks. Action counts are robot-steps; each run contains 1,600,000.

| Solver / allocation | Tasks | Loaded forward | Loaded rotations | Loaded waits | All work on final-unfinished orders | % of robot-steps | Completed loaded detour ratio | Five-stop orders completed |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| ours-four-cores-3770 | 3770 | 466974 | 421149 | 603284 | 202843 | 12.7% | 2.41 | 593 |
| ours-32-workers-3852 | 3852 | 469784 | 416358 | 600825 | 217172 | 13.6% | 2.36 | 624 |
| nms-four-cores | 2914 | 477551 | 561393 | 483779 | 496435 | 31.0% | 2.68 | 346 |
| nms-32-workers | 3172 | 495753 | 543475 | 475108 | 480674 | 30.0% | 2.56 | 390 |

The detour ratio is completed orders' loaded forward moves divided by the sum of obstacle-aware shortest-path distances between their waypoints. It ignores rotations, waiting and congestion; it is a descriptive lower bound, not an achievable throughput estimate.

At matched allocations, our solver completes more five-stop orders and spends fewer robot-steps on orders left unfinished. Its total loaded waiting is higher. These counts do not establish that waiting is the main avoidable cost or that all unfinished work could have been recovered. The compared policies finish different orders, and our configurations include known-horizon triage.

For orders completed by both solvers:

| Matched allocation | Shared completed orders | Our loaded work | NMS loaded work | Difference | Our forward / rotations / wait | NMS forward / rotations / wait |
|---|---:|---:|---:|---:|---|---|
| Four cores | 2889 | 959015 | 1027047 | -6.62% | 307153 / 282597 / 369265 | 332253 / 364280 / 330514 |
| 32 workers | 3126 | 1005904 | 1017570 | -1.15% | 326882 / 293937 / 385085 | 344556 / 351223 / 321791 |

The shared-order comparison controls task IDs, but assignments and congestion histories still differ. These are observational accounting results, not isolated causal estimates. The completed [paired-setting horizon ablation](../staged-no-horizon-split-full-v65/horizon-ablation.json) measures the cutoff contribution separately: +7.6% on four cores and +6.1% on32 workers for these selected configurations.

[Audited counts and raw hashes](audit.json), [exact input/result manifest](manifest.json).
