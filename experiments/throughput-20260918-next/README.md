# Scheduler and distance-consistency experiments, 18 September 2026

The first follow-up batch did not establish a new default that reliably improves throughput. Keep HRRN enabled and 64 fallback samples. The three new distance-consistency switches remain experimental and default off. No map-specific CGAR policy was introduced. See [NMS transfer priorities](NMS-transfer.md) for the next assignment and planning experiments.

All 120 short scheduler runs, 90 full scheduler runs and 150 full distance runs completed with zero planner errors, schedule errors or timeouts. The new binary with all switches off reproduces the old baseline trajectory in all 30 instance/seed pairs, including paths, assignments, task events and revealed tasks. [Equivalence evidence](results/baseline-equivalence.json).

## Full-horizon effects

Changes below are relative to the repaired baseline at `87322af`, paired by seeds 0, 1 and 2. Ranges show the smallest and largest paired completion change; they are not confidence intervals. Each instance uses its archived horizon.

| Instance | HRRN disabled | Samples 64 → 256 | Refine chain costs |
|---|---:|---:|---:|
| WAREHOUSE | -2.2 to +1.0% | -1.9 to +4.1% | +0.0 to +0.0% |
| SORTATION | -17.7 to -11.4% | -13.9 to -5.7% | +0.0 to +0.0% |
| CITY-01 | +3.4 to +7.8% | +0.0 to +0.0% | -0.8 to +2.2% |
| CITY-02 | +6.3 to +8.9% | +0.0 to +0.0% | -2.9 to +0.6% |
| GAME | +1.0 to +5.2% | +0.0 to +0.0% | -1.3 to +0.6% |
| RANDOM-01 | +3.7 to +11.5% | +0.0 to +0.0% | -4.2 to +1.8% |
| RANDOM-02 | +8.0 to +11.3% | +0.0 to +0.0% | -3.0 to +2.7% |
| RANDOM-03 | +6.4 to +16.1% | +0.0 to +0.0% | -8.2 to +1.1% |
| RANDOM-04 | +3.9 to +8.2% | +0.0 to +0.0% | -1.4 to +0.9% |
| RANDOM-05 | +5.5 to +8.5% | +0.0 to +0.0% | -4.7 to +3.3% |

Disabling HRRN improves many smaller instances but reduces full SORTATION completions by 11.4–17.7%. Its outstanding-task age at the 90th percentile rises from 1,660–1,955 to 4,630–4,710 steps on SORTATION, and from 3,098–3,446 to 5,000 on WAREHOUSE. Increasing samples to 256 also reduces SORTATION throughput. Those changes are not promoted. Task-age figures include every revealed unfinished task, including capacity-excluded tasks on RANDOM-05; they are descriptive and do not establish starvation freedom.

Refining chain costs produces mixed effects on smaller instances and no completion change on WAREHOUSE or SORTATION. In seed 0, the last SORTATION counter at step 4,800 reports 1,689 refined legs and zero changed scalar costs: the original approximations already had the same numerical value. WAREHOUSE reports 2,168 refined legs but only two changed costs. Improved estimate provenance alone cannot close this throughput gap.

`peek`: 29/30 trajectories identical to baseline; completion changes range from -8.66% to +0.00%.

`stable_stall`: 30/30 trajectories identical to baseline; completion changes range from +0.00% to +0.00%.

`combined`: 5/30 trajectories identical to baseline; completion changes range from -8.66% to +3.29%.

The switches are independent: `CGAR_REFINE_CHAIN_COSTS`, `CGAR_SCHEDULER_CACHE_PEEK`, and `CGAR_STABLE_STALL_BASIS`. Refinement upgrades only cached approximate legs, retains scalar values after table eviction, and invalidates changed task epochs. Peek avoids scheduler-driven LRU promotion. Stable stall observations reset the measurement window when the potential changes while preserving the fair-primary ticket. Unit tests cover these failure modes; all existing regression fixtures also pass with all three switches enabled. [Build/test log](results/validation/validation.txt), [source and binary hashes](results/validation/source-hashes.json).

## How short can evaluation be?

A 1,000-step cap is a fast screen, not a reliable universal prediction of final throughput. On SORTATION, disabling HRRN looks 2.36–2.37% better at 1,000 steps but is 11.4–17.7% worse by 5,000. The task population and congestion evolve; large early completion counts do not remove that bias. The previous fallback repair calibration was specific to that repair.

We recalibrated on a 25-step grid. The directional criterion requires at least 100 completions in both policies for every seed and the same full-run classification (above +1%, below −1%, or between) at that prefix and every later tested prefix. The stricter criterion also stays within five percentage points of the full effect. These are retrospective cutoffs for tested changes, not guarantees for future mechanisms.

| Instance | Full steps | HRRN directional cutoff, all 3 seeds | Within 5 pp |
|---|---:|---:|---:|
| WAREHOUSE | 5000 | 4975 | 4975 |
| SORTATION | 5000 | 1775 | 3100 |
| CITY-01 | 3000 | 400 | 525 |
| CITY-02 | 3000 | 500 | 900 |
| GAME | 5000 | 4925 | 4925 |
| RANDOM-01 | 600 | 175 | 475 |
| RANDOM-02 | 600 | 150 | 475 |
| RANDOM-03 | 800 | 300 | 775 |
| RANDOM-04 | 1000 | 625 | 650 |
| RANDOM-05 | 2000 | 625 | 1475 |

