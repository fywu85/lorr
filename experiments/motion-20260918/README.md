# CGAR movement diagnosis, 18 September 2026

The optional orientation-and-turn profile improves WAREHOUSE by 10.7%, CITY-01 by 6.2%, and RANDOM-02 by 9.9% across six seeds. SORTATION averages -1.5%, with individual effects from -9.1% to +6.5%, so the default remains unchanged.

The matched-goal experiment establishes a substantial movement gap even when NMS's map-specific guidance and fleet limits are removed. CGAR loses substantial agent time to waiting, including forward proposals whose dependency chain ends at a turning robot. This identifies an opportunity; it does not make every observed wait avoidable.

The retained changes are observational diagnostics, optional turn-first tie-breaking and optional orientation-aware route costs. Two unsuccessful movement mechanisms are archived as reproducible patches and removed from the active planner. No map-specific CGAR rule was introduced. The default motion policy remains unchanged; the switches and measured tradeoffs below are explicit.

## 1. Where CGAR spends time

Thirty full-horizon instrumented runs cover all ten instances and seeds 0, 1 and 2. All reproduce the previous baseline's paths, assignments, task events and revealed tasks exactly. The counters use read-only cached distances: they build no tables, promote no cache entries and consume no randomness. [Equivalence](results/diagnostic-equivalence.json), [counters](results/movement-counters.json).

Percentages below pool the three seeds within each instance. Forward, turning, planned waiting and blocked forward waiting are disjoint, apart from the separately retained tiny final-safety category. Farther moves are a subset of forward actions; recovery participation overlaps actions. A move farther from the goal may be necessary to unblock traffic.

| Instance | Forward | Turn | Planned wait | Forward blocked | Forward farther | In recovery |
|---|---|---|---|---|---|---|
| WAREHOUSE | 43.0% | 12.7% | 29.0% | 15.3% | 9.4% | 0.0% |
| SORTATION | 60.0% | 12.0% | 17.1% | 11.0% | 7.9% | 0.0% |
| CITY-01 | 77.5% | 11.3% | 6.6% | 4.6% | 2.7% | 0.0% |
| CITY-02 | 55.1% | 16.8% | 14.0% | 14.1% | 7.3% | 0.0% |
| GAME | 34.9% | 19.4% | 19.7% | 26.0% | 11.2% | 0.4% |
| RANDOM-01 | 59.8% | 25.6% | 8.9% | 5.7% | 4.1% | 0.1% |
| RANDOM-02 | 44.3% | 25.1% | 19.2% | 11.4% | 6.8% | 0.2% |
| RANDOM-03 | 31.6% | 23.7% | 27.1% | 17.6% | 9.4% | 0.1% |
| RANDOM-04 | 25.9% | 22.0% | 30.8% | 21.3% | 10.2% | 0.2% |
| RANDOM-05 | 22.3% | 19.0% | 37.3% | 21.3% | 9.1% | 0.1% |

A blocked forward action was originally offered as FW and converted to W. Almost all these dependency chains end at a robot offering a turn. Planned waits include reservation pressure and congestion; they are not all voluntarily idle robots. Recovery means direct membership of an installed witness, including future waves. It does not measure all traffic effects caused by recovery or priority rules. The rare recoveries on WAREHOUSE/SORTATION do not support blaming recovery alone for their large gap.

The pickup/delivery split is measured from actual task-stop state, with action counts independently checked against simulator trajectories. Phase-specific percentages use only robot steps in that phase; the phase share uses all robot steps.

| Instance | Phase | Share of agent time | Forward in phase | Wait in phase | Turn-blocked in phase |
|---|---|---|---|---|---|
| WAREHOUSE | before pickup | 21.9% | 40.7% | 45.9% | 13.0% |
| WAREHOUSE | after pickup | 78.1% | 43.6% | 43.8% | 15.9% |
| SORTATION | before pickup | 24.6% | 61.1% | 27.5% | 9.9% |
| SORTATION | after pickup | 75.4% | 59.6% | 28.3% | 11.3% |
| GAME | before pickup | 2.5% | 28.9% | 51.3% | 16.1% |
| GAME | after pickup | 97.5% | 35.0% | 45.6% | 26.3% |

## 2. Same positions and goals, different motion planners

Twenty inputs are taken immediately before decisions 101 and 501 of the seed-0 CGAR baseline: two snapshots per instance. Both planners start fresh on identical positions, orientations and current goals. Each robot has at most one fixed goal, no scheduling or reassignment runs, and no new work is revealed. Reached goals retire; those robots remain movable. The 100-step probe measures goal arrivals and remaining exact spatial distance, not lifelong throughput. Both planners lose their original internal history.

