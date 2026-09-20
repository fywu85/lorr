# Current RANDOM-05 action audit

The full trajectories of our selected records and matched NMS runs replay
without collisions or task-lock errors. All use the archived 800-robot input
and 2,000 steps. These measurements describe executed actions, not CPU time.

For task IDs completed by both solvers, count loaded actions from the first
waypoint through completion. Negative deltas mean fewer actions for ours.

| Allocation | Common completed tasks | Turns / task, ours minus NMS | Forwards / task | Waits / task | Total loaded steps / task |
|---|---:|---:|---:|---:|---:|
| Four cores: 3655 vs 2914 | 2880 | -24.16 | -6.20 | +16.99 | -13.36 |
| 32 workers: 3705 vs 3172 | 3114 | -13.63 | -2.48 | +24.65 | +8.54 |

Our solver saves turns and forward moves but still waits more on these common
tasks. Four-core savings exceed the extra waits; in the 32-worker pair they do
not. Overall throughput also reflects which orders finish, initial approaches
and work left incomplete. Assignments and congestion histories differ, so this
is not an isolated causal comparison or a recoverable-throughput estimate.

This motivates a general search-model experiment: keep the current first
decision and fixed rollout budget, but construct some future perturbations
using previously evaluated priority vectors. Such continuations may represent
the next optimized policy better than independent random offsets. This is an
untested hypothesis; no past score, action or reservation will be reused.

[Audit and raw hashes](audit.json), [manifest](manifest.json),
[complete trajectory equivalence checks](trajectory-controls.json).
Order release-to-completion latency is tracked separately in
[the waiting report](../task-waiting-frontiers-20260920T1612/REPORT.md).
