# Preserve each worker's search depth while using the four cores

The prescribed-total-work V100 split finishes sooner but its raw 2 / 4-worker task totals are lower. All six cases, exact controls and the independent 300-million-step accounting now pass. A different question is whether complete searches at the original depth benefit from a parallel portfolio under the current native metric.

Compare 1 / 2 / 4 workers, each with the existing 4M candidate threshold and one-million repair-attempt ceiling, on the verified p90 / pickup-weight-8 profile. The per-worker search limits are unchanged; aggregate limits grow by 2x / 4x, with separate constructions and RNG streams. All workers finish before the best complete score is selected. This changes computation and search diversity together, not just thread count. Sampled global-work counters describe the selected worker only.

All six full paired seed-0 / seed-2 cases reserve four physical cores apiece, with a shared 5-second development deadline and 32 decimal GB process RSS. Exact one-worker controls must reproduce 155,120 / 155,056 and their complete hashes. Use frozen V96 source 27be6e312fdd79ad310583d33eeb2a4781d75b97 and binary 37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752. The new neutral-tail experiment is absent.

The earlier four-full-worker experiment used the generic learned field and lost 1,198 / gained 229 tasks. It supplies a negative prior, not a native-field measurement. More compute need not improve lifelong throughput, even if it improves a local score. This test will measure the effect rather than infer it from the shorter searches. Ordinary fairness, held/started protection, bounded retargeting and explicit `--trick WAREHOUSE` remain unchanged.

V103 benchmark 8900366, validator 8900367, outcomes 8900368 run on research38, 24 disjoint physical cores, starting 2026-09-20 17:28:50 UTC. Raw `runs/cgar-native-workers-full-v103-20260920`; results `results/native-workers-full-v103`.

V103 is complete: two workers give 154,979 / 155,084 (-141 / +28), four give 154,809 / 154,795 (-311 / -261). All six full cases, exact controls and 300 million independently accounted actions pass. Keep one worker; neither mean nor peak improves. [Full result](../results/native-workers-full-v103/summary.md).