Use [screen-1000.json](screen-1000.json) for quick runtime/correctness and early-quality checks. Promote candidate defaults only after paired full-horizon validation. Some small instances can be screened much earlier; WAREHOUSE and GAME can require nearly their full horizons under this criterion. [All calibrated cutoffs](results/horizons.json), [raw compact curves](results/scheduler-full/metrics.json).

## Reconstructed original CGAR core

The preserved pre-fix `cgar.cpp` and `cgar.hpp` were rebuilt with the earliest recorded wrappers from commit `c2f0e5b` and the existing simulator objects. The original complete executable and wrappers were not preserved. This is a core reconstruction, not an exact replay of the historical draft binary. Its source files and build recipe are retained in `draft-source/` and `build_draft.py`.

Both runs used reserved physical cores and 1,000 ms decisions on EPYC 9354 hosts. The reconstructed draft had ten concurrent instances; the baseline was in the 30-core matrix. The draft still contains deadline-sensitive policies, so its behavior may vary with execution conditions.

| Instance | Reconstructed draft, seed 0 | Repaired baseline, seed 0 | Change |
|---|---:|---:|---:|
| WAREHOUSE | 37812 | 36353 | -3.9% |
| SORTATION | 67149 | 63217 | -5.9% |
| CITY-01 | 5978 | 6053 | +1.3% |
| CITY-02 | 6690 | 6637 | -0.8% |
| GAME | 4532 | 5848 | +29.0% |
| RANDOM-01 | 443 | 457 | +3.2% |
| RANDOM-02 | 544 | 574 | +5.5% |
| RANDOM-03 | 636 | 645 | +1.4% |
| RANDOM-04 | 655 | 634 | -3.2% |
| RANDOM-05 | 542 | 1032 | +90.4% |

The remaining deficits on WAREHOUSE and SORTATION are real in this reconstruction comparison: baseline throughput has not fully recovered on every instance. RANDOM-05 is not an unrestricted gain: the current certified mode parks 31 robots and excludes tasks outside its supported core. The draft does not use the same restriction. [Provenance and comparison](results/draft/comparison.json).

## KittyKnight and NMS references

Each reference had an exclusive GRID allocation, 32 logical CPUs on 16 physical cores, 128 GiB requested memory, a 1,000 ms decision deadline and the documented 30-minute preprocessing allowance. CGAR uses one physical core and 30 seconds preprocessing, which it satisfies. These are resource-aware reference measurements, not official competition scores: the cluster mixes EPYC 7532 and 9354 CPUs, the original competition used different hardware, and each leader was run once with its supplied configuration. Leader map-specific policies remain in their own sources; none were copied into CGAR.

All columns use the same horizon for each row, capped at 1,000 steps. A dagger marks a reference that completed the horizon but recorded a timeout; it is not a clean success and is retained rather than silently excluded.

| Instance | Steps | CGAR seed 0 | KittyKnight | NMS |
|---|---:|---:|---:|---:|
| WAREHOUSE | 1000 | 9281 | 30440 | 30460† |
| SORTATION | 1000 | 14140 | 30933 | 31164† |
| CITY-01 | 1000 | 2129 | 2823 | 2811 |
| CITY-02 | 1000 | 2801 | 5601 | 5584 |
| GAME | 1000 | 1979 | 5680 | 6063† |
| RANDOM-01 | 600 | 457 | 683 | 638 |
| RANDOM-02 | 600 | 574 | 1239 | 1226 |
| RANDOM-03 | 800 | 645 | 2056 | 2350 |
| RANDOM-04 | 1000 | 634 | 1462 | 2532 |
| RANDOM-05 | 1000 | 513 | 1098 | 1629 |

NMS recorded one timeout at timestep 0 on each of WAREHOUSE, SORTATION and GAME. Other recorded errors and any further failures are preserved in the [run summaries](results/leaders/run-summaries.json) and [timeout diagnostics](results/leaders/timeout-diagnostics.json). Short-horizon results establish a substantial gap on several instances; they do not establish a full-horizon ranking.

## Resources, reproducibility and retained evidence

CGAR matrices ran at most six suites of five single-threaded instances concurrently, each pinned to a distinct physical core. `exclusive=true` prevents other scheduled jobs sharing the host, and recorded cgroup CPU quotas were unlimited. Jobs still share host memory bandwidth and normal system activity. Grid Engine granted 8 GiB per slot; a 30-slot job has a 240 GiB process address-space limit, not a separate 8 GiB limit per child. The earlier strict study separately validated a one-slot 8 GiB run.

The scheduler matrix used an EPYC 9354 host; the distance matrix used EPYC 7532. Comparisons within each matrix use the same allocation. Cross-matrix elapsed times are not implementation speedups. Executable hashes and build manifests identify frozen binaries independently of later working-tree edits.

```sh
python3 tools/benchmark_matrix.py --output runs/new-estimate-matrix \
  --variants experiments/throughput-20260918-next/estimate-variants.json \
  --seeds 0 1 2 --parallel-suites 6 --jobs-per-suite 5
# Add --horizon-profile experiments/throughput-20260918-next/screen-1000.json for a short screen.
# For a frozen binary, also pass --binary PATH --source-manifest BUILD_JSON.
python3 experiments/throughput-20260918-next/analyze_matrix.py \
  --input runs/new-estimate-matrix --output runs/new-estimate-analysis
```

Build `lifelong` and `cgar_regression` before using the current executable. The reference launcher is `run_references.py`; the historical-core reconstruction uses `build_draft.py`. `analyze_matrix.py`, `package_references.py` and `report.py` produce the retained evidence and this report. Raw trajectories, binaries, generated leader assets and logs stay in ignored `runs/`. Compact results include every scheduled case, resource records and source hashes.
