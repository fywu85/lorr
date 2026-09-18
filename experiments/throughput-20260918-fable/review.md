The WAREHOUSE and SORTATION loss is a task-dispatch defect, and it looks recoverable without touching the persistent primary or the validated witnesses. I agree with the earlier README on that.

I disagree on three points: the steady-state mechanism, the reading of the hybrid regressions, and the leader code cited by the earlier independent review. The logs also show a recovery weakness on GAME that neither report mentions.

I ran nothing. Every figure comes from the committed records or from summary lines in the local logs under `runs/`.

## What the experiments establish

**Supported causally, as single runs**

- **The scheduler drives the SORTATION loss, not the motion fixes.** One binary with one flag changed gives a threefold difference, and the `draft_scheduler` control matches the archived draft prefix. WAREHOUSE has no such control, so the claim there is an inference.
- **The fallback policy matters.** The `sample` variant differs by one flag and gains on both large maps. That flag also shortened local search, which works against the treatment, so the sign is robust.

**Not established**

- **Budget exhaustion is not the steady cause.** Deadline misses occur only in the first call, and every later fallback is a node-limit miss. Misses grow by exactly twice the fallback count in all four large-map runs, because the second search repeats the first and cannot succeed.
- **CITY-01 says nothing about sampling.** No fallback occurred there, so the sampling code never ran. The small CITY-01 gain in the README is a timing-noise estimate.
- **RANDOM-01 is a one-seed result.** RANDOM-01 reproduced exactly, so its hybrid loss is a real policy effect for that seed.
- **Pickup distance is not the only mechanism.** Sampling chose short chains at step 0, and later assignments inherited the long ones.
- **Sustained gains are unknown.** In the archived full runs the draft decays every window while the fixed build rises. One scheduling call dominates the thousand-step window.

| Steps 1 to 800, from `results/*.json` | SORTATION fixed | SORTATION sample | WAREHOUSE fixed | WAREHOUSE sample |
|---|---:|---:|---:|---:|
| Assignments made by fallback | 53% | 52% | 35% | 36% |
| Deadline misses | 0 | 0 | 0 | 0 |
| Mean chain estimate at step 0 | 232 | 168 | 231 | 171 |
| Mean chain estimate afterwards | 204 | 280 | 238 | 308 |
| Mean pickup estimate afterwards | 139 | 95 | 99 | 60 |

| Confounder | Observed |
|---|---|
| Same code path, SORTATION step 0, robots matched locally | 639 vs 275 |
| Same code path, CITY-01 tasks, zero fallbacks in both | 2,093 vs 2,108 |
| Step-0 assignments made by sampling under `sample` | 95% SORTATION, 93% WAREHOUSE |
| CITY-01 later pickup estimate, sparse vs dense | 20.2 vs 32.6, chains equal near 495 |

The first two rows measure the wall-clock confounder directly. Table building is time-bounded, so identical configurations diverge.

The dense `hybrid` branch lengthened CITY-01 pickups while chains stayed equal. An age-over-cost ratio does that once every task is a candidate. Consider a robot freed at step 500:

- Fresh task: revealed 5 steps ago, 5 cells away, chain 233. Ratio 1.02.
- Stale task: revealed at step 0, 150 cells away, chain 300. Ratio 2.11.

The distant task wins, and the sixteen-nearest cap was hiding this. I would not fix the two probe cases by widening candidates under the current score, because the probe defines its optimum by that same score.

A second candidate cause is that the dense branch ranks Manhattan and exact distances together, which favours tasks whose distance tables are missing. Both explanations remain hypotheses. The runner already defines variants with the ratio disabled, but no result was recorded.

## Defects and weaknesses

In the table below, `cgar.cpp` is `cgar/cgar_planner/cgar.cpp` and `probe-v2.patch` is in `experiments/throughput-20260917/`.

| Finding | Location | Severity | Consequence |
|---|---|---|---|
| Fixed search radius, then oldest task | `cgar.cpp:1194`, `:1231-1235` | High | Half of later SORTATION assignments ignore distance |
| Second search repeats a failed search | `cgar.cpp:1233` | Medium | About four fifths of later SORTATION search work finds nothing |
| Fixed one-third budget, half spent on chain tables before matching | `cgar/src/TaskScheduler.cpp:36`, `cgar.cpp:1114-1122` | Medium | Step-0 matching reached about one robot in ten |
| Every idle robot assigned in the same call, and a test asserts it | `cgar.cpp:1227-1237`, `cgar/tests/cgar_regression.cpp:61-71` | High | Poor step-0 choices become permanent |
| Unopened tasks never re-matched, though the simulator allows it | `cgar.cpp:1092-1103`, `cgar/src/TaskManager.cpp:46-53` | High | No repair path |
| Age ratio as ranking score | `cgar.cpp:1124-1129` | High, hypothesis | Wider candidates lengthen empty travel |
| Sampling loop never checks the clock | `probe-v2.patch:229`, `:298-307` | Medium | About 600,000 estimates at step 0 regardless of deadline |
| Exact and Manhattan distances ranked together | `cgar.cpp:1157-1166` | Medium on obstacle maps | Underestimated tasks win |
| Evacuation built robot by robot, waves batch only consecutive distinct robots, one transaction slot | `cgar.cpp:782-796`, `:752-760`, `:997` | High for progress | Long serial transactions block the primary |
| Move accepted into a cell whose occupant is committed to the mover's cell | `cgar.cpp:634-636`, `:1031-1035` | Low | The swap reaches the final guard, and both robots stay committed for steps |
| Progress minimum mixes Manhattan and exact values | `cgar.cpp:569-580`, `:990-995` | Low to medium | False stalls trigger unneeded primary transactions |
| Robot rotation advances one position per call | `cgar.cpp:1105-1106` | Low today | No real fairness once deferral exists |

