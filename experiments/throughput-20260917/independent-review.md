**Independent CGAR throughput review — 2026-09-17**

The strongest evidence locates the major regression in task scheduling, especially initial pickup allocation, with shared distance-cache effects still part of the attribution. It does not support treating the loss as a necessary cost of the motion-correctness repairs.

This review was performed by an available session agent; the requested Fable 5.1 max model was unavailable. The independent work consisted of source inspection, existing-log inspection, a small task-distribution scan, and two tiny synthetic probes. The main agent supplied and ran the controlled ablations and trajectory analyses. This report changes no production code and does not report a new full-suite benchmark.

The decisive controlled comparison uses the same corrected motion backend on SORTATION for 1,000 steps, with 1,000 ms per decision and 30,000 ms preprocessing:

| Scheduler variant | Completed tasks | Planner errors | Schedule errors | Entry timeouts |
|---|---:|---:|---:|---:|
| Instrumented current scheduler | 4,704 | 0 | 0 | 0 |
| Hybrid exhaustive scoring for small pair products | 4,639 | 0 | 0 | 0 |
| Transplanted draft scheduler | 14,337 | 0 | 0 | 0 |

Sources: [current summary](results/fixed-SORTATION-1000.json), [hybrid summary](results/hybrid-SORTATION-1000.json), and [draft-scheduler summary](results/draft_scheduler-SORTATION-1000.json). The archived original draft completed 14,231 tasks in its first 1,000 steps; see [trajectory summary](trajectory-summary.json). These are single runs. The scheduler transplant changes assignment timing, assignment choices, and distance-cache accesses together, so it does not isolate those contributions from one another. Reverting to the draft scheduler is a diagnostic, not the proposed production fix.

The initial assignment measurements explain why the hybrid-only run does not recover throughput. At timestep zero, the instrumented current scheduler assigns 10,000 robots, but only 639 assignments come from local candidates; 9,361 use ordinary oldest-task fallback. It records 9,362 empty searches caused by the deadline and 415 caused by the node limit. Those counters count searches, not disjoint sets of robots. The hybrid run still uses sparse search for the initial large batch and makes 9,725 fallback assignments. After initialization, its recorded fallback count stops increasing and its incremental pickup cost is much lower, but it retains the initial poor assignments.

The archived full runs independently show that first-task pickup allocations become much worse. This comparison includes the first task of every robot, rather than comparing the draft's first 200 assignments with the fixed scheduler's first 10,000:

| Instance | Mean first pickup Manhattan distance, draft → fixed | Mean observed pickup delay, draft → fixed |
|---|---:|---:|
| WAREHOUSE | 41.54 → 218.29 | 121.17 → 768.93 steps |
| SORTATION | 44.68 → 234.47 | 75.73 → 929.24 steps |

All 10,000 first pickups were observed in each cohort. The draft spreads initial assignments over a mean assignment step of 17.57; the fixed scheduler assigns every robot at recorded step 1. Planner timestep-zero diagnostics and recorded assignment step 1 use different indexing conventions. See [initial cohorts](initial-cohorts.json).

Mean delivery durations among completed tasks change much less: WAREHOUSE 799.93 → 821.56 steps and SORTATION 536.55 → 560.44 steps. These completed-task populations differ, so this is supporting evidence rather than a controlled delivery-speed comparison. Both affected maps have `core=free`, no pockets, and no parked robots. Their early throughput gaps also appear before substantial transaction activity. Pocket normalization and capacity parking cannot explain these two regressions.

Two selection errors are directly reproducible in the current scheduler:

1. Candidate discovery stops after 16 task IDs, even when there is one idle robot and the configured pair budget is 2,000,000. Discovery uses pickup distance before ranking whole-chain cost. A slightly farther pickup with a much shorter chain can therefore be excluded. The additional 2,048-visited-cell limit can yield no candidate at all, after which ordinary fallback chooses the oldest task without comparing alternatives. See `cgar.cpp` lines 1186–1235 in the [reviewed production source](../../cgar/cgar_planner/cgar.cpp).
2. Tasks sharing a pickup are sorted using HRRN at pickup distance zero. Their true order can reverse at the robot's nonzero pickup distance, but the candidate-count cap can discard the correct winner first. See `cgar.cpp` lines 1124–1138 and 1197–1199. In the first 15,000 task definitions, WAREHOUSE has 6,765 tasks in pickup buckets containing at least 16 tasks; SORTATION has 1,856. Thus an entire 16-candidate budget can be consumed at one endpoint. These distribution counts establish exposure to the mechanism, not its standalone contribution to benchmark loss.

The persisted [synthetic probe](scheduler_selection_probe.cpp) was compiled against the existing corrected planner object, with a matching header. Both cases use one robot on an open two-row grid, a valid certificate, no pockets, a 100 ms scheduling budget, `CGAR_MAX_PAIRS=2000000`, `CGAR_SCHED_TABLES=128`, and HRRN enabled. All relevant distances are exact straight-line grid distances.

