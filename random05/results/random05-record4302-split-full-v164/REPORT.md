# Selected 4,302 qualification: exact repeat, one failed control

All seven declared new attempts have finished and are independently audited. The 4,302 repeat matches all six full trace fields. Every candidate run passes the strict full-horizon checks, but the seed3 control timed out at step0. The complete four-pair qualification therefore fails its all-runs-valid requirement.

| Planner seed | Credit 0 | Credit 0.5 | Difference |
|---|---:|---:|---:|
| 0 | 4254 | 4302 | 48 |
| 1 | 4189 | 4250 | 61 |
| 2 | 4252 | 4276 | 24 |
| 3 | FAILED | 4224 | — |

The three complete valid pairs total 12828 versus 12695 (+1.0477%), all positive. This subset includes the selecting seed0 and cannot stand in for the predeclared four-pair result. The four candidate scores average 4,263. The exact repeat is a separate repetition and is not double-counted.

The failed control took 1,045.313 ms at step0 (assignment498.259 ms, look-ahead522.687 ms). Its previous successful historical score is not substituted. The largest successful qualification entry is 956.058 ms. Separate fixed first-step work changes remain different configurations.

No fresh-input claim is made for 4,302. Its selected full score remains +35.62% against matched max(NMS, KK)=3,172.

[Original failure](failure-details.json), [independent audit](audit.json), [exact repeat](control-equivalence.json), [paired accounting](paired-comparison.json).
