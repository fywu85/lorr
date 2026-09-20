# TRICK: native turn price 4 with the late matching guard

With turn price 4, the matching guard changes seeds 0/2 by +69 / -2, reaching 155,164 / 155,144. Relative to the guarded turn-1 reference, these are -9 / +54: a mixed two-seed comparison and no new record. Keep the confirmed turn-1 profile; the individual gains do not add predictably.

Verified 2026-09-20T22:04:46.018307+00:00. Source [dac3fc7](https://github.com/fywu85/lorr/commit/dac3fc75cb6a149b3e9a6b4770b64170ead87ed9); binary `4f194edc5a806ae31198ef5ff29b995372b2e92bf266f490c010dbf0574481a5`.

Every case completes5000steps with10000robots, zero planner/scheduler errors and timeouts, and independently reconciled50million robot actions. All28source/test hashes, fixed work, distinct 4-core bindings,5s entry deadline and32decimalGB RSS checks pass. Exact prior controls reproduce their complete trajectory hashes. Shared-host timings do not certify the competition1s limit or isolate speedups.

| Variant / seed | Tasks | Paired change | Final1000 | Mean entry ms | Max entry s | Peak RSS GB | Mean CPU cores /4 | Full minutes |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| trick_matchguard0 /0 | 155,095 | +0 | 34,105 | 450.504 | 0.998513 | 11.922 | 1.500 | 38.55 |
| trick_matchguard1 /0 | 155,164 | +69 | 34,174 | 450.589 | 1.000625 | 11.921 | 1.503 | 38.57 |
| trick_matchguard0 /2 | 155,146 | +0 | 34,120 | 451.714 | 1.004164 | 11.933 | 1.500 | 38.65 |
| trick_matchguard1 /2 | 155,144 | -2 | 34,118 | 453.462 | 1.012842 | 11.948 | 1.503 | 38.80 |

| Variant / seed | Oldest never assigned | Outstanding age p90 /max | First-half tasks incomplete | Initial never assigned | Max retargets | After-pickup assignments |
|---|---:|---:|---:|---:|---:|---:|
| trick_matchguard0 /0 | 188 | 535 /1092 | 0 | 0 | 1 | 0 |
| trick_matchguard1 /0 | 175 | 533 /1071 | 0 | 0 | 1 | 0 |
| trick_matchguard0 /2 | 189 | 533 /1336 | 0 | 0 | 1 | 0 |
| trick_matchguard1 /2 | 181 | 533 /1336 | 0 | 0 | 1 | 0 |

Ordinary HRRN and forced-oldest admission remain enabled, short preference remains OFF, and held/started/primary/recovery protections remain. Finite observations on solver seeds of one fixed input do not prove starvation freedom. Tail regressions are retained in the table.

| Candidate / seed | Empty robot-work change | Unfinished task-work change | Forward change | Turn change | Wait change |
|---|---:|---:|---:|---:|---:|
| trick_matchguard1 /0 | +2,485 | -18,891 | +218 | +692 | -910 |
| trick_matchguard1 /2 | +3,601 | -6,070 | -155 | +199 | -44 |

Unfinished-task work overlaps the empty/loaded and action partitions; never add these as separate costs. Changed task cohorts and trajectories prevent a causal savings interpretation. The assumed5000-step horizon and all Warehouse policies require `--trick WAREHOUSE`.

[Full verification and exact per-case configuration](verification.json), [fairness](fairness.json), [physical work](work-budget.json), [sampled policy work](work-samples.json).
