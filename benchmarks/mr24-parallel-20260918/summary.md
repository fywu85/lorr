# CGAR MR24 parallel evaluation — 2026-09-18

All ten production CGAR instances were launched concurrently using `--jobs 10`, at their archived horizons, with 1,000 ms per decision and 30,000 ms preprocessing. **10 of 10 runs completed their horizons with zero planner errors, scheduling errors, and entry timeouts.**

The executable and four recorded production source hashes match the [September 17 sequential run](../mr24-20260917/summary.md) exactly. The fallback-sampling and hybrid-scoring prototypes are not enabled. This is a new execution mode, not a planner implementation change.

**Resource context:** the ten jobs shared a one-CPU quota. They started together; the short instances finished first, leaving the longer instances to share the quota. Wall-clock budgets can change table construction and task allocation under contention. These single-run numbers measure this shared-resource parallel execution; they do not estimate ten-core scaling or establish a causal code improvement. A Fable CLI source review and lightweight reporting also overlapped part of the run.

| Instance | Steps | Draft tasks | Sequential CGAR | Parallel CGAR | Change vs sequential | Planner errors | Schedule errors | Timeouts | Wall seconds |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| WAREHOUSE | 5000 | 37812 | 26350 | 24241 | -8.0% | 0 | 0 | 0 | 1201.86 |
| SORTATION | 5000 | 62703 | 30922 | 30326 | -1.9% | 0 | 0 | 0 | 1255.02 |
| CITY-01 | 3000 | 5978 | 6235 | 5795 | -7.1% | 0 | 0 | 0 | 464.36 |
| CITY-02 | 3000 | 6690 | 6768 | 5765 | -14.8% | 0 | 0 | 0 | 611.57 |
| GAME | 5000 | 5159 | 5761 | 2082 | -63.9% | 0 | 0 | 0 | 856.41 |
| RANDOM-01 | 600 | 443 | 457 | 457 | +0.0% | 0 | 0 | 0 | 8.69 |
| RANDOM-02 | 600 | 544 | 574 | 574 | +0.0% | 0 | 0 | 0 | 11.89 |
| RANDOM-03 | 800 | 636 | 645 | 645 | +0.0% | 0 | 0 | 0 | 18.49 |
| RANDOM-04 | 1000 | 655 | 634 | 634 | +0.0% | 0 | 0 | 0 | 34.29 |
| RANDOM-05 | 2000 | 542 | 1032 | 1032 | +0.0% | 0 | 0 | 0 | 58.08 |

Wall seconds are each process's elapsed time while competing with the other jobs; adding them does not give the suite's wall time. The longest process took 1255.02 seconds.

**RANDOM-05 retains the existing restricted capacity mode:** 31 robots park outside the core, 769 remain active, and tasks with outside stops are excluded. Simulator success does not establish end-to-end starvation freedom.

Reproduce from the workspace root, using a fresh output directory:

```sh
python3 tools/benchmark_cgar.py --output runs/cgar-parallel-repeat --jobs 10
```

[Machine-readable results](summary.json) · [CSV](summary.csv) · [Source, executable, configuration, and resource metadata](metadata.json)

The raw simulator outputs, logs, and executable snapshot remain locally in `runs/cgar-parallel-20260918` and are excluded from Git.

**GAME allocation diagnostic.** Both runs assign all 6,500 robots their first task at step 1. Mean first-pickup Manhattan distance rises from **2.76 cells sequentially to 284.10 cells in parallel**. All 6,500 first pickups are observed in the sequential trajectory, versus 5,303 in parallel; 1,197 remain unobserved at step 5,000. Mean delay among the observed pickups is 26.48 versus 1,811.33 steps; the latter excludes the unobserved pickups and is not an unconditional mean. This demonstrates a substantial initial allocation difference, while leaving its contribution relative to later routing/cache effects unisolated. See [cohort statistics](game-initial-cohorts.json) and the [offline analysis script](analyze_game_initial.py), which requires the preserved local raw trajectories.