The supplied NMS reference keeps the archived configuration. A second reference replaces only environment initialization with unit costs for every legal action, a generic map/test type, and no category-specific active-fleet limit. It retains NMS's orientation graph, 129 global action sequences, PIBTS search and 32-worker portfolio. The original submission is unchanged. The replacement is [nms_uniform_environment.cpp](nms_uniform_environment.cpp).

Counts below sum the two 100-step probes. Goals already occupied at the start are excluded from arrivals. Every one of the 80 replay runs completed without a collision or late decision. The 20 repeated CGAR cases have identical per-agent arrival times and movement summaries across the two reference matrices; this check is not a full path hash.

| Instance | CGAR arrivals | Supplied NMS | Uniform NMS | Uniform / CGAR |
|---|---|---|---|---|
| WAREHOUSE | 4159 | 8769 | 8047 | 1.93x |
| SORTATION | 6092 | 9607 | 9445 | 1.55x |
| CITY-01 | 1110 | 1256 | 1190 | 1.07x |
| CITY-02 | 1561 | 2145 | 2009 | 1.29x |
| GAME | 818 | 1799 | 1362 | 1.67x |
| RANDOM-01 | 200 | 148 | 197 | 0.98x |
| RANDOM-02 | 364 | 279 | 397 | 1.09x |
| RANDOM-03 | 438 | 551 | 790 | 1.80x |
| RANDOM-04 | 356 | 640 | 971 | 2.73x |
| RANDOM-05 | 316 | 400 | 673 | 2.13x |

This controls task assignment and shows that the motion stack matters. It does not attribute the difference solely to orientation, lookahead or parallel search. NMS had 32 logical CPUs on 16 physical cores and 128 GiB requested memory; CGAR was pinned to one physical core of that same allocation. Both had a 1,000 ms decision limit, with their existing preprocessing allowances. Thus this is a resource-aware diagnostic, not an equal-compute comparison or an official competition score. Each NMS snapshot was run once; its deadline-driven search can vary between runs.

Supplied NMS deactivates up to 4,000 pending GAME robots and 300 RANDOM-04 robots; the uniform reference deactivates zero pending robots. RANDOM-05 retains CGAR's existing capacity restriction and 31 parked robots. The snapshots originate from CGAR and cover two fixed times, not a random sample of all encountered states. [Inputs](results/snapshot-inputs.json), [results and allocations](results/snapshots.json), [reference build provenance](results/snapshot-uniform-build.json).

## 3. Full-horizon motion changes

Effects are arithmetic means of paired completion changes, not confidence intervals or an aggregate competition score. Guidance experiments use seeds 0–2. The final cache policy is also tested on seeds 3–5, chosen before those results were observed. Those are additional planner seeds on the same maps and task inputs, not held-out maps.

`turn_first` changes only the order of equal-distance candidates: fewer required turns before empty-cell preference. `orientation` caches exact unit-cost reverse BFS distances over (cell, orientation), then ranks complete turn-and-forward macros against a wait. The original spatial potential, persistent primary, pocket locks, commitments and recovery witness remain in control of progress. The combination applies both changes globally.

| Instance | turn_first | orientation | orientation_turn_first |
|---|---|---|---|
| WAREHOUSE | +7.5% | +1.3% | +10.4% |
| SORTATION | -2.2% | -3.2% | -0.7% |
| CITY-01 | +4.9% | +5.3% | +7.5% |
| CITY-02 | +1.0% | +0.2% | +3.1% |
| GAME | -0.2% | -0.4% | -0.3% |
| RANDOM-01 | +0.5% | +2.5% | +3.0% |
| RANDOM-02 | -2.1% | +6.7% | +7.0% |
| RANDOM-03 | +1.6% | +4.2% | +5.5% |
| RANDOM-04 | +1.0% | +3.7% | +2.5% |
| RANDOM-05 | -0.2% | +3.5% | +1.9% |

The first oriented cache holds 512 MiB and permits 32 complete new tables per decision. Its LRU working set churns on large instances: most eligible PIBT calls still use spatial guidance, while table construction consumes about four times the baseline's total decision time. A prescribed count, not elapsed time, decides when guidance falls back.

The revised cache ranks currently requested goals every 32 steps, preferring higher demand and retaining resident entries on ties. It admits only as many goals as fit in the same 512 MiB. Both the number of table builds and the refresh interval are fixed policy bounds. This avoids repeatedly rebuilding goals that cannot remain resident. Neither policy uses map names, map categories or supplied guidance weights.

| Instance | Seeds 0–2 | Seeds 3–5 | All six seeds |
|---|---|---|---|
| WAREHOUSE | +11.4% | +10.0% | +10.7% |
| SORTATION | -1.0% | -2.0% | -1.5% |
| CITY-01 | +6.7% | +5.6% | +6.2% |
| CITY-02 | +1.3% | +0.5% | +0.9% |
| GAME | -0.2% | +0.4% | +0.1% |
| RANDOM-01 | +3.4% | +5.0% | +4.2% |
| RANDOM-02 | +10.3% | +9.5% | +9.9% |
| RANDOM-03 | +4.0% | +5.9% | +5.0% |
| RANDOM-04 | +4.8% | +2.3% | +3.6% |
| RANDOM-05 | +3.3% | -0.1% | +1.6% |

