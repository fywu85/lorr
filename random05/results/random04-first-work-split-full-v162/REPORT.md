# RANDOM-04 fixed startup work

All nine new full runs pass independent replay and strict limits. The seed4/FIRST_K4032 control reproduces2782 in all six fields. Smaller first-step portfolios do not improve paired total throughput; later steps retain K8064.

| Seed | First work | Tasks | Step0 ms | Later maximum ms |
|---|---:|---:|---:|---:|
| 4 | 4032 | 2782 | 801.694 | 568.639 |
| 4 | 3072 | 2724 | 780.124 | 561.346 |
| 4 | 2048 | 2746 | 683.837 | 574.737 |
| 0 | 4032 | 2749 | 793.831 | 583.210 |
| 0 | 3072 | 2751 | 768.115 | 566.037 |
| 0 | 2048 | 2746 | 688.884 | 576.317 |
| 11 | 4032 | 2736 | 795.787 | 575.039 |
| 11 | 3072 | 2729 | 800.430 | 569.920 |
| 11 | 2048 | 2718 | 682.524 | 560.626 |

Totals at first work4032/3072/2048 are 8267/8204/8210. This is a runtime/throughput tradeoff on three archived planner seeds, not a new selected record or universal runtime guarantee.

The earlier seed11 step0 timeout at1,050.759 ms remains a failed original attempt. Its successful new4032 repetition does not erase that failure or establish its cause. These are complete declared first-step portfolios; no search was truncated by elapsed time.

[Independent audit](audit.json), [exact control](control-equivalence.json), [per-phase runtime](startup-latency.json).
