# More full-depth global workers do not improve the mean

Keeping the original search limits in every worker gives mixed results with two workers and losses on both seeds with four. Retain one global worker for the current throughput reference. This tests additional complete search work, unlike the earlier comparison that divided a fixed total across shorter searches.

| Global workers | Seed 0 | Seed 2 | Mean | Mean change | Mean entry across seeds | Average CPU cores |
|---|---:|---:|---:|---:|---:|---:|
| 1, exact control | 155,120 | 155,056 | 155,088 | 0 | 451.551 ms | 1.504 |
| 2 | 154,979 | 155,084 | 155,031.5 | -56.5 | 459.524 ms | 2.023 |
| 4 | 154,809 | 154,795 | 154,802 | -286 | 478.566 ms | 3.097 |

Two workers change the paired totals by -141 / +28; four by -311 / -261. Final-1,000 changes are +14 / 0 and -39 / -54. None exceeds the 155,120 record. All six totals meet the target numerically, with four-worker seed 2 exactly equal to 154,795; that does not make the extra-work policy better than the controls.

Each worker retains the 4M candidate threshold and one-million repair-attempt ceiling. Aggregate configured work grows with the worker count; every worker finishes before deterministic selection. Four regional regions and two repair rounds stay unchanged. Work counters describe the selected worker, not aggregate work. Construction, stopping conditions and RNG consumption change across these policies. This is not an equal-work speedup comparison and uses no partial-deadline fallback.

All six complete 5,000-step / 10,000-robot runs pass. Both one-worker trajectories exactly reproduce the V99 best profile, and all 28 source/test hashes match `27be6e312fdd79ad310583d33eeb2a4781d75b97`; binary `37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752`. There are 24 disjoint bound physical cores, four per case, on shared research38 with no CPU quota. Maximum entry across the matrix is 1.036029 seconds; RSS remains at most 12.223 GB. Every entry meets the authorized 5-second development deadline and 32 decimal GB limit. Strict one-second certification remains separate.

Independent accounting verifies 300 million robot steps. Every first-half task finishes in all six runs, no initial task remains never assigned, retargets remain at most one and no holder changes after pickup. Oldest never-assigned ages for two workers are 184 / 188, and four workers 186 / 180, versus controls 188 / 193. Outstanding-age p90 changes are +1 / -3 and +3 / 0. Empty and unfinished-work differences are small and mixed; these counts are not causal savings. Finite fairness observations do not establish starvation freedom.

Benchmark 8900366, verification 8900367 and accounting 8900368 completed successfully. Verification finished 2026-09-20 18:10:10 UTC; accounting 18:10:49 UTC. All arms use p90 / pickup weight 8, ordinary fairness, explicit `--trick WAREHOUSE`, configured horizon 5,000, and neutral-tail scoring OFF.

[Full verification](verification.json), [fairness](fairness.json), [physical work](work-budget.json), [independent accounting](../native-workers-full-v103-outcomes/accounting.json), [prescribed-work design](../../native_portfolio/FULL_PER_WORKER.md).
