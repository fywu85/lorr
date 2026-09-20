# Independent physical-wait replay (V112)

Replay the already verified best-profile seeds0/2 (V99, pickup8/p90) without
changing the solver. At each physical step, independently apply all actions,
check vertices, edges, obstacles, task ownership, and exact service events.
Classify a wait by current-goal service, obstacle ahead, free forward cell after
all other observed actions, or occupied forward cell. The categories partition
all waits. Report whole-run and exact final1000/400/100-step cohorts.

Future completion labels are post-hoc observations only. They are never supplied
to a planner and cannot establish that an unfinished task was unfinishable.
A free next cell does not establish feasibility of the five-step macro, the
preferred guidance direction, or benefit from moving. Counts are not saved time,
counterfactual completions, or throughput ceilings.

The driver freezes source, reference accounting, work summary and map/header
hashes. It requires one actually bound physical GRID core, no cgroup quota, and
verifies both raw trajectory hashes before and after analysis. Independent
phase and wait totals must exactly match the previous full accounting. Nine
hand fixtures cover repeated-location service, clear/vacated/incoming forward
cells, a stationary blocker, future-outcome labels, and rejection of edge swaps,
vertex collisions and missing service events.

GRID8900658 completed at2026-09-20T20:24:05.317433+00:00 on research44, bound CPU32.
All100million physical actions and all service events validate; all nine fixtures
pass. Binary907b9046b0b24ebdd731737d037e85bd96378b432082a8198f22e97f0f88c302.
[Results](../results/physical-waits-audit-v112/summary.md).

The first two submissions are preserved under
`runs/cgar-physical-waits-audit-v112-20260920` (8900651, absent SGE_BINDING variable)
and `runs/cgar-physical-waits-audit-v112-r2-20260920` (8900653, actual binding not
applied on research38). Both stopped before compiling/replaying. The corrected
launcher checks actual affinity and selects hosts with working binding. Successful
raw artifacts: `runs/cgar-physical-waits-audit-v112-r3-20260920`.

The evidence favors studying traffic interactions over treating at-goal waiting
as a large loss. About92% of waits have an occupied forward cell; about57% have
an incoming moving blocker and17% a rotating blocker. Only265/289 waits serve a
current goal. Completed-task waiters blocked by eventually unfinished tasks
account for15942/15583 observations, only0.032%/0.031% of all robot actions; that
does not justify assuming a large gain from cutoff-aware motion priorities.
