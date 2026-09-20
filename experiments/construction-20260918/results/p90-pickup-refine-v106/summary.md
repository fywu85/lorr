# Retain pickup weight 8 after the local p90 refinement

Both neighboring settings lose on both full seeds. The current **155,120** record and the four-seed weight-8 recommendation remain unchanged.

| Pickup weight | Seed 0 | Seed 2 | Differences from weight 8 | Paired mean |
|---|---:|---:|---:|---:|
| **8, exact control** | **155,120** | **155,056** | 0 / 0 | **155,088** |
| 6 | 155,054 | 154,784 | -66 / -272 | 154,919 |
| 10 | 154,993 | 154,769 | -127 / -287 | 154,881 |

All six full 5,000-step runs and 30,000 complete decisions pass, with no errors/timeouts, a 5-second development deadline and 32,000,000,000-byte RSS cap. Both controls reproduce the V99 complete hashes exactly. All28source/testhashes, the frozen binary, fixed work and24disjointphysicalcores (four per case) are checked. Independent accounting verifies **300 million robot steps** and unchanged raw hashes.

Weight6 increases empty work by18,023 /7,878robotsteps and loadedwaits by5,710 /58,401. Weight10 changes emptywork by+952 /-4,670, while loadedwaits rise21,211 /68,585. Their final1000task changes are-37 /-118 and+51 /-5 respectively. These are descriptive differences between changing task/traffic cohorts, not a causal partition of lost throughput. Weight10's slightly lower agep90 (-3 /-5) does not compensate for its lower completion total.

All six runs retain ordinary HRRN, forced-oldest admission, held/started protection, one unopened retarget and global short preference OFF. Every task revealed in the first half completes, no initial task remains never assigned, and there are no post-pickup reassignments. Oldestnever ages: weight8=188/193; weight6=188/190; weight10=184/182. Maximum outstanding ages are mixed, and finite tests do not establish starvation freedom.

Entry means span447.381–497.919ms, maxima up to1.098030s, process RSS below11.963GB. Seed0 timings show more shared-host contention than seed2; these are not controlled speed comparisons or strict1s certification. Fixed complete work, exact controls and zero timeouts support the task-count comparison despite timing variation.

Source `27be6e312fdd79ad310583d33eeb2a4781d75b97`; binary `37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752`. Full verification 2026-09-20T19:23:12.814891+00:00; independent accounting 2026-09-20T19:24:04.377060+00:00. All cases use `--trick WAREHOUSE`, native bands, configuredH5000/p90, matching64 and one global worker. The new cadence option is not present in this frozen binary.

[Verification and exact environments](verification.json), [waiting tails](fairness.json), [physical work](work-budget.json), [independent accounting](../p90-pickup-refine-v106-outcomes/accounting.json), [predeclared protocol](../../native_pickup/P90_REFINEMENT.md).
