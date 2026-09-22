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
| RANDOM-03 | 9/10 | 2616.1 ± 15.6 | 2598–2646 | 2646 | 2359 | +10.90% | +12.17% |
| RANDOM-04 | 9/10 | 2775.3 ± 14.3 | 2761–2799 | 2799 | 2649 | +4.77% | +5.66% |
| RANDOM-05 | 7/10 | 4238.4 ± 46.5 | 4162–4302 | 4302 | 3172 | +33.62% | +35.62% |

## Individual seeds

| Instance | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| RANDOM-01 | 740 | 739 | 742 | 739 | 745 | 735 | 736 | 737 | 739 | 735 |
| RANDOM-02 | 1396 | 1391 | 1408 | 1393 | 1392 | 1395 | 1391 | 1386 | 1398 | 1399 |
| RANDOM-03 | 2611 | 2606 | 2611 | 2646 | 2610 | 2633 | FAIL | 2626 | 2598 | 2604 |
| RANDOM-04 | 2799 | 2771 | 2761 | 2761 | 2766 | 2789 | 2763 | 2790 | FAIL | 2778 |
| RANDOM-05 | 4302 | 4250 | 4276 | 4224 | 4251 | 4204 | FAIL | FAIL | 4162 | FAIL |

## Timing failures

- RANDOM-05 seed6: compute step0: 1003.563ms. [Original attempt](trick-random-05-wrapup-seed6/summary.json).
- RANDOM-05 seed7: compute step0: 1157.953ms. [Original attempt](trick-random-05-wrapup-seed7/summary.json).
- RANDOM-05 seed9: compute step0: 1165.669ms. [Original attempt](trick-random-05-wrapup-seed9/summary.json).
- RANDOM-04 seed8: compute step0: 1096.994ms. [Original attempt](trick-random-04-wrapup-seed8/summary.json).
- RANDOM-03 seed6: compute step0: 1214.267ms. [Original attempt](trick-random-03-wrapup-seed6/summary.json).

These are descriptive statistics for development-selected profiles. Some
reused seeds helped select those profiles. They do not establish unseen-map
or fresh-task-stream variability, nor a statistical confidence claim against
the maximum of the NMS/KK observations. General solvers were not re-evaluated.

This report describes this frozen measurement batch only. Any later
reliability repair is evaluated separately; failed attempts remain here.

[Frozen protocol](../../experiments/random12345-wrapup-seeds10-protocol.json), [independent audit](audit.json), [full numeric statistics](seed-statistics.json).