- Chain-cap fixture: the robot starts at cell 0 in a 2×101 grid. Tasks 0–15 visit `{0,100}`; task 16 visits `{1,1}`. All are revealed at timestep 0. The selected task has distance cost 100; the available optimum under the implemented scoring rule costs 1. These costs are the scheduler's movement-distance objective, not rotation- and service-inclusive execution times.
- HRRN-bucket fixture: the robot starts at cell 0 in a 2×31 grid at timestep 100. Tasks 0–15 visit `{20,21}` and were revealed at timestep 80. Task 16 visits `{20,30}` and was revealed at timestep 0. At pickup distance zero the shorter tasks rank first. At the actual pickup distance 20, task 16 has the larger HRRN score: `1 + 100/30 > 1 + 20/21`.

Actual output:

```text
CHAIN_CAP selected=0 selected_cost=100 optimal_task=16 optimal_cost=1
BUCKET_HRRN selected=0 selected_cost=21 selected_score=1.95238 optimal_task=16 optimal_cost=30 optimal_score=4.33333
```

The second optimum is the HRRN optimum, not the minimum-distance task. To reproduce from the workspace root with a matching built backend:

```sh
env/bin/x86_64-conda-linux-gnu-c++ -std=c++17 -O0 \
  -I cgar/inc -I cgar/cgar_planner -isystem env/include \
  experiments/throughput-20260917/scheduler_selection_probe.cpp \
  cgar/build/CMakeFiles/lifelong.dir/cgar_planner/cgar.cpp.o \
  -Wl,-rpath,"$PWD/env/lib" -o /tmp/cgar-scheduler-selection-probe
/tmp/cgar-scheduler-selection-probe
```

Several secondary mechanisms deserve separate measurement:

- `task_chain_cost()` caches a Manhattan fallback permanently by task ID, without recording exactness or upgrading it when a distance table appears (`cgar.cpp` lines 1059–1081). The fixed scheduler estimates every initially available task with only 128 table builds and a distance deadline. Underestimated chain costs are favored by the HRRN denominator. The mechanism is established by inspection; its throughput contribution remains unisolated.
- `DistanceOracle::find()` updates shared LRU order on every lookup (lines 365–369), and `plan()` trims the cache before routing (line 925). Broad scheduler scans therefore change which active route goals survive. Use no-build, preferably non-LRU-touching, estimates for broad scoring, and refine selected candidates within the distance budget. Cache compaction increased table capacity relative to the draft, so compaction itself is a weaker explanation for the loss.
- `Agent.best` records minima across both Manhattan and exact route estimates (lines 562–580 and 990–995). An early underestimate can cause later exact progress to count as a stall until it beats the old lower bound. Track a consistent exact potential for certified stall detection; retain the primary ticket when changing the estimate basis. Large observed stall counters are compatible with this mechanism but do not prove it caused the regression.
- Rotation commitments and blocker-induced waits merit counters, but their principal rules predate this regression. The corrected-backend scheduler transplant is stronger evidence than speculation about relaxing commitments or shortening recovery witnesses.

The ranked, map-agnostic implementation plan is:

1. **Repair large-batch initialization.** As a diagnostic, replace otherwise-unmatched robots' ordinary oldest fallback with a deterministic sample of up to 64 still-unused tasks, scored with the existing no-build estimate. Include oldest as a feasible candidate baseline and retain the separate explicit fair slot. At the observed 9,361 fallback robots this requires approximately 599,104 sample evaluations. Sample from a live task vector with constant-time removal, use a sampler independent of PIBT's random stream, and reserve fallback work before local BFS consumes the scheduler budget. Sequential sampling still favors earlier robots and can miss rare good choices; it is a bounded approximation, not an optimal matcher. The three ablations above do not test this policy; subsequent measurements and holdouts are recorded in the [main diagnosis](README.md).
2. **Use exhaustive scoring when the pair product fits.** For small `idle_robots × free_tasks`, evaluate all pairs with no-build estimates under the existing work budget. This addresses steady-state undercoverage but cannot repair already-installed initial assignments. In sparse mode, evaluate co-located tasks at the robot's actual pickup distance before retaining the best pairs; separate endpoint exploration limits from retained task-count limits.
3. **Share search work for larger batches.** A stronger subsequent design uses multi-source BFS from idle robots to obtain several nearby-robot labels at task endpoints, evaluates every task's whole-chain score, and matches the resulting sparse pairs. Additional labels, rounds, or global samples can refill unmatched robots. This covers useful tasks beyond any individual robot's fixed local radius while sharing graph traversal. Respect certificate-compatible distance semantics; a search through unrestricted free cells is only a ranking lower bound when pocket restrictions apply.
4. **Refine estimates and isolate cache policy.** Record chain exactness and the remaining-stop epoch, retain exact scalar leg costs after table eviction, and upgrade approximate legs when information becomes available. Test non-LRU-touching scheduler lookups separately from assignment changes. Preserve priority for constructing the persistent primary's complete distance table.

All proposals use task data, fleet size, budgets, and graph topology. None requires map names, coordinates, aisle orientations, or per-map constants.

