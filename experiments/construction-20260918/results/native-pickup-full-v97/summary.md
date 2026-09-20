# Pickup weight 8 improves both tested Warehouse seeds

With the existing mean completion margin, pickup weight 8 finishes **154,929 / 154,965** tasks on solver seeds 0 / 2, gains of **40 / 409** over weight 5. Both exceed the user-specified NMS target of 154,795. The paired mean is 154,947, but neither run exceeds the separate p90-margin record of 155,057. These are solver seeds on one fixed input.

| Pickup weight | Seed 0 | Seed 2 | Changes from weight 5 | Mean |
|---|---:|---:|---|---:|
| 5, exact control | 154,889 | 154,556 | 0 / 0 | 154,722.5 |
| 8 | 154,929 | 154,965 | +40 / +409 | 154,947 |
| 12 | 154,820 | 154,844 | -69 / +288 | 154,832 |

All six full 5,000-step, 10,000-robot runs pass; both control trajectories reproduce the original V92 hashes exactly. Validation checks 30,000 complete decisions, all 28 source/test hashes, the frozen binary, fixed search work, 24 disjoint physical cores (four per run), no CPU quota and the 32,000,000,000-byte process RSS ceiling. Independent accounting verifies all 300 million robot steps and the unchanged raw hashes.

Weight 8 reduces empty work by 27,184 / 21,623 robot steps and unfinished-task work by 8,016 / 37,586. Its final 1,000-step gains are 48 / 219; effects before that window are -8 / +190. Unlike the percentile-only change, this intervention changes assignments throughout the run. Work differences are descriptive, not a causal partition of the throughput gain.

Ordinary HRRN, forced-oldest admission, held/started protection and the one-retarget limit remain enabled; global short-task preference stays OFF. Every task revealed by step 2,500 finishes by step 5,000. No initially revealed task remains never assigned. For weight 8, oldest never-assigned ages are 188 / 183, outstanding-age p90 improves by 11 / 14, and historical maximum first-assignment waits are 697 / 696 versus 700 / 691 for the control. The last comparison is mixed; finite runs do not prove starvation freedom.

Weight 8 averages 455.321 / 455.815 ms per entry, with maxima 0.996136 / 0.997466 s and peak RSS 11.949 / 11.949 GB. These are shared-host runs with a 5-second development deadline; observing maxima below one second does not certify strict competition timing.

Source `bdf5e7a145626f87d0321eb8df8ab08b9ec3cb4f`; binary `c49f8ccef14674ff718b0e94fa0f4c94ddf16c404fe37112eb20a449a6a21c6b`. Full validation completed 2026-09-20 16:36:06 UTC; independent accounting completed 16:36:57 UTC. This is an explicit `--trick WAREHOUSE` configuration with configured horizon 5,000, not a generic-default change.

Next, combine weight 8 with p90 in a controlled full comparison. The gains need not add: assignment changes the task pool, traffic and the observations used to learn completion margins.

[Full verification and exact environments](verification.json), [waiting-time evidence](fairness.json), [independent work accounting](../native-pickup-full-v97-outcomes/accounting.json), [combined profiles](../../native_pickup/p90-combined-variants.json).
