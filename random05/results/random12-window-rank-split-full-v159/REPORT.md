# Sparse window completion-priority objectives

All original attempts are retained and independently audited against source307d5cc9/build159. Disabled controls match all six complete trace fields exactly.

| Case | Tasks | Mean entry ms | Max entry ms |
|---|---:|---:|---:|
| trick-random-01-window-rank-power0 | 729 | 63.618 | 101.728 |
| trick-random-01-window-rank-power0p125 | 728 | 68.469 | 111.054 |
| trick-random-01-window-rank-power0p5 | 719 | 71.196 | 108.889 |
| trick-random-01-window-rank-power1 | 715 | 68.162 | 109.378 |
| trick-random-02-window-rank-power0 | 1408 | 346.124 | 544.435 |
| trick-random-02-window-rank-power0p125 | 1396 | 356.670 | 548.794 |
| trick-random-02-window-rank-power0p5 | 1382 | 336.666 | 504.186 |
| trick-random-02-window-rank-power1 | 1340 | 498.239 | 710.431 |

Rank powers0.125/0.5/1 lose on both RANDOM01 and RANDOM02. The selected729/1408 controls repeat exactly. Concentrating the window objective on shorter remaining tasks does not improve these profiles; keep rank weighting off on01/02.

[Independent audit](audit.json), [exact controls](control-equivalence.json). These are development results on archived inputs, not fresh validation.
