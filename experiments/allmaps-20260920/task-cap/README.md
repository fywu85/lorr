# Explicit RANDOM task admission cap

The crowded maps spend25–38% of actions waiting, and scoring/forecast refinements
so far have not closed the gap. A fresh immutable read of PILOT source
[d51bdbf2](https://github.com/fywu85/lorr/commit/d51bdbf2c4e6830f8cabacb52ac9cfa8234aa663)
shows a separate active-task admission policy. This experiment transfers the
bounded-admission idea into CGAR's own scheduler. No independent solver build,
job, output or held-out task stream was read or changed.

CGAR_TRICK_RANDOM_TASK_CAP requires --trick RANDOM-01..05. A positive cap limits
simultaneously assigned tasks;0 leaves the old policy unchanged. New assignments
use CGAR's existing candidate discovery/ranking and oldest-admission rules within
the available slots. Completion reopens a slot for another task. The receiving
robot set can change; goal-less robots remain available for CGAR displacement.
This does not park extra robots. Existing capacity parking on RANDOM05 is a
separate unchanged mechanism. Held and started tasks are never dropped, even
when an externally supplied initial assignment is already above the cap.

Fairness is secondary and longer task waiting is an explicit possible cost of
this trick. No new stationary obstacle, future-task information, time-based
quality cutoff or incomplete returned search is introduced. Complete source,
fixed work and strict entry budgets remain required. The reference has additional
unopened-task/full-pool matching and optional admission pricing; those are not
part of this bounded first transfer.

Regression checks complete assignment/action identity for absent/zero/full-fleet
limits, dynamic replenishment after completion, valid joint execution, real idle
robot displacement by a held primary, preservation of held/started tasks above
the cap, exact CLI scope and malformed limits. The independent analyzer replays
all actual assignment events, checks the cap at every dispatch, rejects dropped
held tasks, and reconciles admitted-task and peak-active counters. A hand fixture
accepts a valid ownership transfer/completion and rejects both an illegal drop
and an over-cap assignment.

Declared full matrices: RANDOM04 seed0 control2059 versus caps600/650/675;
RANDOM05 seed10 control3065 versus caps600/700/750. Other selected settings stay
fixed, including original scheduler pricing and temporal score. No paid-chain,
new exact assignment, or window experiment is composed. Full1000/2000steps,
strict1000ms,4 physical cores/case,32decimalGB and node-local logs. No throughput
claim before full verification; repeat any positive signal on additional seeds.

RANDOM04 full results: control2059; caps600/650/675=1941/1758/1911.
All four valid under1s, independent admission audits pass and control repeats
its full selected trajectory. No RANDOM04 promotion.

RANDOM05 seed10 full results: control3065, cap600=3032, **cap700=3130**,
cap750=2985. All four valid; the control repeats its full prior trajectory.
The selected-seed improvement is65tasks(+2.12%), with585.66ms mean,695.18ms maximum,
239,919,104-byte peak RSS. The independent2000-dispatch audit confirms at most700
active assignments, all held tasks preserved, and3776distinct admitted tasks.
Existing31-robot capacity parking remains unchanged. Outstanding-task agep90 is
still2000; this is not starvation-free. Cap700 is promoted as an explicit trick,
with exact profile inbest-random05.json. Seeds0and2 are declared matched replications
against the identical uncapped profile, not selected on their outcomes.

A bounded follow-up is declared before outcomes: selected seed10 cap700 control
versus caps625/650/675. All other settings and full-run requirements remain fixed.
The cap700 seed0/2 replication runs independently; this is a parameter search on
the development seed, not a held-out generalization claim. In the first matrix,
cap700 reduces observed pickup delay71.85->42.23steps and pre-pickup forward
work89,352->48,831 actions, but total forward share also falls28.82%->27.90%.
That does not establish that the throughput gain comes from less congestion.

Cap700 replication: seeds0/2/10 control3043/3015/3065 versus3033/3063/3130;
aggregate+1.129%, one loss. Nearby625/650/675 caps yield3042/3084/3106 versus
3130control on seed10. All valid. This cap-only frontier is superseded by3571
with motion commitments and no new task cap; its composition test is pending.
