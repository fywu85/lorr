# RANDOM-01 task transitions against Kitty Knight

The current selected PILOT739 and matched KK692 traces both pass independent full replay. The new diagnostic partitions every one of their60,000 robot-steps into approach, loaded work or a censored tail with no subsequent pickup. Independent physical distances reproduce the previously verified table hash.

| Same685 completed tasks | PILOT739 | KK692 |
|---|---:|---:|
| Steps from previous completion/start to pickup | 5922 | 7955 |
| Relaxed physical approach bound | 4978 | 6588 |
| Approach steps above that bound | 944 | 1367 |
| Steps from pickup to completion | 44842 | 45855 |
| Relaxed loaded route bound | 41585 | 41708 |
| Loaded steps above that bound | 3257 | 4147 |
| Remaining-chain penalty of actual pickup heading | 689 | 812 |
| Assignments during pickup transitions | 1139 | 1748 |

PILOT already has shorter task transitions and loaded routes on the common cohort. This does not support treating a broad assignment deficit versus KK as the primary diagnosis. Conditional on actual pickup headings, there is still689 steps of remaining-chain heading penalty on these685 tasks. The approach cost needed to achieve another heading is not included, so689 is not a savings estimate.

A narrower scheduler hypothesis: price the pickup heading independently of the task-length preference. The current selected matcher uses oriented distance to any pickup heading plus0.25 times internal cell hops. The older whole-chain mode scales the entire continuation, including its heading difference, by the task-length coefficient. The new experiment removes each continuation minimum and prices only its heading excess, preserving the original cell-hop task-length term.

Full-trace completed cohorts include739/692 tasks; their remaining-chain pickup-heading penalties are749/819 steps. All completion/approach/loaded counts exclude the censored subset unless explicitly in the complete robot-step partition. No throughput bound or causal gain follows from these observations.

[Diagnostic](audit.json), [full replay](replay.json), [trace manifest](manifest.json), [distance provenance](distance-reference.json).
