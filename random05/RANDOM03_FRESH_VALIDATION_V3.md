# RANDOM-03 frozen fresh-input validation V3

Completed 2026-09-22 01:52 UTC. The frozen 2,646-task configuration finishes **2,660 / 2,622** on new task/start streams. It is **+9.110% above matched max(NMS, Kitty Knight)** in aggregate, below the 10% milestone. Individual margins are +8.086% and +10.168%.

Against the preceding 2,620-task recipe on these same streams, it gains **+0.590%**, positive on both inputs. The two changes are the physical matching feasibility gate and observed-progress horizon correction .25/span32. Both profiles use source162, planner seed3 and explicit `--trick RANDOM-03`. This comparison qualifies the combined change; it does not isolate the two components.

The [protocol](experiments/random03-fresh-validation-v3-protocol.json) and [12-case manifest](experiments/random03-fresh-validation-v3-full.json) were committed in [3fc121ad](https://github.com/fywu85/lorr/commit/3fc121ad4ccd0ec4e56c60e25caa4d35fb86b156) before generation at 2026-09-22T01:25:17.637576+00:00. Both reference teams and all repetitions were declared before generation. Seeds50021/50022 remain excluded from tuning at every density. [Candidate and baseline source](https://github.com/fywu85/lorr/commit/4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad).

Every original run completes800steps with zero planner/scheduler errors or entry timeouts. All12 pass the source/input/protocol/binary/runtime-assets audits, actual32-logical-CPU affinity on16 physical EPYC9354 cores with no CPU quota, strict1000ms entries,30s preprocessing,32decimalGB process guard, and independent movement/collision/assignment/task-event replay. PILOT completes its declared fixed work or fails.

| Input seed | Candidate | Previous | NMS repeats | KK repeats | max(NMS, KK) | Gain | Candidate mean / max ms |
|---|---:|---:|---|---|---:|---:|---:|
| 50021 | 2660 | 2639 | 2461 / 2444 | 2084 / 2093 | 2461 | +8.086% | 494.489 / 727.685 |
| 50022 | 2622 | 2612 | 2361 / 2380 | 2094 / 2130 | 2380 | +10.168% | 478.907 / 677.620 |

Candidate total5,282 versus previous5,251 and the sum of per-input reference maxima4,841. NMS is stronger on both inputs. Candidate RSS is464.27/461.88MB. These are two streams on the same map layout, not unseen geometry or the colleague's private inputs. Fresh2,660 belongs to a different input and does not replace the archived2,646 record.

| Run | Longest completed order, steps | Oldest unfinished, censored steps | Initial orders unfinished |
|---|---:|---:|---:|
| seed50021-ours | 761 | 800 | 41 |
| seed50021-baseline | 769 | 800 | 40 |
| seed50021-nms-repeat1 | 788 | 800 | 46 |
| seed50021-nms-repeat2 | 775 | 800 | 46 |
| seed50021-kk-repeat1 | 779 | 800 | 60 |
| seed50021-kk-repeat2 | 800 | 800 | 57 |
| seed50022-ours | 793 | 800 | 36 |
| seed50022-baseline | 795 | 800 | 37 |
| seed50022-nms-repeat1 | 771 | 800 | 49 |
| seed50022-nms-repeat2 | 762 | 800 | 47 |
| seed50022-kk-repeat1 | 772 | 800 | 56 |
| seed50022-kk-repeat2 | 774 | 800 | 56 |

Waiting metrics are secondary. Unfinished orders remain censored at800steps, so their eventual maximum waiting time is unknown.

Evidence: [protocol/source/timing audit](results/random03-fresh-validation-v3/audit.json), [independent replay and waiting audit](results/random03-fresh-validation-v3/replay-audit.json), [actual binary/launch-affinity audit](results/random03-fresh-validation-v3/launch-audit.json), [generation metadata](results/random03-fresh-validation-v3/generation.json).
