# Generic mixed pickup grouping: full comparison

All four full 5,000-step cases passed source/binary, simulator, complete-entry, memory, physical-binding and prescribed-work checks. Both local-group controls exactly reproduce V64 matching64. Source [5dc6ae4](https://github.com/fywu85/lorr/commit/5dc6ae4). Shared-host 5s development deadline; no strict 1s certification.

| Seed | Local grouping | Mixed pickup grouping | Difference | Final 1,000 difference | Empty-work difference |
|---|---:|---:|---:|---:|---:|
| 0 | 144,967 | 144,632 | -335 | -3 | -47949 |
| 2 | 144,511 | 144,398 | -113 | -61 | -18724 |

Mixed grouping loses **0.15476%** in paired mean. It reduces empty work slightly, but this does not translate into throughput: both full totals and both tails decline. Keep `CGAR_REASSIGN_MATCH_PICKUP_GROUPS=0`. Larger estimated matching savings are not evidence of a better policy. This result tests the implemented hard 16/16 grouping; it does not rule out every task-centered assignment algorithm.

All 20,000 decisions completed. Maximum entry 0.892794429s; peak RSS 11,939,561,472 bytes. No partial score or failed entry was accepted.

[Complete comparison](comparison.json) · [Independent verification](verification.json).
