# Four p90 solver seeds average 154,885.5 tasks

The additional seeds 1 / 3 finish **154,846 / 154,912** tasks. Combined with verified seeds 0 / 2, the same frozen p90 / pickup-weight-5 profile yields:

| Solver seed | Tasks | Difference from NMS target 154,795 |
|---|---:|---:|
| 0 | 155,057 | +262 |
| 1 | 154,846 | +51 |
| 2 | 154,727 | -68 |
| 3 | 154,912 | +117 |
| Mean | **154,885.5** | **+90.5** |

This confirms the existing profile; the single-run best remains 155,057. These are four solver RNG seeds on one fixed map/task input, not four independently generated instances or a statistical guarantee of beating NMS.

Both new full 5,000-step / 10,000-robot runs pass all source/binary, complete-work, timing, memory, allocation and action-accounting checks. Independent accounting covers their 100 million robot steps. Source `27be6e312fdd79ad310583d33eeb2a4781d75b97`, binary `37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752`; same as the original p90 runs.

Every task revealed by 2,500 finishes by 5,000. Oldest never-assigned ages are 190 / 192; historical maximum first-assignment waits are 717 / 699. No initially revealed task remains never assigned, at most one unopened retarget occurs and no task is reassigned after pickup. Unfinished-task work is 1,106,033 / 1,102,876 robot steps.

Mean entries are 446.690 / 445.666 ms, maxima 1.014136 / 1.009184 seconds, peak RSS 11.977 / 11.945 GB. Eight disjoint physical cores on shared research44, four per process. The 5-second development deadline and 32 decimal GB cap pass; strict one-second certification remains separate.

Relative to the historical mean-margin results, p90 gains 168 / 133 / 171 / 136 tasks across seeds 0 / 1 / 2 / 3, mean +152. Those older mean runs used the V93 binary. V96 mean controls exactly reproduce seeds 0 / 2; mean seeds 1 / 3 were not rerun under V96. Therefore this historical comparison is not labelled a four-pair same-binary experiment.

Validation completed 2026-09-20 17:00:43 UTC; independent accounting completed 17:01:07 UTC. The profile remains an explicit `--trick WAREHOUSE` configuration with known horizon 5,000 and ordinary fairness.

[Four-seed record with source hashes](../horizon-percentile-four-seeds-v98.json), [new full verification](verification.json), [waiting tails](fairness.json), [independent accounting](../horizon-percentile-seeds13-v98-outcomes/accounting.json).
