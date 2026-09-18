# CGAR throughput diagnosis — 2026-09-17

Much of the WAREHOUSE and SORTATION regression appears recoverable without map-specific logic. The strongest causal evidence points to the scheduler's budget-exhausted fallback, introduced in the preceding migration fixes. Repairing that fallback is the first recommendation. These experiments leave the production planner unchanged and do not establish that the full competition gap to KittyKnight or NMS is closed.

**What caused the regression**

The sparse scheduler searches locally for task pickups, stops after 16 task IDs or 2,048 visited cells, and shares a wall-clock budget across the fleet. An unmatched robot then gets the oldest available task if its new local search finds nothing. On the instrumented SORTATION run's initial call, only 639 of 10,000 assignments came from local candidates; **9,361 used this ordinary fallback**. Once the budget is exhausted, it commits the rest of the fleet to tasks without comparing their travel costs. This fallback is separate from the explicit oldest-task admission intended to preserve fairness.

The [archived trajectory analysis](initial-cohorts.json) compares the first task of every robot, using its position at assignment. All 10,000 first pickups were observed in each cohort:

| Instance | First pickup Manhattan distance, draft → fixed | First pickup delay, draft → fixed |
|---|---:|---:|
| WAREHOUSE | 41.54 → 218.29 | 121.17 → 768.93 steps |
| SORTATION | 44.68 → 234.47 | 75.73 → 929.24 steps |

The draft spreads initial assignments over a mean assignment step of 17.57; the fixed scheduler assigns all robots at recorded step 1. This is an allocation/timing comparison, not a comparison of identical assignment cohorts. Mean delivery duration among completed tasks changes much less: WAREHOUSE 799.93 → 821.56 steps and SORTATION 536.55 → 560.44 steps. Those completed-task populations also differ. See [full trajectory statistics](trajectory-summary.json).

Both affected maps have every free cell in the certified core, no pockets, and no parked robots. Their early gaps precede substantial recovery-transaction activity. Pocket normalization and capacity parking cannot explain these two regressions. More decisively, transplanting the draft scheduler into the corrected motion backend produces **14,337 SORTATION tasks in 1,000 steps**, versus **4,704** with the current scheduler. The transplant also changes distance-cache accesses, so allocation and cache effects are not fully separated. It retains known draft scheduling shortcomings and is only a diagnostic control.

**Measured prototypes**

The fallback prototype reserves one quarter of the scheduler's budget for the final allocation pass. When local search produces no candidate, it compares the oldest task with 64 deterministically sampled entries from the still-unused task vector, using existing distance/whole-chain estimates. Removal is constant time, the sample does not consume PIBT's random stream, and the separate fair-admission rule remains active. The 64 draws are distinct for the observed pools of at least 64 tasks; smaller pools can repeat entries. Sampling can miss good candidates and is a bounded approximation.

A separate hybrid branch evaluates all robot/task pairs when their product fits the existing pair budget. Both branches use identical settings across all tested maps. No map names, special coordinates, preferred aisle directions, or per-map weights appear in either policy.

| Instance | Steps | Current scheduler | Fallback repair only | Fallback + hybrid scoring |
|---|---:|---:|---:|---:|
| SORTATION | 1,000 | 4,704 | **12,173 (+158.8%)** | 12,575 |
| WAREHOUSE | 1,000 | 5,696 | **8,464 (+48.6%)** | 8,424 |
| RANDOM-01 | 600 | 457 | **457 (unchanged)** | 408 |
| CITY-01 | 1,000 | 2,093 | **2,108 (+0.7%)** | 2,038 |

The fallback-only SORTATION result recovers **77.5% of the measured gap** to the 14,337-task transplanted-scheduler control. Its initial estimated pickup distance averages about 65 cells, versus 233 for the instrumented current scheduler. This measurement combines exact distances and Manhattan fallbacks; it is not the observed pickup delay. The prototype also selects shorter estimated task chains, so the throughput gain cannot be assigned entirely to pickup travel.

Hybrid scoring alone reaches only 4,639 SORTATION tasks because it leaves the bad large-batch initialization in place. Adding it to the fallback repair regresses both holdouts: RANDOM-01 by 10.7% and CITY-01 by 2.6%. Broader candidates, matching choices, approximate distances, and cache accesses all change in that branch. A better score under the current surrogate is not sufficient evidence of better throughput. **Keep the hybrid branch experimental.** The fallback-only RANDOM-01 result has no observed fallback use; it demonstrates unchanged behavior where local search already succeeds, not broad validation of the sampling policy. The small CITY-01 difference should be treated as roughly flat, given timing sensitivity.

All **14 runs** completed with zero planner errors, scheduling errors, and entry timeouts. The existing regression suite passes with both fallback-only and combined variants, including the 1,608 certificate cases, pocket evacuation, persistent primary, capacity, 10,000-robot assignment, fair admission, compact distances, and short-deadline checks. Two additional selection probes reproduce weaknesses in the current scheduler: a 16-task cap excludes a much cheaper whole chain, and ordering a shared-pickup bucket at distance zero excludes the best HRRN task at the robot's actual distance. Hybrid scoring fixes both probe selections, while the fallback-only change intentionally leaves those separate issues untouched. See [validation output](validation.txt) and the [probe source](scheduler_selection_probe.cpp).

**What I would implement next, in order**

