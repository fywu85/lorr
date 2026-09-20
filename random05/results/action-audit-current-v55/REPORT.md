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

## Goal suppression near the horizon

The replay reconstructs our triage condition from current positions, task stages,
remaining grid distances and cumulative steps per forward move. It reproduces
all20 sampled native triage/movement counts in each of our runs, and its totals
match the independently certified action audit.

| Run | Loaded waits with an active goal | Loaded waits with a suppressed goal | Suppressed share of loaded waits |
|---|---:|---:|---:|
| ours-four-3655 | 520097 | 79060 | 13.2% |
| ours-32-3705 | 520986 | 81395 | 13.5% |
| nms-four-2914 | 483779 | 0 | 0.0% |
| nms-32-3172 | 475108 | 0 | 0.0% |

For NMS the table reports raw loaded waits; no equivalent internal suppression
rule is inferred. Our suppression starts after step1400 in these runs. It
accounts for about13% of our loaded waits, so it must be separated from ordinary
traffic delays. Neither category is a causal estimate of lost throughput.

[Suppression audit](suppression-audit.json) includes200-step bins and checks
against the native logs. [Reproduction manifest](suppression-manifest.json).
