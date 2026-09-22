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
| RANDOM-04 | 6/10 | 2775.0 ± 15.7 | 2761–2799 | 2799 | 2649 | +4.76% | +5.66% |
| RANDOM-05 | 8/10 | 4241.9 ± 43.2 | 4155–4302 | 4302 | 3172 | +33.73% | +35.62% |

## Individual seeds

| Instance | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| RANDOM-01 | 740 | 739 | 742 | 739 | 745 | 735 | 736 | 737 | 739 | 735 |
| RANDOM-02 | 1396 | 1391 | 1408 | 1393 | 1392 | 1395 | 1391 | 1386 | 1398 | 1399 |
| RANDOM-03 | 2611 | 2606 | 2611 | 2646 | 2610 | 2633 | 2620 | 2626 | 2598 | 2604 |
| RANDOM-04 | 2799 | 2771 | 2761 | FAIL | 2766 | FAIL | 2763 | 2790 | FAIL | FAIL |
| RANDOM-05 | 4302 | 4250 | 4276 | 4224 | 4251 | FAIL | 4155 | 4249 | FAIL | 4228 |

## Timing failures

- RANDOM-05 seed5: resource_preflight. [Original attempt](trick-random-05-reliability-persistent-seed5/summary.json).
- RANDOM-05 seed8: compute step1: 1127.361ms. [Original attempt](trick-random-05-reliability-persistent-seed8/summary.json).
- RANDOM-04 seed3: resource_preflight. [Original attempt](trick-random-04-reliability-persistent-seed3/summary.json).
- RANDOM-04 seed5: compute step0: 1024.577ms. [Original attempt](trick-random-04-reliability-persistent-seed5/summary.json).
- RANDOM-04 seed8: compute step0: 1083.437ms. [Original attempt](trick-random-04-reliability-persistent-seed8/summary.json).
- RANDOM-04 seed9: resource_preflight. [Original attempt](trick-random-04-reliability-persistent-seed9/summary.json).

These are descriptive statistics for development-selected profiles. Some
reused seeds helped select those profiles. They do not establish unseen-map
or fresh-task-stream variability, nor a statistical confidence claim against
the maximum of the NMS/KK observations. General solvers were not re-evaluated.

This report describes this frozen measurement batch only. Any later
reliability repair is evaluated separately; failed attempts remain here.

[Frozen protocol](../../experiments/random12345-reliability-persistent-seeds10-v173-protocol.json), [independent audit](audit.json), [full numeric statistics](seed-statistics.json).