1. **Make budget exhaustion preserve assignment quality.** Promote the fallback repair after bounding its actual evaluation cost under short budgets. Keep the oldest task as a candidate and retain the independent fair admission. Track local/fallback/fair assignments, deadline versus node-limit misses, sampled evaluations, and pickup distances. Do not simply defer every unmatched robot: repeating the same bounded search can defer work forever and stop admissions-driven fairness from advancing.
2. **Share candidate discovery across the fleet.** For large batches, use graph search from idle robots to find several nearby robots at each task endpoint; score whole chains and match the resulting sparse pairs. Replenish unmatched robots with further labels or global samples. This avoids thousands of overlapping local searches and provides coverage beyond a fixed local radius. In local mode, separate endpoint coverage from task-count caps and evaluate co-located tasks at the robot's actual pickup distance. Test each change independently; the negative hybrid holdouts rule out adopting exhaustive scoring unconditionally on current evidence.
3. **Make distance estimates refinable and separate scheduler cache policy.** The current chain cache permanently retains a Manhattan fallback without recording its exactness. Record approximate/exact leg costs and the remaining-stop epoch, upgrade estimates when tables become available, and retain exact scalar leg costs after table eviction. Broad task scoring should not repeatedly promote entries in the route table's shared LRU. Preserve priority for completing the persistent primary's exact table. Also keep stall detection on a consistent distance basis: mixing a prior Manhattan minimum with later exact distances can falsely classify progress as a stall. These mechanisms are established by source inspection; their individual throughput effects remain unmeasured.
4. **Then address routing quality with generic traffic and turn costs.** Evaluate online opposing-flow/vertex-load penalties as route tie-breakers, and bounded orientation-aware lookahead for selected congested robots. The local KK and NMS sources contain useful implementation patterns, detailed in the independent review. Use observed traffic and uniform action costs, with no hand-built lanes or map-specific constants. Keep dynamic traffic preferences separate from the static progress potential and validate any lookahead action against recovery reservations.

Preserve eligibility filtering, parked-robot exclusions, unique assignments, persistent task/stop tickets, complete validated recovery witnesses, and fair admission throughout. Do not restore task-ID prefix eligibility or abort unresolved recovery transactions just to recover throughput. Simulator success and finite regressions do not establish end-to-end starvation freedom.

**Limits and next benchmark**

These are single runs at default PIBT seed 0, executed sequentially under the shared one-CPU quota, with 1,000 ms per simulator decision and 30,000 ms preprocessing. The scheduler receives its existing fraction of that decision budget. The first three SORTATION controls used instrumented build v1; remaining runs used v2, which also maintains the available-task vector with all flags disabled. Motion algorithms and production source revision are the same; bookkeeping and wall-clock variation remain possible confounders. [Source patches](probe-v1.patch), [v2 patch](probe-v2.patch), [hashes](metadata.json), [CSV](summary.csv), and [per-run records](results/) preserve the details.

Timing sensitivity is material: the archived fixed WAREHOUSE run completed 4,437 tasks in its first 1,000 steps, while the fresh control reaches 5,696. Archived draft prefixes are 8,121 for WAREHOUSE and 14,231 for SORTATION. They are context, not matched controls for the effect sizes above. The 1,000-step experiments emphasize initialization and do not establish sustained gains over the archived 5,000-step horizons. Run the minimal fallback change through all ten full horizons, repeated wall-clock trials and multiple seeds, before promoting a general throughput claim. Record per-1,000-step throughput, pickup/delivery delays, task-age tails, recovery activity, and deadlines as well as total tasks. Compare the leaders under the same settings afterward; this pass has not measured their full-suite gap.

**Reproduction**

Build production CGAR and its tests using the [workspace instructions](../../README.md). Then, from the repository root:

```sh
python3 experiments/throughput-20260917/build_probe.py
python3 experiments/throughput-20260917/validate_probe.py
python3 experiments/throughput-20260917/run_ablation.py fixed SORTATION --steps 1000
python3 experiments/throughput-20260917/run_ablation.py sample SORTATION --steps 1000
python3 experiments/throughput-20260917/run_ablation.py hybrid_sample SORTATION --steps 1000
```

The builder requires the recorded production source hash and reuses the existing simulator objects. It writes only beneath the ignored experiment `build/v2/` directory. The runner defaults to that build, clears inherited `CGAR_*` settings, uses a fresh output directory for each run, and rejects nonzero exits, wrong horizons, errors, or timeouts. Its diagnostic options are not production configuration. The current builder produces v2; the v1 patch retains the earlier instrumentation for audit. Raw outputs stay in ignored `runs/cgar-diagnosis-20260917/`; compact summaries and metadata are committed here. The two trajectory-analysis scripts require the preserved local raw draft and fixed outputs, which are not published.

The [independent review](independent-review.md) provides source locations, synthetic counterexamples, and an additional critical pass on these recommendations. That first pass used the session agent tool. Fable 5.1 was not listed by that tool; the installed Claude Code CLI had not been checked at that point.

**Follow-up parallel evaluation, September 18:** [all ten archived horizons were run concurrently](../../benchmarks/mr24-parallel-20260918/summary.md) with the same production executable under the shared one-CPU quota. All ten completed without errors or entry timeouts, but GAME, both CITY cases, WAREHOUSE, and SORTATION had lower throughput than the sequential run. This adds evidence of sensitivity to available computation time; it is not a test of the fallback prototype.

The requested [Fable 5.1 Max review through Claude Code](../throughput-20260918-fable/README.md) is now complete. Its [checked findings](../throughput-20260918-fable/assessment.md) add steady-state node-limit failures, reassignment and recovery-latency proposals, and qualifications to the interpretation of the hybrid and sampling comparisons.
