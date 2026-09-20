# Keep p90: higher completion margins regress on both seeds

The verified p90 controls reproduce **155,057 / 154,727** and both complete V96 trajectories exactly. P95 and p99 reduce full throughput on both solver seeds. No new record or configuration promotion.

| Margin | Seed 0 | Seed 2 | Changes from p90 | Mean |
|---|---:|---:|---|---:|
| p90, exact control | 155,057 | 154,727 | 0 / 0 | 154,892 |
| p95 | 155,036 | 154,715 | -21 / -12 | 154,875.5 |
| p99 | 154,856 | 154,492 | -201 / -235 | 154,674 |

All six full 5,000-step / 10,000-robot cases pass. Verification binds all 28 source/test files to `27be6e312fdd79ad310583d33eeb2a4781d75b97` and binary `37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752`. The 24 bound physical cores are disjoint, four per process, with no CPU quota. All 30,000 complete entries satisfy the shared-host 5-second development limit and 32 decimal GB RSS. Maximum entry is 1.011254 seconds and peak RSS 12.533 GB; this does not certify strict one-second timing.

Independent accounting verifies 300 million robot steps and unchanged input hashes. Exact action/schedule/event prefixes run through the first possible intervention: p95 at 3,787 / 3,790 and p99 at 3,786 / 3,789. All task-count differences occur in the final 1,000 steps. Compared with p90, unfinished-task work rises 8,646 / 8,323 for p95 and 84,466 / 92,808 for p99. Empty work also rises. Higher empirical percentiles are not guaranteed to improve completion probability or throughput.

Every first-half task finishes in every arm, and no initially revealed task remains never assigned. P95 leaves outstanding-age p90 unchanged; p99 worsens it by 9 / 10 steps and increases oldest never-assigned ages from 190 / 185 to 200 / 198. HRRN, forced-oldest admission, held/started protection and at most one unopened retarget remain unchanged. These finite observations do not prove starvation freedom.

Validation completed 2026-09-20 17:01:27 UTC; independent outcomes completed 17:02:27 UTC. Retain p90 for the pending pickup-weight and search-portfolio comparisons. No larger-percentile sweep is justified by this result.

[Full verification](verification.json), [fairness](fairness.json), [independent work and prefix evidence](../horizon-percentile-refine-v98-outcomes/accounting.json).
