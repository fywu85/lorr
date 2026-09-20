# Neutral-tail service scoring loses both full seeds

The optional neutral-tail score finishes **154,930 / 154,882**, versus exact controls **155,120 / 155,056**: losses of **190 / 174**, mean **-182**. Keep the option OFF. The result supports retaining the current score; it does not indicate a correctness failure in the optional implementation.

| Mode | Seed 0 | Seed 2 | Mean | Final-1,000 change |
|---|---:|---:|---:|---|
| Existing native service score | 155,120 | 155,056 | 155,088 | Control |
| Neutral tail after a goal hit | 154,930 | 154,882 | 154,906 | +12 / -12 |

All four full 5,000-step / 10,000-robot runs pass, including exact complete OFF trajectories and all 28 source/test hashes. Source `e757965ad5869b9004e63da4c0bab52ecdb64549`, binary `c7f7b32063284b42c4cf50894df983b7d1af3489b2f009854b092a270430a0b4`. Active service receipts and all 25 counter samples validate; the ON mode reprices 17,532,505 / 17,547,496 of 27,719,878 / 27,740,371 goal-serving candidate evaluations. Physical reservations and CGAR protections remain intact. The complete generic and native compatibility checks passed before this quality comparison.

Independent accounting verifies 200 million robot steps. Despite 9,323 / 11,925 fewer empty actions and 1,322 / 7,592 less work on unfinished tasks, throughput falls. Loaded waits rise 49,214 / 46,232 and loaded turns rise 10,772 / 10,250; loaded forward moves fall 50,663 / 44,557. These are whole-policy observations with changed task cohorts, not a causal decomposition. Most of the throughput loss already exists before the last 1,000 steps (-202 / -162).

All first-half tasks still finish; no initial task remains never assigned, maximum retargets remain one and no assignment changes after pickup. ON oldest never-assigned ages are 182 / 181, but outstanding-age p90 changes +3 / 0 and maximum historical first-assignment wait changes +4 / +28. No uniform fairness improvement or starvation guarantee is claimed.

ON mean entry times are 448.776 / 446.736 ms, maxima 1.008243 / 1.009697 seconds; peak RSS 13.225 / 11.950 GB. Four physical cores per process, 16 disjoint cores on shared research38, no CPU quota. Every entry meets the authorized 5-second development deadline and 32 decimal GB limit. This is not strict one-second certification or a controlled speedup measurement.

Benchmark 8900363, validator 8900364 and independent accounting 8900365 are complete. Full verification finished 2026-09-20 18:07:44 UTC; independent accounting 18:08:22 UTC. The profile uses p90 margin and pickup weight 8, explicit `--trick WAREHOUSE`, ordinary fairness and short preference OFF. The 155,120 best and its four-seed confirmation remain unchanged.

[Verification](verification.json), [fairness](fairness.json), [work summary](work-budget.json), [independent accounting](../native-neutral-tail-full-v101-outcomes/accounting.json), [implementation and tests](../../tricks/neutral_tail/README.md).
