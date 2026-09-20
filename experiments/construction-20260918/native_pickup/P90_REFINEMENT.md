# Local pickup-weight refinement on the verified p90 profile

Predeclared 2026-09-20. Compare weights **8 / 6 / 10**, each on solver seeds **0 / 2** for the full 5,000 Warehouse steps. The weight-8 control is the verified 155,120 / 155,056 profile. V97 found weight 8 ahead of weights 5 and 12 under the earlier mean completion margin; this bounded comparison tests its immediate neighborhood with p90. It is not a broad parameter search or a claim of improvement.

Only `CGAR_PICKUP_WEIGHT` changes. Use the existing frozen V96 source `27be6e312fdd79ad310583d33eeb2a4781d75b97` and binary `37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752`, ordinary HRRN / forced-oldest admission / protected holders / one-retarget limit, global short preference OFF, and explicit `--trick WAREHOUSE`. No fresh-pickup audit or neutral-tail scoring. The current record and four-seed recommendation remain unchanged until full verification.

Six independent cases run on 24 disjoint physical cores, four per process, shared GRID EPYC 9354 hosts, complete fixed work, 5-second development deadline and 32,000,000,000-byte RSS cap. Verify exact control trajectories, all source and binary hashes, per-entry validity, fairness tails and independent action accounting. Log losses as well as wins; a single valid increasing full score can raise the single-run record, while the paired mean and four-seed evidence remain separate.

Submitted GRID 8900448; frozen full verifier 8900449 and independent accounting 8900451. Raw `runs/cgar-p90-pickup-refine-v106-20260920`; results `results/p90-pickup-refine-v106` and `-outcomes`. The first accounting preparation mistakenly used the benchmark raw directory and stopped with `FileExistsError` before submitting any job; corrected preparation uses a distinct `runs/cgar-p90-pickup-refine-outcomes-v106-20260920`. Benchmark inputs/runs were unchanged.


Completed and independently verified 2026-09-20 19:24:04 UTC. Weight6=155054/154784 (-66/-272); weight10=154993/154769 (-127/-287); weight8 controls exactly155120/155056. Both candidates lose both seeds; retain8. All6fullcases and300million robotsteps pass, everyfirsthalf taskfinishes, maxone retarget/noafterpickup. [Final report](../results/p90-pickup-refine-v106/summary.md).
