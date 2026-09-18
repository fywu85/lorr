# CGAR MR24 sequential repeat — 2026-09-18

All ten production CGAR instances ran one at a time with `--jobs 1`, in the same order as the [original sequential run](../mr24-20260917/summary.md). The executable SHA-256, four recorded source hashes, input paths, horizons, and configuration match that run. No `CGAR_*` overrides or experimental planner changes were enabled. Each decision has a 1,000 ms limit, with 30,000 ms preprocessing.

**10 of 10 runs completed their horizons with zero planner errors, scheduling errors, and entry timeouts.**

**Resource context:** the account remains limited to one CPU in total, shared with other processes. This run removes competition between CGAR instances, but does not give CGAR exclusive CPU access. The user chose to finish the run and document the contention. A sampler began during GAME, after the RANDOM cases completed. Over 30.6 minutes of intervals with an unchanged benchmark process, CGAR averaged 45.6% of one CPU (interval range 23.4%–90.2%), while the entire account cgroup averaged 100.0%. See [CPU observations](cpu-observations.json) for the measured window and exclusions. These numbers are not a measurement of 32-core or ten-core performance.

| Instance | Steps | Original sequential | Ten-way parallel | Sequential repeat | Repeat vs original | Repeat vs parallel |
|---|---:|---:|---:|---:|---:|---:|
| RANDOM-01 | 600 | 457 | 457 | 457 | +0.0% | +0.0% |
| RANDOM-02 | 600 | 574 | 574 | 574 | +0.0% | +0.0% |
| RANDOM-03 | 800 | 645 | 645 | 645 | +0.0% | +0.0% |
| RANDOM-04 | 1000 | 634 | 634 | 634 | +0.0% | +0.0% |
| RANDOM-05 | 2000 | 1032 | 1032 | 1032 | +0.0% | +0.0% |
| GAME | 5000 | 5761 | 2082 | 5104 | -11.4% | +145.1% |
| CITY-01 | 3000 | 6235 | 5795 | 6215 | -0.3% | +7.2% |
| CITY-02 | 3000 | 6768 | 5765 | 6725 | -0.6% | +16.7% |
| WAREHOUSE | 5000 | 26350 | 24241 | 25838 | -1.9% | +6.6% |
| SORTATION | 5000 | 30922 | 30326 | 30891 | -0.1% | +1.9% |

The large parallel loss was substantially reduced by running sequentially, but the repeat did not exactly recover the original result under continued contention. GAME's first-assignment cohort illustrates why: mean Manhattan distance to the first pickup was 2.76 cells originally, 284.10 during the shared-quota parallel run, and 51.39 in this sequential repeat. All 6,500 robots were assigned on step 1 in each run. In the parallel run, 1,197 first pickups remained unobserved by the horizon, so observed delay averages exclude censored pickups. See the [cohort analysis](game-initial-cohorts.json). These observations identify changed initial allocations, without isolating their entire causal effect from distance-cache and routing changes.

Task counts above compare the same production implementation. A single sequential repeat under shared CPU load does not isolate all causes of a difference: wall-clock deadlines can still change search completion, distance-table construction, and task allocation.

| Instance | Original sequential wall seconds | Parallel wall seconds | Repeat wall seconds |
|---|---:|---:|---:|
| RANDOM-01 | 0.76 | 8.69 | 1.82 |
| RANDOM-02 | 0.96 | 11.89 | 3.21 |
| RANDOM-03 | 1.80 | 18.49 | 5.80 |
| RANDOM-04 | 3.40 | 34.29 | 13.88 |
| RANDOM-05 | 7.01 | 58.08 | 21.01 |
| GAME | 285.82 | 856.41 | 586.31 |
| CITY-01 | 85.48 | 464.36 | 112.63 |
| CITY-02 | 135.80 | 611.57 | 222.61 |
| WAREHOUSE | 397.74 | 1201.86 | 531.49 |
| SORTATION | 440.34 | 1255.02 | 607.51 |

Repeat process durations sum to 2106.28 seconds (35.10 minutes), excluding small runner overhead. Parallel durations overlap and must not be summed as suite elapsed time.

**RANDOM-05 retains the existing restricted capacity mode:** 31 robots park outside the core, 769 remain active, and tasks with outside stops are excluded. Simulator success does not establish end-to-end starvation freedom.

Reproduce from the workspace root with no `CGAR_*` environment overrides and a fresh output directory:

```sh
python3 tools/benchmark_cgar.py --output runs/cgar-sequential-repeat-new --jobs 1 \
  --instances RANDOM-01 RANDOM-02 RANDOM-03 RANDOM-04 RANDOM-05 \
  GAME CITY-01 CITY-02 WAREHOUSE SORTATION
```

[Machine-readable results](summary.json) · [CSV](summary.csv) · [Metadata](metadata.json) · [Parallel comparison](../mr24-parallel-20260918/summary.md)

The raw simulator outputs, logs, executable snapshot, and CPU samples remain locally in `runs/cgar-sequential-repeat-20260918` and are excluded from Git.
