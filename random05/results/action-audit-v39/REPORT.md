# RANDOM-05 replay and robot-work audit

Source: `56b0970`; all four complete trajectories pass independent replay,
vertex/edge collision checks, task-assignment locks, goal-event checks and
robot-step accounting. Inputs and source hashes are in `report.json`.

| Run | Tasks | Robot work on unfinished tasks | Uninterrupted wait p99 |
|---|---:|---:|---:|
| ours3395 | 3395 | 13.41% | 13 |
| ours3197-no-cutoff | 3197 | 16.18% | 11 |
| nms4-2914 | 2914 | 31.03% | 6 |
| nms32-3172 | 3172 | 30.04% | 6 |

Compare loaded work only on task IDs completed by both solvers:

| Pair | Common tasks | Extra forward steps per task | Extra turns per task | Extra waits per task | Extra total steps per task |
|---|---:|---:|---:|---:|---:|
| ours3395 vs nms4-2914 | 2860 | +0.65 | -15.50 | +25.77 | +10.92 |
| ours3395 vs nms32-3172 | 3049 | +4.66 | -3.52 | +35.26 | +36.40 |
| ours3197-no-cutoff vs nms4-2914 | 2810 | +0.69 | -14.99 | +27.09 | +12.79 |
| ours3197-no-cutoff vs nms32-3172 | 2926 | +5.12 | -2.31 | +38.28 | +41.08 |

Longer waits remain after controlling for completed task IDs. The largest
difference is against NMS32: about35 extra waiting steps per common completed
task. Against NMS4, fewer turns compensate for part of the extra waiting.
Our lower share of work on unfinished tasks helps explain why our final task
count can still be higher. These are observations, not causal savings:
assignments, congestion histories, and start/completion times differ. Work on
unfinished tasks cannot simply be converted into a predicted throughput gain.

Next controlled hypothesis: augment the current pipeline with safe, already
aligned forward chains/cycles among robots previously scheduled to wait.
Preserve every promised forward move, require positive aggregate potential
gain relative to an idle turn, and certify the resulting actual and next moves.
The option is off unless `R05_EARLY_FILL=1`. The subsequent full comparison
found no gain: thresholds 0/1/4 score 3,369/3,355/3,381 versus 3,395. See
`../early-fill-split-full-v40/summary.json`; the feature remains off.
