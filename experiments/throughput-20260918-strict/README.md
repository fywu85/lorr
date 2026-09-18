# CGAR strict deadlines and fallback repair — 2026-09-18

CGAR now fails explicitly when a decision misses its wall-clock deadline. A fixed deterministic fallback sample improves sparse task allocation without map-specific rules. On matched reserved-core runs across three seeds, SORTATION improves by **61.3–83.1%** and WAREHOUSE by **11.4–18.6%**; the other eight instances retain their matched task counts. These are effects of the fallback repair against the strict-deadline control, not comparisons against a contended historical run.

## Implementation

The scheduler skips a second identical search after an empty result, replenishes candidates that other robots took, and compares 64 deterministic samples from unused eligible tasks with the oldest task. An O(1) removal array keeps sampling independent of already allocated tasks. Explicit fair admission and the separate PIBT random stream remain. Both policy modes use the same bookkeeping and counters; `CGAR_FALLBACK_REPAIR=0` disables the repair for matched controls. No map names, coordinates, lanes, endpoint patterns, or per-map weights were introduced.

Scheduling and planning now share the entry’s absolute deadline. Expiry in distance construction, scheduling, recovery or PIBT throws `cgar::Timeout`; `Entry::compute` logs the stage and exits with code 124. The benchmark records an invalid timeout with unavailable result counts. It no longer treats a time-truncated schedule, Manhattan substitution caused by elapsed time, partial sample, or budget-induced waiting plan as success. The competition simulator was not modified. Fixed count limits and their associated heuristic estimates remain the prescribed algorithm.

## Matched full-horizon results

Both modes use the same binary, fixed limits, instruments, 1,000 ms shared decision deadline, 30,000 ms preprocessing limit, and archived instance horizons. Seeds 0, 1 and 2 are paired. Seed 0 is repeated three times for each policy. Another seed-0 repaired suite uses 5,000 ms to check budget invariance: 110 normal full-horizon runs in total. Every run completed with zero simulator errors or timeouts.

| Instance | Seed 0 control | Seed 0 repair | Gain range across 3 seeds |
|---|---:|---:|---:|
| WAREHOUSE | 32,538 | 36,353 | +11.4% to +18.6% |
| SORTATION | 39,201 | 63,217 | +61.3% to +83.1% |
| CITY-01 | 6,053 | 6,053 | +0.0% |
| CITY-02 | 6,637 | 6,637 | +0.0% |
| GAME | 5,848 | 5,848 | +0.0% |
| RANDOM-01 | 457 | 457 | +0.0% |
| RANDOM-02 | 574 | 574 | +0.0% |
| RANDOM-03 | 645 | 645 | +0.0% |
| RANDOM-04 | 634 | 634 | +0.0% |
| RANDOM-05 | 1,032 | 1,032 | +0.0% |

[All paired seed results](results/paired-results.json) · [Run summaries and resource use](results/run-summaries.json)

The historical draft counts were 62,703 for SORTATION and 37,812 for WAREHOUSE. The repaired seed-0 counts reach that SORTATION level and remain about 3.9% below the WAREHOUSE count. Those historical runs used different hardware/resource conditions and are not a matched causal comparison. The gap to KittyKnight/NMS has not yet been remeasured. RANDOM-05 retains its existing restriction: 31 parked robots, 769 active robots, and tasks with outside-core stops excluded.

## What changed in task selection

For seed 0, the initial average Manhattan distance to pickup falls from 105.6 to 45.2 cells on SORTATION and from 78.2 to 39.7 on WAREHOUSE. Mean observed pickup delay falls from 502.5 to 162.0 steps and from 308.0 to 211.7 respectively. These delay averages exclude assigned tasks whose pickups were unobserved at the horizon; those counts fall from 4,854 to 2,594 and from 3,377 to 2,195.

| Instance / policy | Tasks in steps 1–1000 | 1001–2000 | 2001–3000 | 3001–4000 | 4001–5000 |
|---|---:|---:|---:|---:|---:|
| WAREHOUSE / control | 8391 | 7231 | 5965 | 5692 | 5259 |
| WAREHOUSE / repair | 9281 | 7872 | 7015 | 6170 | 6015 |
| SORTATION / control | 10048 | 7453 | 7435 | 7161 | 7104 |
| SORTATION / repair | 14140 | 13660 | 13353 | 11998 | 10066 |

The repair reduces redundant local-search work: at the last periodic log (step 4,800), visited candidate-search nodes fall from 81.1 million to 56.8 million on WAREHOUSE and from 118.2 million to 104.2 million on SORTATION. The latter also completes more tasks and makes more assignments. These counters stop before the final horizon. The improvement remains present in each 1,000-step completion window; it is not confined to initialization. Broad sample lookups can also change distance-cache recency, so this experiment estimates the implemented repair as a whole.

## CPU, RAM and deadline repeatability

