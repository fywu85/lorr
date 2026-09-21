# General finite unopened-task retarget budget

The default CGAR matching pass allows each task one reassignment before pickup.
The independent RANDOM05 algorithm and NMS/KK revisit unopened assignments more
freely. This experiment tests whether a finite budget of two or four changes
retains useful pickup opportunities as robots move.

`CGAR_REASSIGN_MATCH_TASK_BUDGET` defaults to1 and accepts1..8. Nondefault budgets
require the existing matching pass, with no matching-horizon guard. The option
is general: no map name, density branch, layout, or new task-selection objective.
Each moved task is counted, including all tasks in a permutation cycle. Its count
is removed only after the task is started or disappears. The20-step robot cooldown,
primary/next-primary, recovery, pocket, commitment and oldest-admission protections
remain. Started tasks cannot be retargeted. No task is dropped to the free pool.

Group size32, fixed group quota, resident-only oriented distances, acceptance
margin and complete-work deadlines stay the same. A timeout is a failed entry.
Default1 must reproduce the existing assignments and actions. Diagnostics record
repeated moves and the largest actual per-task count. This preserves a finite
retarget bound; it is not a new end-to-end liveness theorem.

After regression, compare budgets1/2/4 and4 with cadence1 on selected RANDOM02,
RANDOM04, RANDOM05 and CITY02 profiles. Their existing map-specific fields/ranks
remain explicit tricks; the new budget mechanism itself is general. Full horizons,
strict1s, four bound physical cores,32decimalGB. Repeat useful gains on more seeds.

All16 full runs passed independent finite-budget and started-ownership checks,
strict1s and memory limits. Budget1 controls exactly match their prior trajectories.
Budget1/2/4/4+cadence1 scores: RANDOM02=1160/1160/1160/1188;
RANDOM04=1622/1587/1562/1615; RANDOM05=2684/2745/2723/2746;
CITY02=16169/16315/16103/16029. Keep losses as evidence, not new defaults.
Historical analyzer constants caused failed analysis attempts (not solver failures);
[preserved receipts](preserved-analysis-failures.json). Independent counters now
use declared budgets and actual schedules, with started ownership and cooldown
checks. Legacy Warehouse verification retains default bound one.
