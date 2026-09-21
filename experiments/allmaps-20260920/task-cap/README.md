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
