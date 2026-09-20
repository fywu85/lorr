# New Warehouse best:155057 with a prospective p90 completion margin

The90th-percentile margin finishes155057/154727tasks on solver seeds0/2, up168/
171over exact mean-margin controls. Best155057 exceeds the NMS target154795 by
262 (+0.1693%); the two-seed mean154892 exceeds it by97. Seed2 individually is
still68below target. This is a cutoff-aware Warehouse TRICK requiring explicit
--trick WAREHOUSE and a configured5000-step horizon, not a generic improvement
or a proof of repeatability across new inputs.

| Margin estimator | Seed0 | Seed2 | Paired changes | Mean |
|---|---:|---:|---|---:|
|rational mean, exact control|154889|154556|0/0|154722.5|
|median|154767|154466|-122/-90|154616.5|
|p75|154991|154691|+102/+135|154841.0|
|p90|155057|154727|+168/+171|154892.0|

Only already-completed tasks that retained their first accepted holder train the
five duration-minus-bound histograms. Each entry uses an immutable nearest-rank
snapshot. A percentile is a heuristic, not a guaranteed completion probability.
Short-task preference stays OFF; ordinary HRRN, forced oldest admission, held/
started protection and the one-retarget limit remain unchanged.

All eight complete5000-step/10000-robot runs and40000entry measurements pass,
including all28source/test hashes, frozen binary, no CPU quota,32disjoint bound
physical cores and32decimalGB process RSS. Both mean control trajectories exactly
reproduce V92. The independent accounting also verifies400million robot steps and
exact pre-intervention action/schedule/event prefixes. The p90 prefixes are3798/
3800steps; all168/171additional completions occur in the final1000steps. This is
not evidence of higher steady-state throughput before the known cutoff.

Unfinished task work falls1155533->1090394 and1164023->1090445robot steps (about
5.6%/6.3%), while empty work rises19000/21482. These are observed accounting
differences in changing cohorts, not a causal partition or recoverable-work bound.

Fairness: all tasks revealed by2500 finish by5000; no initially revealed task
remains never assigned. Final oldest never-assigned ages190/185, and no reassignment
after pickup. Historical maximum first-assignment waits687/716 versus control
700/691 are mixed; outstanding-age p90 improves7steps on each seed. There is no
persistent early backlog in these runs, but no formal starvation-freedom claim.

Best-run mean entry450.624ms, maximum1.024536s, peakRSS11.925GB, average CPU1.502
cores of4reserved; wall38.56minutes. Second-seed mean447.517ms, max1.006404s and
RSS12.483GB. Shared5s development; strict1s competition certification is separate.

Source27be6e312fdd79ad310583d33eeb2a4781d75b97, binary
37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752.
Best completed2026-09-20T16:18:39.911128UTC.

[Exact best configuration, timestamp and hash](best-record.json), [full validation](verification.json),
[waiting tails](fairness.json), [independent accounting/prefixes](../horizon-percentile-full-v96-outcomes/accounting.json).
The p90 profile is the next working best; generic defaults remain unchanged.
Additional p90 seeds1/3 and paired p95/p99 refinements are now running.
