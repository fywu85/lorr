# TRICK: five-step matching confirmation, full seeds1/3

Reject five-step matching as a replacement for ten-step matching. The additional paired seeds lose208/192tasks. Combining V108 and this V111 confirmation, differences on seeds0/1/2/3 are-11/-208/+30/-192, with one win and three losses. The candidate mean154953.5 is95.25below the control155048.75. More frequent matching decreases empty work in all four runs but increases unfinished work and, on these additional seeds, sharply increases waiting. Retain interval10; one-retarget and cooldown20 rules stay unchanged. Machine-readable four-seed evidence: ../match-cadence-four-seeds-v111.json.

Verified 2026-09-20T20:37:25.423013+00:00. Source [1ac2a4d](https://github.com/fywu85/lorr/commit/1ac2a4dd31bfe2b2262c4ab496604f71b37742c3); binary `741d8e378df5465bf035af92a3b18e0b8f066043c83f0e25560966abbbfd5cf3`.

Every case completes5000steps with10000robots, zero planner/scheduler errors and timeouts, and independently reconciled50million robot actions. All28source/test hashes, fixed work, distinct four-core bindings,5s entry deadline and32decimalGB RSS checks pass. Exact prior controls reproduce their complete trajectory hashes. Shared-host timings do not certify the competition1s limit or isolate speedups.

| Variant / seed | Tasks | Paired change | Final1000 | Mean entry ms | Max entry s | Peak RSS GB | Mean CPU cores /4 | Full minutes |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| trick_match10 /1 | 154,999 | +0 | 34,060 | 446.665 | 0.990719 | 11.945 | 1.507 | 38.21 |
| trick_match5 /1 | 154,791 | -208 | 34,004 | 447.164 | 0.999944 | 11.982 | 1.510 | 38.25 |
| trick_match10 /3 | 155,020 | +0 | 34,120 | 446.315 | 1.004091 | 11.943 | 1.507 | 38.17 |
| trick_match5 /3 | 154,828 | -192 | 33,999 | 448.791 | 0.990769 | 11.944 | 1.505 | 38.38 |

| Variant / seed | Oldest never assigned | Outstanding age p90 /max | First-half tasks incomplete | Initial never assigned | Max retargets | After-pickup assignments |
|---|---:|---:|---:|---:|---:|---:|
| trick_match10 /1 | 186 | 536 /1090 | 0 | 0 | 1 | 0 |
| trick_match5 /1 | 185 | 536 /1329 | 0 | 0 | 1 | 0 |
| trick_match10 /3 | 184 | 535 /1331 | 0 | 0 | 1 | 0 |
| trick_match5 /3 | 186 | 536 /1071 | 0 | 0 | 1 | 0 |

Ordinary HRRN and forced-oldest admission remain enabled, short preference remains OFF, and held/started/primary/recovery protections remain. Finite observations on solver seeds of one fixed input do not prove starvation freedom. Tail regressions are retained in the table.

| Candidate / seed | Empty robot-work change | Unfinished task-work change | Forward change | Turn change | Wait change |
|---|---:|---:|---:|---:|---:|
| trick_match5 /1 | -17,522 | +10,720 | -71,516 | +3,958 | +67,558 |
| trick_match5 /3 | -14,106 | +13,665 | -60,006 | +1,667 | +58,339 |

Unfinished-task work overlaps the empty/loaded and action partitions; never add these as separate costs. Changed task cohorts and trajectories prevent a causal savings interpretation. The assumed5000-step horizon and all Warehouse policies require `--trick WAREHOUSE`.

[Full verification and exact per-case configuration](verification.json), [fairness](fairness.json), [physical work](work-budget.json), [sampled policy work](work-samples.json).
