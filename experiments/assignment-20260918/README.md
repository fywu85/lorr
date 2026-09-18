# NMS-inspired task assignment experiments, 18 September 2026

Implemented global pickup weighting and bounded swaps of unopened tasks. All 360 full-horizon runs completed with zero planner errors, schedule errors or timeouts. The changes are available as independent switches; the production defaults remain `CGAR_PICKUP_WEIGHT=1`, `CGAR_REASSIGN=0`, and `CGAR_HRRN=1`. There is no map-specific CGAR configuration.

## Results and default decision

Every comparison pairs seeds 0, 1 and 2 across all ten archived horizons. Tables report the arithmetic mean of the three paired percentage changes, not a confidence interval or official competition score. Per-seed counts and ranges are retained in [effects.json](results/effects.json); per-matrix metrics retain completion curves and trajectory hashes.

| Instance | Pickup weight 2 | Pickup weight 5 | Reassignment | Reassignment + weight 5 |
|---|---:|---:|---:|---:|
| WAREHOUSE | -0.6% | +0.6% | +1.0% | -2.3% |
| SORTATION | -3.8% | -6.5% | -2.6% | -5.7% |
| CITY-01 | +0.8% | +0.0% | +1.9% | -0.1% |
| CITY-02 | +0.1% | -0.6% | +1.4% | -0.2% |
| GAME | -0.8% | -1.2% | -0.0% | -1.7% |
| RANDOM-01 | -0.0% | +0.2% | -0.4% | +1.5% |
| RANDOM-02 | -3.3% | -1.9% | -0.9% | -3.4% |
| RANDOM-03 | +1.5% | -2.5% | +1.8% | -1.3% |
| RANDOM-04 | -0.1% | -1.9% | +1.0% | -2.9% |
| RANDOM-05 | +0.4% | -0.2% | +1.6% | +1.8% |

Weight 5 with the existing HRRN objective reduces SORTATION throughput in every seed (-10.1% to -3.6%). Reassignment alone changes SORTATION by -7.8% to +1.8%, so the seed-0 gain did not repeat. These results do not justify enabling either setting by default.

NMS ranks by weighted travel cost directly. To test that interaction, a third matrix disables HRRN while retaining CGAR's independent oldest-task admission. This configuration still uses a single global weight 5. The direct-cost profile is a throughput/waiting-time tradeoff; it is not enabled by default. One final global weight-10 setting tests whether greater pickup emphasis improves that tradeoff.

| Instance | Direct cost, weight 5 | Direct cost, weight 5 + reassignment | Direct cost, weight 10 |
|---|---:|---:|---:|
| WAREHOUSE | +1.8% | +0.2% | +1.4% |
| SORTATION | -5.7% | -5.8% | -10.1% |
| CITY-01 | +6.3% | +6.9% | +7.1% |
| CITY-02 | +10.3% | +10.1% | +9.5% |
| GAME | +2.9% | +2.8% | +3.2% |
| RANDOM-01 | +6.8% | +7.4% | +9.6% |
| RANDOM-02 | +7.5% | +7.5% | +7.8% |
| RANDOM-03 | +11.5% | +12.5% | +9.4% |
| RANDOM-04 | +4.4% | +3.4% | +7.0% |
| RANDOM-05 | +7.7% | +9.2% | +8.4% |

Direct cost with weight 5 improves the three-seed mean on nine of ten instances, including CITY-01 (+6.3%), CITY-02 (+10.3%) and RANDOM-03 (+11.5%). SORTATION loses 5.7% on average and its unfinished-task age p90 rises from 1,660–1,955 to 4,508–4,600 steps. Weight 10 increases the SORTATION loss to 10.1%. The useful outcome is an optional global throughput profile with explicit costs; the conservative default is retained.

The previous study already found gains on the eight smaller instances from disabling HRRN alone. The new weight-5 result should not be credited with all those gains. Compared with that earlier direct-cost weight-1 policy, weight 5 adds about 2.0% on WAREHOUSE and 9.3% on SORTATION (three-seed means), although SORTATION remains below the HRRN baseline. [Comparison with the prior controlled runs](results/direct-cost-weight-comparison.json).

To run that profile on reserved cores:

```sh
python3 tools/benchmark_gridengine.py --output runs/cgar-direct-cost --jobs 10 \
  --env CGAR_HRRN=0 --env CGAR_PICKUP_WEIGHT=5
```

These are adaptations of two NMS ideas inside CGAR, not a port of NMS's complete scheduler. NMS also maintains broader sorted candidate lists and couples its choices to a different, parallel motion planner. A weak or negative result here does not isolate the value of those mechanisms in NMS.

## Pickup travel and waiting

Completed-task throughput is the adoption criterion. Reducing an estimated pickup distance or even total empty movement does not by itself establish a gain: congestion, the selected task mix, and unfinished tasks also matter. The following counts come from actual simulator actions and pickup events.

