# All four pickup-weight-8 seeds exceed the NMS target

The additional full runs score **154,999 / 155,020** on solver seeds 1 / 3. Combined with seeds 0 / 2, the same p90 / weight-8 profile scores **155,120 / 154,999 / 155,056 / 155,020**, mean **155,048.75**, 253.75 above NMS 154,795. All four seeds exceed target. The single-run record remains 155,120.

| Solver seed | P90 / pickup weight 5 | P90 / pickup weight 8 | Change | Final-1,000 change |
|---|---:|---:|---:|---:|
| 0 | 155,057 | 155,120 | +63 | +71 |
| 1 | 154,846 | 154,999 | +153 | +137 |
| 2 | 154,727 | 155,056 | +329 | +139 |
| 3 | 154,912 | 155,020 | +108 | +199 |
| Mean | 154,885.5 | **155,048.75** | **+163.25** | **+136.5** |

All eight historical/current paired profiles use the same exact source `27be6e312fdd79ad310583d33eeb2a4781d75b97` and frozen binary `37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752`; the environments differ only in pickup weight. Runs were performed in separate batches on shared hosts, so this is not a controlled timing comparison. Seeds are solver RNG seeds on one fixed input, not four independent generated instances.

The two new full runs pass all source, complete-work, 5,000-entry, trajectory and resource checks. Their 100 million robot steps pass independent accounting. Across the four weight-8 runs, every task revealed by 2,500 completes by 5,000, no initially revealed task remains never assigned, no task changes holder after pickup, and the one-retarget limit holds. Oldest never-assigned ages are 188 / 186 / 193 / 184. Outstanding-age p90 improves by 14 / 13 / 12 / 12 versus weight 5, and empty work falls on all four seeds. Fairness tails are not uniformly improved: seed 3's maximum outstanding age is 1,331 versus 1,072 before. Finite observations do not prove starvation freedom.

New seeds 1 / 3 average 447.527 / 447.397 ms per entry, maximum 1.003442 / 0.994332 s, peak RSS 11.947 / 11.926 GB, average CPU 1.506 / 1.512 cores out of four reserved. Wall times are 38.28 / 38.26 minutes. Both satisfy the authorized shared-host 5-second development deadline and 32 decimal GB cap; strict one-second certification remains separate. Benchmark 8900345, verification 8900346 and accounting 8900347 are complete. Verification finished 2026-09-20 17:59:57 UTC, independent accounting 18:00:20 UTC.

The profile still requires explicit `--trick WAREHOUSE` and configured horizon 5,000; short preference stays OFF, ordinary HRRN and forced-oldest admission stay ON. Native neutral-tail scoring and additional full-depth workers remain separate pending experiments.

[Four-seed aggregate with parent hashes](../p90-pickup8-four-seeds-v102.json), [full verification](verification.json), [fairness](fairness.json), [independent accounting](../p90-pickup8-seeds13-v102-outcomes/accounting.json), [exact best configuration](../p90-pickup-combined-v99/best-record.json).
