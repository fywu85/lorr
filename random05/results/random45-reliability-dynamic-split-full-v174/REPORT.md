# Selected PILOT throughput variability

Frozen planner seeds0–9 on each archived instance; one observation per seed.
All successful full runs pass independent replay, strict1,000ms entries,
30-second preparation and32GB. Original binaries/settings are fixed per
instance. Historical matching observations are reused; exact repeats are
not extra samples. No algorithm or parameter search was performed.

**Mean ± SD uses successful full runs only.** SD is the sample standard
deviation with denominator n−1. Failed attempts remain failures and have no
throughput observation. Where failures occur, these statistics describe the
successful subset, not the unconditional ten-seed distribution.

| Instance | Valid / attempted | Mean ± sample SD | Range | Best | max(NMS, KK) | Mean lead | Best lead |
|---|---:|---:|---:|---:|---:|---:|---:|
| RANDOM-01 | 10/10 | 738.7 ± 3.2 | 735–745 | 745 | 692 | +6.75% | +7.66% |
| RANDOM-02 | 10/10 | 1394.9 ± 6.0 | 1386–1408 | 1408 | 1256 | +11.06% | +12.10% |
| RANDOM-03 | 10/10 | 2616.5 ± 14.7 | 2598–2646 | 2646 | 2359 | +10.92% | +12.17% |
| RANDOM-04 | 10/10 | 2773.2 ± 15.1 | 2754–2799 | 2799 | 2649 | +4.69% | +5.66% |
| RANDOM-05 | 10/10 | 4230.1 ± 46.5 | 4155–4302 | 4302 | 3172 | +33.36% | +35.62% |

## Individual seeds

| Instance | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| RANDOM-01 | 740 | 739 | 742 | 739 | 745 | 735 | 736 | 737 | 739 | 735 |
| RANDOM-02 | 1396 | 1391 | 1408 | 1393 | 1392 | 1395 | 1391 | 1386 | 1398 | 1399 |
| RANDOM-03 | 2611 | 2606 | 2611 | 2646 | 2610 | 2633 | 2620 | 2626 | 2598 | 2604 |
| RANDOM-04 | 2799 | 2771 | 2761 | 2761 | 2766 | 2789 | 2763 | 2790 | 2754 | 2778 |
| RANDOM-05 | 4302 | 4250 | 4276 | 4224 | 4251 | 4204 | 4155 | 4249 | 4162 | 4228 |

## Timing failures

Every declared seed completes within the strict limit.

## Allocation refusals

3 original allocations were refused before any solver launch. Each has
one predeclared retry with the identical binary, seed, settings, inputs and
limits. Refusals remain in the original audit and are not throughput
observations or extra planner seeds. The table counts the resulting full
solver attempt once per seed.

- RANDOM-04 seed7: originaltrick-random-04-reliability-dynamic-seed7, retrytrick-random-04-reliability-dynamic-seed7-allocation-retry1; {'kind': 'core_binding', 'expected': 16, 'observed': 64}.
- RANDOM-04 seed8: originaltrick-random-04-reliability-dynamic-seed8, retrytrick-random-04-reliability-dynamic-seed8-allocation-retry1; {'kind': 'core_binding', 'expected': 16, 'observed': 64}.
- RANDOM-04 seed9: originaltrick-random-04-reliability-dynamic-seed9, retrytrick-random-04-reliability-dynamic-seed9-allocation-retry1; {'kind': 'core_binding', 'expected': 16, 'observed': 64}.

These are descriptive statistics for development-selected profiles. Some
reused seeds helped select those profiles. They do not establish unseen-map
or fresh-task-stream variability, nor a statistical confidence claim against
the maximum of the NMS/KK observations. General solvers were not re-evaluated.

This report describes this frozen measurement batch only. Any later
reliability repair is evaluated separately; failed attempts remain here.

[Frozen protocol](../../experiments/random12345-final-reliability-seeds10-v174-protocol.json), [independent audit](audit.json), [full numeric statistics](seed-statistics.json).

## Runtime repair and full-run verification

The final cohort contains 50 full runs and 50,000 planning calls: ten distinct
planner seeds per instance. RANDOM-01/02 reuse their already valid frozen
measurements; RANDOM-03 uses source173 and RANDOM-04/05 use source174.
Every selected solver run passes independent movement, collision, assignment,
task-event and waiting-accounting replay, the strict 1,000 ms entry limit,
30-second preparation limit and 32 decimal GB resource guard.

| Instance | Worst step over ten seeds (ms) | Peak RSS over ten seeds (MB) |
|---|---:|---:|
| RANDOM-01 | 224.043 | 405.119 |
| RANDOM-02 | 780.246 | 376.214 |
| RANDOM-03 | 772.972 | 460.857 |
| RANDOM-04 | 765.902 | 500.015 |
| RANDOM-05 | 885.879 | 453.771 |

All runs use 32 workers on 16 bound physical EPYC9354 cores, with no CPU quota.
This is measured reliability on these allocations, not a guarantee under arbitrary host contention.

The runtime changes are portable IPO, exact vectorized Hungarian scans, a
synchronous persistent Entry worker, and explicit OpenMP binding. RANDOM-04/05
also distribute the same independent work dynamically; RANDOM-05 uses passive
OpenMP waiting and skips redundant matching bookkeeping. Fixed budgets,
objectives, seeds, horizons and strict deadlines are unchanged. No partial
search or assignment is returned on a deadline.

All 29 available earlier successful full trajectories for the repaired
RANDOM-03/04/05 cohorts match all six compared fields exactly. RANDOM-04 seed8
is its first successful full run and has independent replay but no previous
successful full trajectory for an equivalence comparison.

[RANDOM-03 equivalence](../random345-reliability-persistent-split-full-v173/control-equivalence.json),
[17 main RANDOM-04/05 comparisons](control-equivalence.json),
[two allocation-retry comparisons](../random04-dynamic-allocation-retry1-split-v174/control-equivalence.json).

The original seed batch and unsuccessful repairs remain separate evidence:
[original five timeouts](../random12345-wrapup-seeds10-split-full/REPORT.md),
[IPO/binding attempt](../random345-reliability-bound-split-full-v171/REPORT.md),
[persistent-worker attempt](../random345-reliability-persistent-split-full-v173/REPORT.md).
The last attempt qualifies RANDOM-03 only; its RANDOM-04/05 failures are not erased.

Exact Hungarian, worker-lifetime and static/dynamic equivalence regressions
pass in the IPO builds. Source173 tests take 69.58 seconds; source174 tests
take 64.35 seconds. Source and binary hashes are pinned in the build archives.

No throughput research or fresh-input generation was performed during this
bounded repair. The campaign is paused at the user's request after logging,
committing and pushing the completed work.