| SORTATION, seed | Policy | Tasks | Empty forward moves | Mean time from first assignment to observed pickup | Unfinished-task age p90 |
|---|---|---:|---:|---:|---:|
| 0 | baseline | 63217 | 7306875 | 162.0 | 1955 |
| 0 | weight5 | 60913 | 6413393 | 140.6 | 2039 |
| 0 | reassign | 64376 | 6831564 | 139.9 | 1944 |
| 0 | weighted_cost | 60572 | 7006411 | 160.4 | 4600 |
| 1 | baseline | 67712 | 7578248 | 152.1 | 1774 |
| 1 | weight5 | 63775 | 6921336 | 153.2 | 1942 |
| 1 | reassign | 66479 | 7360338 | 156.6 | 1744 |
| 1 | weighted_cost | 64336 | 7561935 | 168.5 | 4508 |
| 2 | baseline | 68815 | 7683751 | 152.1 | 1660 |
| 2 | weight5 | 61873 | 6729740 | 152.0 | 2106 |
| 2 | reassign | 63474 | 7112842 | 162.0 | 1796 |
| 2 | weighted_cost | 63423 | 7527425 | 171.1 | 4539 |

Pickup delay is measured from the first assignment, including time spent with an earlier owner after a swap. It includes only tasks whose pickup was observed; the metrics also retain unpicked assignment ages and counts. Empty forward/turn/wait actions cover all assigned, unopened task intervals through the horizon, including unfinished pickups. Lower total movement with fewer completions is not automatically higher efficiency. Unfinished-task age includes every revealed unfinished task. In RANDOM-05 it includes tasks excluded by the existing capacity mode (31 parked robots), so this is not a claim of unrestricted service.

## Reassignment contract and checks

- Both tasks must be unstarted and already assigned. A swap retains both assignments; it never drops a task or changes its reveal time.
- Each task can change robot at most once before pickup; each robot has a 20-step cooldown.
- Current/next pending primary tickets, recovery participants and reserved cells, pocket service, unfinished turns/moves, started tasks and fairness-admitted tasks are protected.
- Both robots and pickups must be in the core. Existing capacity eligibility remains enforced for every remaining task stop.
- Every ten steps, process at most 256 eligible sources. Each source gets at most 16 local candidates from a 2,048-node search and 16 global samples. Accept only savings of at least four estimated moves and 10% of the old pair cost.
- Use cached tables for all four distances if both goal tables exist, otherwise Manhattan for all four. Build no speculative tables and leave routing-cache recency unchanged.
- Prescribed work must complete before the shared deadline. Failure raises a timeout; elapsed time never truncates a successful pass.

Regression fixtures verify a swap reducing pickup distance from 20 to 2, a one-change cap, started-task protection, a minimum saving, primary protection while another pair swaps, unfinished-turn protection, recovery-witness protection, and preservation of a fair admission even when swapping would be cheaper. Weighting has a fixture that chooses a nearby longer task over a distant short task while preserving waiting-time priority. The full existing regression suite also passed. [Validation](results/swap-validation/validation.txt).

Actual assignment histories across all 360 runs contain no task reassigned after pickup and no task with more than one observed ownership change. Swaps made before a task's first simulator-visible assignment are absent from ownership-change counts; planner counters retain accepted swaps. These checks preserve the intended invariants but are not an end-to-end liveness proof. [Audit](results/reassignment-audit.json).

## Resources and reproducibility

Each matrix used an exclusive GRID allocation with up to six concurrent suites of five native instances, pinned to 30 distinct physical cores, unlimited CPU quota, and 8 GiB requested per slot. CGAR remains single-threaded per instance. GRID scales the process address-space limit with slots; this is not an independent 8 GiB limit for each child. CPU models differ across matrices, so wall-time differences between matrices are not implementation speedups.

| Matrix | CPU | Maximum decision seconds | Peak RSS GiB | Longest native run seconds |
|---|---|---:|---:|---:|
| weights | AMD EPYC 7532 32-Core Processor | 0.535 | 6.234 | 206.6 |
| swaps | AMD EPYC 9354 32-Core Processor | 0.395 | 6.234 | 150.6 |
| nms-cost | AMD EPYC 9354 32-Core Processor | 0.391 | 6.234 | 144.0 |
| cost10 | AMD EPYC 9354 32-Core Processor | 0.381 | 6.234 | 148.8 |

All 120 baseline instance/seed runs reproduce the prior committed baseline trajectories exactly. Repeated weight-5 runs agree across both builds/matrices in all 30 pairs. The equivalence hash covers paths, assignments, events and tasks, excluding runtime measurements. [Evidence](results/equivalence.json).

Full horizons were used because the preceding experiment demonstrated that a 1,000-step screen can reverse the eventual ranking. `metrics.json` retains completion curves and `paired-results.json` retains retrospective prefix cutoffs; they do not validate one universal short horizon for future assignment changes.

```sh
python3 experiments/assignment-20260918/build.py --output runs/new-assignment-build
# After the build and regression job completes:
python3 tools/benchmark_matrix.py --output runs/new-assignment-matrix \
  --binary runs/new-assignment-build/lifelong \
  --source-manifest runs/new-assignment-build/build.json \
  --variants experiments/assignment-20260918/reassign-variants.json \
  --seeds 0 1 2 --parallel-suites 6 --jobs-per-suite 5
# Analyze after completion, or use submit_analysis.py --hold-job JOB_ID:
python3 experiments/assignment-20260918/analyze.py \
  --input runs/new-assignment-matrix --output runs/new-assignment-analysis
```

The variant files cover weights, reassignment and direct-cost ranking. Weight 10 was added after the weight-5 direct-cost tradeoff emerged; it is exploratory tuning on the same ten instances, not a held-out confirmation. Source and binary hashes, build logs, allocation metadata and all compact results are retained. Binaries and raw trajectories remain in ignored `runs/`. The default remains unchanged while the measured alternatives are available for further work.