On the same first three seeds, the revised cache builds 119,881 tables versus 2,027,746 for the original combined profile: 94.1% fewer. The revised profile used 1.21 times the control's total recorded wrapper time within its six-seed matrix, compared with 4.27 times for the earlier combined profile within its own matrix. Different CPU models prevent treating the two matrices as a direct speed comparison. The simulator's wrapper includes logging after planning returns; four measurements exceeded one second, discussed below. It remains optional as `CGAR_ORIENTATION_GUIDANCE=2 CGAR_TURN_FIRST=1`. Full per-seed effects and waiting-task ages are retained, including regressions. [Effects](results/effects.json), [full metrics](results/admission/metrics.json).

## 4. Rejected experiments

A four-proposal experiment varies ordinary robot priorities, completes all four valid action sets and scores their actual actions after cancellation. It preserves the primary identity/priority and fixed recovery constraints, and returns only after all prescribed work finishes. Its immediate-progress objective and priority perturbation reduce full-run throughput. This rejects this implementation, not fixed-work search in general.

A second experiment reserves a turning robot's current cell during PIBT and reports that it cannot vacate that cell to its requesting parent. It preserves the turn intent. This produces much more restrictive planning and severe throughput losses; changing the accounting point for a blocked action does not remove the underlying coordination problem.

| Instance | portfolio4 | turn_first_portfolio4 | rotation | rotation_turn_first |
|---|---|---|---|---|
| WAREHOUSE | -39.9% | -39.4% | -63.9% | -62.6% |
| SORTATION | -4.5% | -30.8% | -58.5% | -59.3% |
| CITY-01 | -16.4% | -11.1% | -52.8% | -51.6% |
| CITY-02 | -11.5% | -10.9% | -51.9% | -51.9% |
| GAME | -17.0% | -16.7% | -68.7% | -68.7% |
| RANDOM-01 | -15.6% | -12.6% | -26.9% | -33.6% |
| RANDOM-02 | -16.6% | -15.8% | -44.8% | -45.3% |
| RANDOM-03 | -21.9% | -20.9% | -56.9% | -54.6% |
| RANDOM-04 | -22.7% | -22.0% | -80.3% | -79.8% |
| RANDOM-05 | -16.5% | -19.2% | -90.3% | -90.0% |

Both mechanisms are removed from active CGAR. Their source changes are retained in [prototypes/](prototypes/), as patches against commit `83fc931`, with frozen build hashes and every completed run. Both archived patches apply cleanly to that commit and reproduce their frozen source hashes ([check](results/prototype-reproduction.json)). They have no live environment switch in the final planner. Regression fixtures exercised primary persistence, pocket evacuation, capacity bootstrap and recovery protection, in addition to simulator validation.

The next larger mechanism to investigate is a fixed amount of search over short **action sequences**, including turns, waits and forward moves, with simultaneous occupancy at each future step. NMS searches a pool of 129 five-action sequences; a one-step spatial proposal plus later repair cannot express that same coordination. Any CGAR integration still needs the primary and recovery reservations represented in every future step. These results do not yet prove that this mechanism alone closes the gap.

## Validation, resources and reproduction

The main matrices contain 450 full-horizon CGAR runs, with 450 recorded valid by the simulator and no reported action errors or entry timeouts. Twelve additional full-horizon runs audit the timing outliers, for 462 total, plus 80 matched-snapshot replays. Recorded validity does not imply that every simulator wrapper measurement stayed below one second. All archived horizons are retained because short lifelong prefixes can reverse the final policy ranking. The 100-step motion probes isolate a different question; they are not a proposed replacement benchmark horizon. Every retained baseline with seeds 0–2 reproduces the prior trajectory: 150 comparisons. [Evidence](results/baseline-equivalence.json).

The main CGAR matrices use 30 distinct reserved physical cores, one per native instance, unlimited CPU quota, exclusive GRID allocation, and 8 GiB requested per slot. The scheduler's process address-space limit scales with all slots; this is not an independent 8 GiB limit for each child. CPU models differ between matrices, so runtime ratios are computed only against the control within each matrix.

