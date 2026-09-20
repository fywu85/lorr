# Where the current best spends robot actions

The verified p90 / pickup-weight-8 runs use all 50 million robot actions per seed. Every robot completes at least one task, and there are no actions in the goal-less phase. The tables below summarize the already independently checked trajectories; they do not classify all movement as avoidable overhead.

| Physical action / phase | Seed 0 (155,120 tasks) | Seed 2 (155,056 tasks) |
|---|---:|---:|
| Forward moves | 46,982,662 (93.965%) | 46,972,700 (93.945%) |
| Turns | 1,513,667 (3.027%) | 1,514,501 (3.029%) |
| Waits | 1,503,671 (3.007%) | 1,512,799 (3.026%) |
| Before first pickup | 9,314,909 (18.630%) | 9,322,172 (18.644%) |
| After first pickup | 40,685,091 (81.370%) | 40,677,828 (81.356%) |

The action rows partition the total, and the pickup-phase rows partition the same total in a different way. Do not sum across both partitions. Work on unfinished tasks is another overlapping view: 1,070,030 (2.140%) / 1,075,648 (2.151%). The remaining work belongs to tasks completed by the horizon, including work contributed by an earlier holder.

This gives three priorities for diagnosis. First, the 18.6% before-pickup share is much larger than the waiting share, so joint assignment and route choice remain relevant; some repositioning is unavoidable. Second, waiting is about 3% of actions, so a claim of a much larger gain from eliminating waits alone would need evidence of secondary effects. Third, the current neutral-tail experiment changes the score around an actual goal visit, where a local objective can misprice continued movement; its full throughput comparison is still pending. These are directions for measurement, not predicted gains or upper bounds.

The temporal planner replaces the offered-action vector with its final plan before recording movement diagnostics. Consequently, its zero blocked-forward and safety-cancel counters do **not** establish congestion-free operation, and the planned-wait count does not identify the reason for waiting. No cause-of-delay claim is made from those counters.

All input hashes, phase conservation, action conservation and finished-plus-unfinished work identities pass. Source `27be6e312fdd79ad310583d33eeb2a4781d75b97`, binary `37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752`. Explicit `--trick WAREHOUSE`, ordinary fairness, same fixed input. [Machine-readable accounting](work-budget.json), [summarizer](../../native_work/work_budget.py), [full throughput result](summary.md).
