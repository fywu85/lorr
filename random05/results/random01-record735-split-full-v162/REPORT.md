# RANDOM-01 annealing qualification at 735

Audited 2026-09-22T00:13:27.631081+00:00. All five full 600-step runs pass independent replay, strict entry limits and resource checks. Source4fb9498e/build162. The exact seed1 repetition matches all six complete trace fields.

| Planner seed | Temperature 0 | Temperature 0.25 | Difference |
|---|---:|---:|---:|
| 0 | 729 | 732 | +3 |
| 1 | 733 | 735 | +2 |
| 2 | 729 | 725 | -4 |
| 3 | 727 | 729 | +2 |
| 4 | 732 | 730 | -2 |

Total **3,651 versus 3,650 (+0.027%)**, with three gains and two losses. The selected735 repeats, but the aggregate gain is only one task and does not establish a meaningful average improvement. Keep it as the selected best permitted by the campaign, with the more convincing gate-only five-seed improvement documented separately.

The record remains **+6.21% against matched max(NMS,KK)=692**. The largest entry across these five repeats/seeds is 123.736ms. Controls are the predeclared same-binary gate-on runs; they are historical deterministic comparisons, not a simultaneous randomized experiment. Fresh01V1 qualifies the earlier729 profile only.
