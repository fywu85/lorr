# RANDOM-04 whole-chain guidance layout search

All eight full 1,000-step runs pass independent source/input/resource checks, movement/assignment/task-event replay, strict one-second entries and 32 GB. The original layout15/one-flip control exactly reproduces2,799 in all six complete trace fields.

| Layout generation seed | Original flip count | Tasks | Maximum entry ms |
|---|---:|---:|---:|
| 15 | 1 | 2799 | 657.644 |
| 15 | 0 | 2772 | 664.794 |
| 1 | 0 | 2625 | 674.095 |
| 3 | 0 | 2692 | 669.996 |
| 7 | 0 | 2686 | 699.831 |
| 23 | 0 | 2670 | 671.668 |
| 31 | 0 | 2683 | 766.586 |
| 43 | 0 | 2677 | 650.875 |

All six additional layouts lose to the existing pattern. Removing the original pattern's tuned flip also loses27 tasks. Keep layout15/one flip; no new throughput record or generalization claim follows. These are explicitly map-tuned `--trick RANDOM-04` experiments on one archived task/start input and planner seed0.

The unchanged layout control is also an additional exact repetition of the source168 faster matching configuration: mean474.599ms, maximum657.644ms, RSS689.877MB. Together with its separate five paired planner seeds, this supports the runtime improvement without removing declared planning work. Earlier source166 timeouts remain recorded failures.

[Independent audit](audit.json), [exact control](control-equivalence.json), [five-seed runtime qualification](../random12345-zero-update-split-full-v168/RANDOM04_RUNTIME.md).
