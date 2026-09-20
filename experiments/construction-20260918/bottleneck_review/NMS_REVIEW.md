# NMS follow-up against current CGAR

2026-09-20 UTC. This pass follows the active `Solution` tree in archive
`44950d1b61b7482b4529a9dc127fa9d220962bdc`, its compiled entry and enabled settings.
NMS has already contributed substantially to CGAR. Its five-action operations,
recursive displacement and randomized repair are in the current temporal kernel.
The earlier [native conformance study](../PROBE.md) matched all 51,600 geometry
cases and 15,360 selected robot operations across construction/repair fixtures.
Those tests and old fixed-goal scores precede today's much stronger CGAR policy;
they must not be presented as a current throughput gap.

The user subsequently authorized a separate map-specific **trick** track if
closing the generic gap remains difficult. Any implementation must require
`--trick <map-instance-name>` and use `[trick]` in commit titles and `TRICK` in
the log. The guidance exclusions below apply to the **generic track**; NMS lane
rules and KK weights may now be tested separately under that explicit flag.
[Track policy](../EXPERIMENT_TRACKS.md). No trick has been implemented yet.

## What the active code actually does

| Component | NMS implementation | Current CGAR implication |
|---|---|---|
| Operations and repair | 129 five-slot patterns; initial depth 3, repair depth 3–5; one distinct blocking owner; randomized displacement and annealing | Already transferred. The active solve calls `try_build`, not its unused `try_echo_slam` multi-owner routine. There is no missing active NMS multi-owner algorithm to copy. |
| Search portfolio | 32 independent worker copies, select the best final internal score; clock-limited work | CGAR has fixed-work workers and prescribed repair. More workers were already mixed across six seeds. This is not the highest-priority untested transfer. |
| Motion weights | Warehouse sets every robot's power to 1; other map families use remaining-chain ranks | Equal power is already our reference. Copying warehouse order or map-specific priority switches adds no general principle. |
| Task candidate coverage | For selected eligible robots, refresh a sorted list over every currently eligible task; refresh oldest lists first | Wider discovery is distinct from increasing CGAR's already-complete pickup-field quota. Measure missed useful fresh tasks and stale candidates before expanding coverage. |
| Assignment | Release all unopened tasks, preserve started tasks, greedily merge per-robot lists using a global heap; weighted pickup plus weighted whole chain | Broader rematching and metric consistency remain useful hypotheses. The lists can be stale, so this is not a globally optimal matching or a fully refreshed matrix each tick. Scheduler LNS is configured to zero time. |
| Assignment price | Warehouse uses `5 * pickup + chain`, with no task-age term in that cost; each inter-errand leg assumes orientation 0 | Larger pickup weights and cost-only variants have already failed in CGAR. Do not copy the factor 5 or assume the chain metric exactly models the robot's arrival orientation. Test coverage/rematching separately from richer chain prices. |
| Guidance | Active warehouse path calls `set_warehouse`: alternating lanes, an eight-column direction mask, hard-coded coordinate bands, forward weights 20/200 | This is map-specific guidance, not a generic online congestion estimator. It falls outside our constraint. Generic, consistent guidance remains a separate research direction; NMS does not supply a ready generic warehouse updater. |
| Heuristic coverage | Precompute weighted oriented distances to every free destination; precompute operation geometry | CGAR uses bounded caches and already shares candidate preparation. Reproduce useful coverage within 32 GB rather than importing the entire NMS memory footprint. |

Source anchors: `Solution/settings.hpp:20–75`, `Planner/eplanner.cpp:24–78`,
`Planner/PIBT/pibts.cpp:65–91,363–435,676–725,920–1008`,
`Objects/Environment/operations.cpp:70`, `environment.cpp:151–180`,
`graph_guidance.cpp:69–99,231–253`, `heuristic_matrix.cpp:61–84`,
`Scheduler/scheduler_solver.cpp:13–45,53–82,182–225,254–284,293–395`,
and `Scheduler/scheduler.cpp:42–49`. Paths are relative to NMS's `Solution`.
[Inspected file hashes and accounting](../results/nms-task-pool-audit/accounting.json).
The unused guidance-path planner, dynamic heuristic rebuilding and scheduler LNS
must not be credited for the active snapshot's results. The enabled fleet-cap
code permits all 10,000 robots in this warehouse case; it is not an explanation
for this comparison and is not a proposed transfer.

## Full trajectory evidence

The saved NMS run completed **154,981** tasks, with **one entry timeout** and
**39.744 GB RSS**. It is useful diagnostic evidence but does not meet our
one-second/32-billion-byte contract. Its original raw trajectory hash matches
the independent historical replay. The new replay accounts for all 50 million
robot steps, passes five hand-counted action fixtures plus a task-pool fixture,
and preserves every earlier metric except the two explicitly corrected below.

Robot steps per completed task:

| Component | Current CGAR, pooled six seeds | KK, one run | NMS, one diagnostic run |
|---|---:|---:|---:|
| Shortest chain of completed tasks | 241.861 | 233.836 | 234.794 |
| Extra loaded forward travel | 7.686 | 4.594 | 3.032 |
| Loaded turns | 9.747 | 5.678 | 5.802 |
| Loaded waits | 13.552 | 8.411 | 7.920 |
| All empty work | 64.295 | 65.691 | 62.021 |
| Unfinished loaded work | 10.223 | 8.628 | 8.672 |
| Idle | 0 | 0 | 0.379 |

NMS's completed loaded detour/turn/wait overhead totals **16.755**, versus about
**30.985** for current CGAR. This is a behavioral difference, not proof that
14.23 steps per completion can be recovered by changing the kernel. The active
kernels are close, while guidance, task cohorts, work budgets and memory differ.

NMS also leaves a long-task backlog: at step 5,000 its 5,029 unassigned tasks
average **506.89** cells and **2,703.02** steps since reveal. **1,562** tasks remain
unpicked after at least 4,000 steps; **4,933** after at least 1,000. CGAR has no
unpicked task aged 1,000 at the six sampled checkpoints. This reinforces the
need to report assignment delay alongside throughput. It does not prove which
part of NMS's advantage is caused by rematching or task preference.

The first replay attempt (GRID 8899254) correctly failed the complete-phase
identity. NMS's initial timeout occurs before TaskManager emits its first accepted
schedule. The older accounting omitted those 10,000 implicit unassigned steps
and included them in the loaded residual. The corrected replay (8899256) retains
the timeout markers and changes idle **48,814 → 58,814**, total loaded
**40,339,131 → 40,329,131**. All other historical fields reproduce exactly.
The original report is retained. Valid CGAR/KK schedules starting at step 0/1
do not need this correction. [Replay evidence](../results/nms-task-pool-audit/cohorts.json),
[reproduction](nms_account.py), [general replay](pool_audit.py).

## Next experiments

1. Finish the current same-state complete small-group probe. NMS's shared
   one-owner restriction makes it especially important to measure feasible joint
   replacements rather than infer a search limitation from blocker counts.
2. Audit useful fresh-task omissions and old/new cost-quote coverage before
   implementing bounded task-centered unopened rematching. Preserve started,
   primary, recovery and finite-retarget/fair-admission protection. Keep ordinary
   assignment and rematching as separate factors; report all empty work and age tails.
3. Use the pending KK guidance/rematching factorial to prioritize a generic
   guidance change versus assignment. NMS's map-specific lanes are diagnostic
   context, not a candidate CGAR implementation. Do not repeat the failed intended-route
   guide or treat larger cache quotas as a new coverage experiment.

No new throughput record or production-policy promotion is claimed by this pass.