| Log observation | Value |
|---|---|
| GAME production, one transaction | about 2,000 moves, 66 robots frozen |
| Steps that transaction held the only slot | before 200 until after 1,200, primary stall rising in lockstep |
| GAME completions per step, before 1,200 vs after 1,400 | 1.9 vs 0.9 |
| SORTATION transactions by step 800, `sample` vs `draft_scheduler` | 110 vs 5 |
| SORTATION table cache full | `sample` from step 400, fixed not by step 800 |
| SORTATION production, fair-slot admissions in 5,000 steps | 3 |
| Wall time per step across records | at most about 150 ms of 1,000 |

- **Radius example.** On an open grid the node cap reaches about 32 steps. A robot freed mid-map in SORTATION is farther than that from any free pickup. It searches twice, finds nothing, and receives the oldest task.
- **Evacuation example.** Three residents with ten-cell exits yield a witness listing all of the first robot's moves, then the second's. A wave ends at the first repeated robot, so waves hold about one move each. The three could leave as a train in about a dozen steps.
- **GAME cost.** GAME throughput was not lower during the long transaction, so the clear cost is progress latency.
- **Sampler detail.** The stride is a large prime, so one robot's draws cannot repeat in a pool of 64 or more, though the README says they can. The real weaknesses are the unbounded loop and draws that form an arithmetic progression.
- **Contention hypothesis.** Recovery activity under `sample` far exceeds the `draft_scheduler` control at similar throughput. Sampling ignores how many robots already head for the same pickup cell.

I agree with the earlier review that permanent Manhattan chain costs and cache reordering by scheduler lookups are real. Both look minor here, because route fallbacks to Manhattan stop growing by step 200 in every record.

The next table shows what executes in the selected leader snapshots. `kk` is the KittyKnight snapshot and `nms` is No Man's Sky.

| What executes | Location |
|---|---|
| KittyKnight compiles `wppl_planner` only, with `my_planner` commented out | `kk/CMakeLists.txt:54-58` |
| No Man's Sky comments out `Solution2` | `nms/CMakeLists.txt:71-78` |
| Both re-match unopened tasks each step | `kk/wppl_planner/src/scheduler.cpp:828-862`, `nms/Solution/Scheduler/scheduler_solver.cpp:192-226` |
| Cost is five times pickup distance plus chain, with no age term, on every map branch | `nms/Solution/Scheduler/scheduler_solver.cpp:53-83` |
| KittyKnight routing loads per-map weight files | `kk/wppl_planner/configs/sortation_large.json:14-18` |
| No Man's Sky routing hand-places one-way rows by coordinate | `nms/Solution/Objects/Environment/graph_guidance.cpp:69-146` |
| Generic traffic-cost search already compiled into CGAR | `cgar/default_planner/search.cpp:76-116` |

Both leader citations in the earlier review therefore point at unused code. The executed routing advantages rely on artefacts your constraints exclude.

## Plan and outlook

1. **First production patch.**
   - Skip the second search when the first found nothing.
   - Replace the fixed node cap with a per-call node budget shared among idle robots, keeping the sixteen-nearest cap.
   - Give remaining robots time-checked sampling.
   - When the clock expires, leave the rest idle and resume from a stored cursor next call. Global discovery makes that deferral bounded, which answers the README's objection to deferral.
   - Add a lookup that does not reorder the cache.
   - Validate with a short-budget test that asserts coverage within a bounded number of calls, plus uniqueness and eligibility.
2. **Objective.**
   - Rank by weighted pickup distance plus chain, using one global weight.
   - Keep age only in the explicit fair slot, set its period deliberately, and report task-age tails. The fallback was supplying most fairness at great cost.
   - Run `CGAR_HRRN=0` on all ten instances first, since it needs no code.
3. **Shared discovery.**
   - Run one breadth-first pass seeded from every pickup cell that holds a free task. Each idle robot claims its labelled cell.
   - Each cell accepts its nearest claimants, up to its task count and a cap derived from its free-neighbour count.
   - Rejected robots retry with exhausted cells removed.
   - Cost is one graph pass per round at any fleet size.
4. **Re-matching unopened tasks.**
   - Never re-match the primary.
   - Cap re-matches per task.
   - Require a strict improvement in exact distance.
   - A re-match issues a fresh ticket, which the existing epoch test requires.
5. **Recovery execution.**
   - Layer the validated serial witness by robot and cell dependencies, then replay the layered schedule with the existing validator. The witness itself is unchanged.
   - Reject a move into a cell whose occupant is committed to the mover's own cell.
   - Reset the progress minimum when its basis turns exact.
6. **Routing.**
   - First size the gap by running the default planner under the CGAR scheduler.
   - Then test observed-flow preference as a tie-break among candidates of equal potential only. That leaves the exact potential and stall logic untouched.
   - Later candidates are a wait-fraction throttle on active robots, motivated by GAME, and bounded orientation-aware lookahead.

Test each item alone against matched controls. Use full horizons, at least three seeds, three wall-clock repeats, and per-window completions. For attribution runs, bound table building by count so identical configurations reproduce. Record fallback reasons, robots per goal cell, transaction lengths, and task-age tails.

The regression against the draft is plausibly recoverable with both progress mechanisms intact. The one controlled comparison reaches the draft's level on the corrected backend, and items 1 to 4 never touch primary selection or witness installation. That evidence is one map, one seed and one thousand steps.

Recovery activity rises with traffic in the logs, and the GAME transaction shows the mechanisms have latency problems of their own. Nothing here measures the leaders. Their executed advantage includes map-specific routing you have ruled out, so parity with them is not implied. Simulator success remains no proof of liveness.
