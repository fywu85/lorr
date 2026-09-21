# RANDOM-04: what task admission changed

These are full, independently replayed archived-input runs. The admission cap is
an explicit `--trick RANDOM-04`; opened orders stay locked and all 700 robots
remain movable. The uncapped and capped 1.25-cutoff recipes differ in admission,
with newer exact cache/matching controls checked separately.

| Planner seed | Uncapped | Cap 560, cutoff 1.25 | Cap 560, cutoff 1 |
|---|---:|---:|---:|
| 0 | 2542 | 2701 | 2718 |
| 3 | 2576 | 2654 | 2689 |
| 4 | 2661 | 2698 | 2738 |
| 5 | 2605 | 2705 | 2735 |

Admission improves all four seeds at the matched 1.25 cutoff (+3.60% aggregate).
Lowering the cutoff to 1 improves each again. These are planner seeds on one
archived task/start input; they are not independent task streams or layouts.

| Configuration | Tasks by step 100 | By 200 | By 500 | By 1000 |
|---|---:|---:|---:|---:|
| Uncapped, cutoff 1.25 | 167 | 448 | 1264 | 2661 |
| Cap 560, cutoff 1.25 | 222 | 570 | 1385 | 2698 |
| Cap 560, cutoff 1 | 222 | 570 | 1385 | 2738 |
| Cap 560, cutoff 0.875 | 222 | 570 | 1385 | 2741 |
| Matched NMS | 309 | 638 | 1379 | 2580 |

The cap's gain appears early; cutoff changes affect the later phase. In the
first 100 steps, the uncapped solver makes 29,903 forward moves, the cap makes
29,081, and NMS makes 25,564. More motion does not imply more completed orders.
The capped solver initially assigns 560 tasks with 348 total unweighted approach
hops and 17,675 internal chain hops, versus 700 / 697 / 26,774 without the cap.
All three initially select the same count of two-stop tasks (271); NMS's early
lead is therefore not explained by selecting more two-stop tasks at time zero.
Different trajectories subsequently reveal tasks at different times, so these
counts are diagnostic observations, not a causal estimate of recoverable work.

The gain has a fairness cost: on seed 4, initial orders still unfinished at the
end increase from 135 without the cap to 181 at cutoff 1.25, 182 at cutoff 1,
and 178 at cutoff 0.875. Oldest unfinished orders are censored at 1,000 steps.
Throughput remains the primary objective; these counts stay in the audit logs.

[Phase counts and action totals](phases.json), [task mix and input trace hashes](task-mix.json).
The authoritative current record and its exact configuration are in
[the PILOT dashboard](../../../PILOT_PROGRESS.md).

For the 2,524 task IDs completed by both the capped 2,762-task solver and NMS,
PILOT uses 189,073 loaded forward moves, 115,526 waits, and 113,441 turns; NMS
uses 176,133 forwards, 92,822 waits, and 141,010 turns. PILOT spends about 2.0%
more loaded robot steps on this shared completed set. Assignments and congestion
histories still differ, so this is not an isolated planner-speed comparison.
It motivates testing detour costs and avoidable waits after the admission change.
[Common-task replay and hashes](common-tasks.json).
