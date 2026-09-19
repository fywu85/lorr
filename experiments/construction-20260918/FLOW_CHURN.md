# Observed traffic-field changes

Fable09 suggested that unusually high field churn precedes the graded margin0
seed0 collapse. GRID8898800 replayed all four completed graded trajectories,
reconstructed all10 cumulative field publications, and checked every production
move/penalized-edge count plus complete movement totals. This is an offline audit;
no trajectory information is supplied to the planner.

| Margin | Seed | Full tasks | First-refresh pair churn (step640) | Direct sign flips |
|---|---:|---:|---:|---:|
| 0 | 0 | 56,537 | 33.179% | 3,614 |
| 0 | 2 | 139,796 | 31.942% | 3,362 |
| 25 | 0 | 138,402 | 43.321% | 388 |
| 25 | 2 | 139,303 | 43.081% | 376 |

The failed margin0 seed has only3.87% higher first-refresh pair churn and7.50%
more sign flips than the healthy margin0 seed; later its churn becomes lower.
Healthy margin25 has higher total membership churn but far fewer direct sign
reversals. The simple prediction of a large early global-churn excess is not
supported. Do not implement sign hysteresis on this evidence alone. Localized
harmful reversals or traffic feedback remain possible, not established causes.
Fable's10% tolerance was a heuristic, not a statistical significance criterion.

Pair churn counts an undirected edge once when its penalized direction appears,
vanishes or reverses. Directed membership churn counts both toggled directions
in a reversal. Weight-only changes are separate. Dominance buckets use current
cumulative observed counts and upper-inclusive25/50/75/100percent bounds.
Production logs contain counts, not field hashes: aggregate reconstruction checks
do not prove an exact match to a logged production vector.

[Reconstruction](results/graded-flow-churn-v39.json),
[comparison](results/graded-flow-churn-comparison-v39.json),
[audit script](flow_churn_audit.py).
