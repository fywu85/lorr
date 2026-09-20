# Five-step matching: small mixed effect, confirmation pending

| Matching interval | Seed 0 | Seed 2 | Paired mean |
|---|---:|---:|---:|
| 10 steps, exact control | 155,120 | 155,056 | 155,088 |
| 5 steps | 155,109 | 155,086 | 155,097.5 |
| Difference | **-11** | **+30** | **+9.5** |

No new record: **155,120** remains the verified peak. The small two-seed mean increase is insufficient to replace the four-seed ten-step recommendation. Full matched seeds1/3 are queued as V111 to check the direction on additional solver seeds of the same fixed input.

All four full runs,20,000complete decisions and200million independently accounted robot steps pass. Both controls reproduce the complete V99 hashes. All28source/test hashes, frozen binary, prescribed matching cadence,16disjoint physical cores (four per process),5sdevelopment deadlines and32decimalGB RSS are checked. No errors, timeouts, post-pickup changes or task retarget beyond one.

Five-step matching reduces empty work by9,995 /15,732robot steps but increases work left on unfinished tasks by7,834 /3,528. Its final1,000step totals gain75 /47; earlier portions lose86 /17. These describe changed task/traffic cohorts, not a causal partition. More frequent matching creates much more activity with little net quality change: at logged decision4,800, accepted moved rows rise14,856->22,996 /14,875->22,995. BFS-node and matrix-entry work roughly doubles; this is not an equal-work speedup.

Every first-half task finishes, no initially revealed task remains never assigned, and global short preference remains OFF. Oldest never-assigned ages are188 /188 for five-step versus188 /193 for ten-step. Outstanding-age p90 changes+2 /-2 and maxima1035 /1072 versus1078 /1044; tails are mixed. These finite observations do not prove starvation freedom.

Five-step entries average455.921 /452.914ms with maxima0.989851 /0.993570s, RSS12.969 /12.242GB and about1.50CPU cores of four reserved. Ten-step averages446.410 /450.572ms. Shared-host timing does not establish a controlled latency effect; strict1s certification remains separate.

Source `1ac2a4dd31bfe2b2262c4ab496604f71b37742c3`; binary `741d8e378df5465bf035af92a3b18e0b8f066043c83f0e25560966abbbfd5cf3`. Full verification 2026-09-20T19:57:35.435954+00:00; independent accounting 2026-09-20T19:58:06.517666+00:00. P90, pickupweight8, Warehouse native bands, matching64 and all remaining solver work are fixed. Explicit `--trick WAREHOUSE` remains required for the base profile.

[Verification/configurations](verification.json), [matching samples](work-samples.json), [fairness](fairness.json), [physical work](work-budget.json), [independent accounting](../match-cadence-native-full-v108-outcomes/accounting.json).
