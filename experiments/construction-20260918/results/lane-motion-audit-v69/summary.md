# TRICK: saved-run movement and task-cohort audit

All eight V65 full trajectories were replayed on a bound GRID core. The original five hand fixtures, an independent lane/phase/window fixture and a deliberately wrong field all behave as expected. Every real run conserves50million robot steps, has no simulator errors/timeouts, and agrees with the independently verified task totals and empty-work counts. The exact static4/16field hash and input/source/binary hashes are retained in[accounting.json](accounting.json). This is analysis of existing runs, not a new benchmark.

| Policy | Seed | Tasks | Empty steps / task | Completed chain mean | Loaded overhead / task | Loaded turns / task | Loaded waits / task |
|---|---:|---:|---:|---:|---:|---:|---:|
| Lanes | 0 | 146,659 | 67.180 | 242.244 | 21.924 | 5.943 | 11.172 |
| Lanes | 2 | 146,566 | 67.299 | 242.187 | 22.035 | 5.968 | 11.254 |
| Lanes + matching64 | 0 | 147,328 | 65.653 | 242.252 | 21.891 | 5.929 | 11.185 |
| Lanes + matching64 | 2 | 147,204 | 65.741 | 242.265 | 22.025 | 5.935 | 11.284 |
| Lanes + short | 0 | 137,985 | 94.809 | 234.624 | 23.586 | 6.465 | 11.452 |
| Lanes + short | 2 | 137,934 | 95.025 | 234.644 | 23.610 | 6.474 | 11.443 |
| Lanes + short + matching64 | 0 | 142,922 | 82.235 | 235.150 | 23.363 | 6.360 | 11.589 |
| Lanes + short + matching64 | 2 | 142,961 | 82.088 | 235.061 | 23.427 | 6.368 | 11.611 |

Without short preference, matching cuts empty work per completion by1.527/1.558steps. Completed chain length changes only+0.008/+0.078cells; loaded overhead changes−0.033/−0.010steps. The observed gain is therefore consistent with better pickup allocation. This is a comparison of endogenous completed cohorts, not a task-matched causal experiment.

Short preference plus matching serves shorter completed chains (~235.1cells against242.25) but still spends82.1–82.2empty steps per completion against65.7. Its lower overall throughput motivates the separate pickup-emphasis factorial; it does not establish that unlimited rematching would recover the gap.

Against-lane moves are already uncommon: with ordinary fairness, empty0.519–0.530% and loaded0.102–0.103% of forward actions. The remaining-potential experiment can still change decisions and interactions; an unchanged aggregate share would not imply an inert score. Direct same-state macro-ranking tests verify activation.

The archived NMS diagnostic serves154,981tasks with completed chain mean234.794, loaded overhead16.755steps, empty62.021steps per completion, turns5.802and waits7.920. Our best seed has chain242.252, overhead21.891, empty65.653, turns5.929and waits11.185. These are descriptive clues toward task selection, pickup allocation and waiting; different policies, task prefixes/cohorts, resources and one NMS timeout prevent a controlled performance or causal savings claim. The NMS diagnostic is not a qualifying replacement for the user target154,795.

[Machine-readable cohort metrics](metrics.json) · [Full benchmark evidence](../trick-match-full-v65/summary.md).
