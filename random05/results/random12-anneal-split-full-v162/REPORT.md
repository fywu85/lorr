# Sparse complete-plan annealing: all16full results

Audited 2026-09-21T23:58:29.465817+00:00. Source4fb9498e/build162; four zero-temperature controls match all six full archived trace fields. All cases complete600steps with valid replay and strict limits.

| Profile | Temperature0 | 0.25 | 1 | 4 |
|---|---:|---:|---:|---:|
| general RANDOM-01 | 727 | 727 | 727 | 727 |
| trick RANDOM-01 | 733 | 735 | 732 | 729 |
| general RANDOM-02 | 1397 | 1387 | 1392 | 1391 |
| trick RANDOM-02 | 1408 | 1389 | 1398 | 1394 |

Selected RANDOM-01 improves733→**735**, **+6.21% versus matched max(NMS,KK)=692**. Mean/max entry73.470/111.230ms, peakRSS386.793MB. This is a selectedseed1 result; its exactrepeat and fouradditional planner seeds are predeclared separately. General01is unchanged in score; all nonzero02settings lose. Keep02at1408 (+12.10% overmatchedmax1256).

The annealer always publishes its best complete legal plan and finishes the declared iteration budget. It does not return a partial plan at a wall-clock cutoff. No fresh-input results are implied.
