# New verified Warehouse best: 155,120

Combining the prospective p90 completion margin with pickup weight 8 finishes **155,120 / 155,056** tasks on solver seeds 0 / 2. Both exceed the user-specified NMS target of 154,795; the paired mean is **155,088**, 293 above target. The best run improves the previous record by 63 and exceeds NMS by 325 (+0.2100%).

| P90 configuration | Seed 0 | Seed 2 | Paired changes | Mean |
|---|---:|---:|---|---:|
| Pickup weight 5, exact control | 155,057 | 154,727 | 0 / 0 | 154,892 |
| Pickup weight 8 | **155,120** | **155,056** | **+63 / +329** | **155,088** |

All four full 5,000-step / 10,000-robot runs pass, with exact complete p90 control trajectories. Verification binds all 28 source/test hashes to `27be6e312fdd79ad310583d33eeb2a4781d75b97` and binary `37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752`. Sixteen disjoint physical GRID cores, four per process, no CPU quota. All 20,000 complete entries and 200 million independently accounted robot steps validate.

This combines measured components, not assumed additive gains. Relative to the old mean-margin / weight-5 baseline, the interaction is +23 / -80 tasks beyond the sum of the separately measured component effects. The combination still improves both matched p90 controls. Its gains before the final 1,000 steps are -8 / +190; the final window adds +71 / +139.

Empty work falls 26,083 / 20,641 robot steps, and unfinished-task work falls 20,364 / 14,797. Outstanding-age p90 improves by 14 / 12. Every task revealed by step 2,500 finishes by 5,000; no initially revealed task remains never assigned. Oldest never-assigned ages are 188 / 193. Historical maximum first-assignment waits are 695 / 689 versus 687 / 716 for the controls, so that metric is mixed. Ordinary HRRN, forced-oldest admission, held/started protection and the one-retarget limit remain on; global short preference stays OFF. These finite observations do not prove starvation freedom.

The best run averages **447.628 ms** per entry and **1.507 CPU cores** out of four reserved; maximum **0.996733 s**, peak RSS **11.945 GB**, full wall **38.27 minutes**. Seed 2 averages 446.578 ms, maximum 1.013663 s, peak RSS 11.960 GB. All satisfy the authorized shared-host 5-second development deadline and 32,000,000,000-byte process limit. Strict one-second competition certification remains separate.

Best completed **2026-09-20 17:16:12.716303 UTC**. Full validation completed 17:16:59 UTC; independent accounting completed 17:17:30 UTC. The profile requires explicit `--trick WAREHOUSE` and a configured 5,000-step horizon. These are two solver RNG seeds on one fixed input. Additional seeds 1 / 3 now confirm the profile at 154,999 / 155,020: all four exceed target, with mean 155,048.75. The earlier four-seed mean 154,885.5 belongs to weight 5. [Confirmation](../p90-pickup8-seeds13-v102/summary.md).

[Exact configuration, source, timestamp and hashes](best-record.json), [full verification](verification.json), [fairness](fairness.json), [independent accounting](../p90-pickup-combined-v99-outcomes/accounting.json).
