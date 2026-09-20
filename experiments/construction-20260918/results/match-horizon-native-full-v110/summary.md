# TRICK: late matching-cycle guard, full seeds0/2

The optional whole-cycle cutoff-tier guard improves both paired full totals:155173/155090 versus155120/155056 (+53/+34; mean+43.5). Seed0 sets a new verified best155173,378above the154795NMS target. All extra completions appear in the final1000-step counts. Four-seed confirmation remains pending; this first pair does not establish uniform tail improvement. At sampled step4800 the guard has vetoed107/119accepted cycles containing128/139worsening task rows. Each veto restores the whole original cycle and leaves independent accepted cycles intact. The guard uses prospective estimates and resident spatial/Manhattan quotes, never future outcomes. Exact best profile and source/hash are in best-record.json and best-variant.json.

Verified 2026-09-20T20:37:17.124271+00:00. Source [37f592a](https://github.com/fywu85/lorr/commit/37f592a48bdb24b936f8d0be8be22ddd405b4ddb); binary `3f66eefd2a54589e48965b5b4031cbbe4d67bd8e6423c8cdc1cb3108f24bf641`.

Every case completes5000steps with10000robots, zero planner/scheduler errors and timeouts, and independently reconciled50million robot actions. All28source/test hashes, fixed work, distinct four-core bindings,5s entry deadline and32decimalGB RSS checks pass. Exact prior controls reproduce their complete trajectory hashes. Shared-host timings do not certify the competition1s limit or isolate speedups.

| Variant / seed | Tasks | Paired change | Final1000 | Mean entry ms | Max entry s | Peak RSS GB | Mean CPU cores /4 | Full minutes |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| trick_matchguard0 /0 | 155,120 | +0 | 34,057 | 447.157 | 0.980804 | 11.947 | 1.505 | 38.26 |
| trick_matchguard1 /0 | 155,173 | +53 | 34,110 | 448.763 | 0.991801 | 11.947 | 1.511 | 38.36 |
| trick_matchguard0 /2 | 155,056 | +0 | 34,053 | 447.514 | 0.996538 | 11.952 | 1.507 | 38.25 |
| trick_matchguard1 /2 | 155,090 | +34 | 34,087 | 446.554 | 1.003767 | 11.932 | 1.509 | 38.17 |

| Variant / seed | Oldest never assigned | Outstanding age p90 /max | First-half tasks incomplete | Initial never assigned | Max retargets | After-pickup assignments |
|---|---:|---:|---:|---:|---:|---:|
| trick_matchguard0 /0 | 188 | 533 /1078 | 0 | 0 | 1 | 0 |
| trick_matchguard1 /0 | 189 | 533 /1072 | 0 | 0 | 1 | 0 |
| trick_matchguard0 /2 | 193 | 536 /1044 | 0 | 0 | 1 | 0 |
| trick_matchguard1 /2 | 187 | 536 /1071 | 0 | 0 | 1 | 0 |

Ordinary HRRN and forced-oldest admission remain enabled, short preference remains OFF, and held/started/primary/recovery protections remain. Finite observations on solver seeds of one fixed input do not prove starvation freedom. Tail regressions are retained in the table.

| Candidate / seed | Empty robot-work change | Unfinished task-work change | Forward change | Turn change | Wait change |
|---|---:|---:|---:|---:|---:|
| trick_matchguard1 /0 | +6,861 | -15,349 | +7,358 | -696 | -6,662 |
| trick_matchguard1 /2 | +4,181 | -9,769 | +1,968 | -1,041 | -927 |

Unfinished-task work overlaps the empty/loaded and action partitions; never add these as separate costs. Changed task cohorts and trajectories prevent a causal savings interpretation. The assumed5000-step horizon and all Warehouse policies require `--trick WAREHOUSE`.

[Full verification and exact per-case configuration](verification.json), [fairness](fairness.json), [physical work](work-budget.json), [sampled policy work](work-samples.json).