| Matrix/profile | Recorded valid / runs | Max wrapper time, s | Wrapper time / control | Peak RSS, GiB |
|---|---|---|---|---|
| diagnostics/baseline | 30/30 | 0.528 | 1.00x | 6.233 |
| orientation/baseline | 30/30 | 0.532 | 1.00x | 6.235 |
| orientation/orientation | 30/30 | 0.600 | 4.25x | 7.017 |
| orientation/orientation_turn_first | 30/30 | 0.601 | 4.27x | 7.017 |
| orientation/turn_first | 30/30 | 0.535 | 0.98x | 6.236 |
| portfolio/baseline | 30/30 | 0.380 | 1.00x | 6.234 |
| portfolio/portfolio4 | 30/30 | 0.388 | 1.55x | 6.240 |
| portfolio/turn_first_portfolio4 | 30/30 | 0.390 | 1.48x | 6.222 |
| rotation/baseline | 30/30 | 0.390 | 1.00x | 6.234 |
| rotation/rotation | 30/30 | 0.383 | 0.75x | 6.212 |
| rotation/rotation_turn_first | 30/30 | 0.387 | 0.72x | 6.238 |
| admission/baseline | 60/60 | 3.718 | 1.00x | 6.234 |
| admission/orientation_admission | 60/60 | 3.725 | 1.21x | 6.755 |

Four `plannerTimes` entries in the admission matrix exceed the nominal 1,000 ms decision limit: 3.725 s and 3.718 s on two separate WAREHOUSE processes, and 1.183 s / 1.231 s on another WAREHOUSE/SORTATION pair. The corresponding log timestamps show each pause after the `planner returns` record, at nearly the same wall-clock time in the two processes. `BaseSystem::plan` joins the planner thread before this log call; its caller measures elapsed time after that call returns. The entry's final deadline check occurs inside the already completed planner thread. Thus these outliers include time after CGAR planning finished. Logging or a host pause is the supported interpretation; the precise system cause was not instrumented.

The repeat reserves 12 physical cores on GRID and runs both profiles on WAREHOUSE and SORTATION for seeds 3–5. All 12 reproduce the original full trajectories exactly; the largest wrapper measurement is 0.593 s. Original measurements remain in every timing table and runtime ratio. The strict CGAR timeout path is unchanged, and no partial plan was accepted to recover performance. [Timing excerpts and comparisons](results/timing-audit.json), [repeat metrics](results/timing-repeat/metrics.json).

Orientation tests compare every source/orientation/goal in a small topology against an independent forward action search, check cache eviction/retention, explicit deadline failure and diagnostic attribution of a turn dependency. Existing safety, scheduling and liveness-mechanism fixtures pass with the new guidance enabled. These are regression checks, not a new end-to-end liveness proof.

```sh
python3 experiments/assignment-20260918/build.py --output runs/new-motion-build
# After its GRID build finishes:
python3 tools/benchmark_matrix.py --output runs/new-motion-matrix \
  --binary runs/new-motion-build/lifelong \
  --source-manifest runs/new-motion-build/build.json \
  --variants experiments/motion-20260918/admission-variants.json \
  --seeds 0 1 2 3 4 5 --parallel-suites 6 --jobs-per-suite 5
python3 experiments/motion-20260918/submit_analysis.py \
  --input runs/new-motion-matrix --output runs/new-motion-analysis --hold-job JOB_ID
```

Snapshot drivers link the original planner objects while replacing the simulator main with [snapshot_driver.cpp](snapshot_driver.cpp). Its action validator rejects obstacles, wrapped rows, duplicate destinations and edge swaps before applying a joint action. [snapshot_jobs.py](snapshot_jobs.py) records source/object/binary hashes and allocates the requested reference CPUs. The frozen inputs are retained under `snapshots/`; rebuilding the drivers uses those committed inputs and does not require historical raw trajectories. Configure both `cgar/build` and `nms/build` with the repository environment first; the snapshot build refreshes their planner objects before linking. For example, after activating the environment and configuring CGAR as in the root README:

```sh
cmake -S nms -B nms/build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$CONDA_PREFIX" -DPYTHON=OFF
python3 experiments/motion-20260918/snapshot_jobs.py build \
  --output runs/new-snapshot-build
# After build-completion.json reports success:
python3 experiments/motion-20260918/snapshot_jobs.py run \
  --build-dir runs/new-snapshot-build --output runs/new-snapshots
python3 experiments/motion-20260918/build_uniform_reference.py \
  --base runs/new-snapshot-build --output runs/new-uniform-build
# After the uniform build completes:
python3 experiments/motion-20260918/snapshot_jobs.py run \
  --build-dir runs/new-uniform-build --output runs/new-uniform-snapshots
```

`python3 experiments/motion-20260918/report.py` regenerates this report and its comparison tables from committed compact results. Rebuilt supplied and uniform drivers also pass a RANDOM-01 snapshot smoke check ([evidence](results/reproducer-validation.json)).

Binaries, full trajectories and raw logs remain ignored under `runs/`. Each build/run directory must be new; GRID submission commands return before the scheduled job finishes.