The progress constraints are part of the implementation requirements. Preserve eligible-task filtering, parked-robot exclusions, unique assignments, persistent task/stop tickets, complete validated witnesses, and the independently evaluated oldest-task admission. Do not restore task-ID prefix eligibility or transaction age aborts to recover throughput. Keep ordinary sampled admissions and explicit fair admissions distinct in accounting. Fairness measured in admissions remains conditional on continued completion of admitted feasible tasks; it is not by itself an end-to-end timestep bound.

Pure deferral of assignments with no local candidate is not a production recommendation. Repeating the same bounded search can defer a robot or task forever, and a fair counter triggered only by successful admissions can stop advancing. Deferral needs resumable search or another guaranteed global-discovery mechanism, plus an admission rule that continues to make progress when local search finds nothing.

Validation should first preserve the existing certificate, action-safety, primary-persistence, pocket, capacity, assignment-coverage, and fair-admission checks, and add the two small selection regressions above. Add a sparse-endpoint case beyond the local node cap, a short-budget case, and sampling checks for uniqueness and eligible-task coverage. For attribution, compare initialization policies independently of the steady-state hybrid branch, keep motion configuration fixed, and record initial pickup distance/delay, fallback reasons, actual sample counts, approximate-estimate frequency, cache misses, rotations, and commitment-blocked waits. Then rerun the complete suite with matched settings and repeated seeds before claiming a general throughput improvement. No throughput result alone establishes starvation freedom.

After dispatch repair, two generic ideas from the local competition sources are worth evaluating against any remaining gap to the leaders. They are subsequent optimization proposals: the corrected motion backend already reaches 14,337 tasks with the transplanted scheduler, versus the archived draft's 14,231, so recovering this regression does not require weakening its progress mechanisms. This limited source review neither measures a remaining leader gap nor attributes a leaderboard score to a particular component.

- **Price route congestion from traffic.** KK contains directed-edge flow accounting in [flow.cpp](../../Code-Archive/2024%20Competition/Team_Kitty_Knight/da7d428b40cc1425b381adefe9709d4accbfdfe1/my_planner/src/flow.cpp), lines 14–55, and adds opposing-flow and vertex-load costs during search in [search.cpp](../../Code-Archive/2024%20Competition/Team_Kitty_Knight/da7d428b40cc1425b381adefe9709d4accbfdfe1/my_planner/src/search.cpp), lines 101–120. These costs enter search ordering in [search_node.h](../../Code-Archive/2024%20Competition/Team_Kitty_Knight/da7d428b40cc1425b381adefe9709d4accbfdfe1/my_planner/inc/search_node.h), lines 102–132. The flow-refinement calls in the inspected [planner.cpp](../../Code-Archive/2024%20Competition/Team_Kitty_Knight/da7d428b40cc1425b381adefe9709d4accbfdfe1/my_planner/src/planner.cpp), lines 123–139, are commented out; this is a concrete available implementation pattern, not evidence that those calls produced KK's measured performance. A CGAR experiment can start with observed traffic or short route-prefix counts as tie-breakers among equally short admissible routes. Derive all counts online, retain uniform base movement costs, and use no map-specific lanes or weights. Keep the static exact potential used for stall detection and certified promotion separate from changing traffic costs. Test whether opposing traversals and blocking waits decrease without worsening deadlines or goal-age tails.
- **Plan rotation time explicitly.** NMS's [time-space state](../../Code-Archive/2024%20Competition/Team_No_Man%27s_Sky/44950d1b61b7482b4529a9dc127fa9d220962bdc/Solution2/inc/LNS/Parallel/TimeSpaceAStarState.h), lines 15–20, carries position, orientation, and time. Its [successor generator](../../Code-Archive/2024%20Competition/Team_No_Man%27s_Sky/44950d1b61b7482b4529a9dc127fa9d220962bdc/Solution2/src/LNS/Parallel/TimeSpaceAStarPlanner.cpp), lines 104–205, advances time for forward, clockwise, counterclockwise, and wait actions and checks time-indexed constraints. A bounded CGAR experiment could use the same action-model distinction in short lookahead for selected congested robots, with uniform action costs and the current validated action set as its fallback. Account for memory and search time rather than building unrestricted orientation-aware tables for every goal. Preserve the primary, pocket exclusions, and reserved recovery witnesses; independently check any accepted action replacement. Measure turn-induced dependency waits, actual forward progress, and computation cost. The proposal is to model turns more accurately, not to expire unresolved commitments or abandon certified recovery.

**Final critical pass after holdout measurements.** Recommend the fallback repair first and retain the hybrid branch as an experiment. The holdout regressions are attributable to enabling that branch as a package; the claim that improved optimization of its score itself causes the loss remains a hypothesis because matching, cache access, and time allocation also change. The fallback-only SORTATION run recovers about 78% of the measured short-horizon loss and remains about 15% below the transplanted-scheduler control. RANDOM-01 has no observed fallback exposure, so its unchanged result is a limited non-regression check. Initial allocation dominates these short experiments; sustained throughput requires longer runs and repeated trials. Use fresh matched controls for effect sizes and keep archived runs as context. See the main diagnosis for all final WAREHOUSE, SORTATION, RANDOM-01, and CITY-01 measurements.
