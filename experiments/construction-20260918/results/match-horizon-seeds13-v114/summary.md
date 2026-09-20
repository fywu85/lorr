# TRICK: matching cutoff guard, additional full seeds 1 and 3

The whole-cycle cutoff guard gains 57 tasks on seed 1 and loses 12 on seed 3. Together with the earlier seed 0/2 gains of 53/34, this is +33 tasks on average across four paired seeds, winning three of four. Retain the guard as a modest observed improvement; it is neither a uniform seed win nor a formal fairness guarantee. The best full single run remains 155173. Independent accounting was retried on binding-capable hosts after the first analysis allocation failed its CPU-affinity assertion before reading trajectories; the benchmark runs and frozen analysis logic were unchanged.

Verified 2026-09-20T21:17:51.132631+00:00. Source [37f592a](https://github.com/fywu85/lorr/commit/37f592a48bdb24b936f8d0be8be22ddd405b4ddb); binary `3f66eefd2a54589e48965b5b4031cbbe4d67bd8e6423c8cdc1cb3108f24bf641`.

Every case completes5000steps with10000robots, zero planner/scheduler errors and timeouts, and independently reconciled50million robot actions. All28source/test hashes, fixed work, distinct four-core bindings,5s entry deadline and32decimalGB RSS checks pass. Exact prior controls reproduce their complete trajectory hashes. Shared-host timings do not certify the competition1s limit or isolate speedups.

| Variant / seed | Tasks | Paired change | Final1000 | Mean entry ms | Max entry s | Peak RSS GB | Mean CPU cores /4 | Full minutes |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| trick_matchguard0 /1 | 154,999 | +0 | 34,060 | 447.286 | 0.989965 | 11.947 | 1.512 | 38.23 |
| trick_matchguard1 /1 | 155,056 | +57 | 34,117 | 448.010 | 0.998354 | 11.946 | 1.512 | 38.30 |
| trick_matchguard0 /3 | 155,020 | +0 | 34,120 | 447.763 | 0.988160 | 11.948 | 1.509 | 38.28 |
| trick_matchguard1 /3 | 155,008 | -12 | 34,108 | 444.914 | 0.991890 | 11.940 | 1.512 | 38.03 |

| Variant / seed | Oldest never assigned | Outstanding age p90 /max | First-half tasks incomplete | Initial never assigned | Max retargets | After-pickup assignments |
|---|---:|---:|---:|---:|---:|---:|
| trick_matchguard0 /1 | 186 | 536 /1090 | 0 | 0 | 1 | 0 |
| trick_matchguard1 /1 | 182 | 535 /1071 | 0 | 0 | 1 | 0 |
| trick_matchguard0 /3 | 184 | 535 /1331 | 0 | 0 | 1 | 0 |
| trick_matchguard1 /3 | 185 | 533 /1331 | 0 | 0 | 1 | 0 |

Ordinary HRRN and forced-oldest admission remain enabled, short preference remains OFF, and held/started/primary/recovery protections remain. Finite observations on solver seeds of one fixed input do not prove starvation freedom. Tail regressions are retained in the table.

| Candidate / seed | Empty robot-work change | Unfinished task-work change | Forward change | Turn change | Wait change |
|---|---:|---:|---:|---:|---:|
| trick_matchguard1 /1 | +1,346 | -17,697 | -1,039 | +181 | +858 |
| trick_matchguard1 /3 | +2,195 | -3,450 | +2,358 | -317 | -2,041 |

Unfinished-task work overlaps the empty/loaded and action partitions; never add these as separate costs. Changed task cohorts and trajectories prevent a causal savings interpretation. The assumed5000-step horizon and all Warehouse policies require `--trick WAREHOUSE`.

[Full verification and exact per-case configuration](verification.json), [fairness](fairness.json), [physical work](work-budget.json), [sampled policy work](work-samples.json).