CGAR currently computes serially. Each native instance was pinned to one distinct physical core on an exclusive GRID allocation (`research27`, AMD EPYC 7532). Thirty reserved cores ran six suites concurrently, five instances per suite. Affinity and ancestor cgroup checks found no CPU quota. The larger native runs used about 0.996–0.998 CPU seconds per wall second. Peak RSS across all full runs was 6.24 GiB, including CGAR, the simulator and output construction. The routine launcher now requests one physical core and 8 GiB per concurrent instance. A separate full SORTATION run under a measured 8 GiB address-space limit passed with 63,217 tasks.

The competition allowed 32 logical CPUs and 128 GB per submission. Extra cores do not accelerate this serial implementation; the smaller allocation is intentional. The CPU model differs from the competition VM. See the [GRID report](../../benchmarks/gridengine-20260918/README.md) for the earlier 9.95-core concurrency proof and resource details.

All three timing repetitions produced identical trajectories, schedules, task data and events for each policy and instance. The 1,000 ms and 5,000 ms repaired runs also matched exactly on all ten instances. The largest recorded simulator decision duration was 0.6953 seconds. A deliberate 1 ms decision failed at distance construction after about 1.057 ms, exited 124, and was classified as a timeout. These checks support the intended complete-or-timeout behavior; successful outputs no longer change with extra available time in the tested cases.

[Repeatability checks](results/repeatability.json) · [Budget invariance](results/budget-invariance.json) · [Timeout result](results/timeout-probe.json) · [Timeout diagnostic](results/timeout-diagnostic.txt) · [8 GiB check](results/memory8g-summary.json)

## Shorter iteration horizons

We derived cumulative completion counts from the full trajectories, rather than selecting a convenient early outcome. On a 25-step grid, the screening criterion requires at least 100 completions per policy and seed, and the same effect classification as the full run (+1% improvement, −1% regression, or within ±1%) for all three seeds at that prefix and every later tested prefix. A separate stricter criterion requires the gain estimate to stay within five percentage points of its full-run value.

| Instance with a measured nonzero effect | Earliest stable direction | Gain estimate within 5 percentage points | Full horizon |
|---|---:|---:|---:|
| WAREHOUSE | 700 | 2750 | 5000 |
| SORTATION | 425 | 2600 | 5000 |

The other eight instances show no effect from this repair, so they do not establish a minimum horizon capable of detecting other changes. The 425/700-step findings are specific to this measured comparison and three seeds; they are not universal performance guarantees or liveness tests. Recovery, routing and reassignment changes may need longer observation.

For routine screening, [screening-horizons.json](screening-horizons.json) caps every archived horizon at 1,000 steps using the same rule across all instances. This provides margin beyond the observed direction thresholds. Full archived horizons remain the acceptance check; use longer runs to estimate gain size.

We then ran all 60 shorter policy/seed/instance combinations explicitly. Every run passed, and its completed-task count exactly matched the corresponding full-run prefix. Across the six paired suites, the slowest native instance took 26.1–31.3 seconds, versus 177.9–196.2 seconds in the full runs (roughly 6–7 times faster); these comparisons exclude queue wait. The three-seed gains at 1,000 steps were about 8–11% on WAREHOUSE and 41–43% on SORTATION, preserving the improvement direction while understating the larger late SORTATION gains. See [short-run validation](results/screening-validation.json), [allocation](results/screening-allocation.json), and [configuration](results/screening-spec.json).

```sh
python3 tools/benchmark_gridengine.py --output runs/cgar-screen-new --jobs 10 \
  --horizon-profile experiments/throughput-20260918-strict/screening-horizons.json
```

[Calibration results](results/horizons.json) · [Completion curves, pickup delays and counters](results/metrics.json)

## Reproduction and provenance

Build the production executable and regression target, then run the matched matrix and analysis:

```sh
python3 experiments/throughput-20260918-strict/run_matrix.py \
  --output runs/cgar-strict-repeat
python3 experiments/throughput-20260918-strict/analyze.py \
  --input runs/cgar-strict-repeat \
  --output experiments/throughput-20260918-strict/repeated-results
```

Run analysis after the GRID matrix finishes, on allocated compute. The matrix can also take `--horizon-profile` to evaluate only the three-seed policy pairs at shorter horizons. The analysis script’s full-run mode expects the complete repeatability/budget matrix.

Binary SHA-256: `b3d21462746328334f6a3cdc1e296c15abc5de88f31e8ed46b753e44924b0fd5`. The run was prepared from a dirty workspace on parent commit `2fdc26ffb7008e613fa264be141fa766cd24cda6`; the recorded source hashes identify the tested changes. [Build validation](results/build-validation.txt), [build source hashes](results/build-source-hashes.json), [matrix specification](results/spec.json), [allocation](results/allocation.json), and [per-run metadata](results/run-metadata.json) are retained. Raw outputs, logs and executables remain in ignored `runs/`.

This implements the first measurement and fallback-repair stage of the [proposal](../throughput-20260918-fable/implementation-plan.md). Shared candidate discovery, objective changes, refinable costs, unopened-task reassignment, recovery concurrency and generic routing improvements remain separate proposed experiments.
