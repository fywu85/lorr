# RANDOM-04 compressed matching qualification

All six extension attempts are complete and independently audited. Three complete the full 1,000-step run with legal movement and assignments; three fail the strict one-second entry limit at startup. Source, input, affinity and memory checks pass. The failed runs have no valid throughput score.

Both recipes use source166, optional-column compression and idle price 32. The candidate uses whole-chain length weight 0.5; the control uses 0.25. The original seed 0/1 pairs were frozen in the preceding runtime batch.

| Planner seed | Control tasks | Candidate tasks | Control maximum ms | Candidate maximum ms |
|---|---:|---:|---:|---:|
| 0 | 2776 | 2799 | 855.101 | 848.301 |
| 1 | 2795 | 2771 | 763.114 | 859.270 |
| 2 | FAIL | FAIL | 1039.572 (timeout) | 1144.865 (timeout) |
| 3 | 2776 | 2761 | 972.860 | 852.734 |
| 4 | FAIL | 2766 | 1041.974 (timeout) | 850.573 |

The three completed pairs (seeds 0, 1 and 3) total **8,331 candidate versus 8,347 control**, **-0.1917%**: one gain and two losses. This incomplete subtotal does not establish a five-seed gain; it also does not erase the failures. The candidate seed-4 result of 2,766 has no valid matching control in this batch.

The selected maximum remains **2,799**, independently replayed and exactly reproduced. Neither its original configuration nor this compressed configuration is robust under the declared one-second limit. Compression alone is insufficient. Startup failures spend 495–593 ms in assignment and 464–497 ms in look-ahead, plus 46–54 ms in task-cost preparation. These phase wall times do not establish external contention as the cause.

Next work should reduce startup matching and search cost while preserving complete fixed-work semantics. No fresh RANDOM-04 task/start input was generated.

[Independent audit](audit.json), [paired accounting](paired-comparison.json), [startup failures](FAILURES.md), [preceding runtime comparison](../random04-chain-runtime-split-full-v166/REPORT.md).
