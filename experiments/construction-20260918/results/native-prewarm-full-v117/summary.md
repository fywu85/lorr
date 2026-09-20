# TRICK: complete native prewarming, full seeds 0 and 2

Reject prewarming as a throughput change: full totals 155,135 / 154,767 lose 38 / 323 tasks. The larger unwarmed cache exactly reproduces both 155,173 / 155,090 baseline trajectories. Keep prewarming OFF and retain the current record. Prewarming lowers observed entry peaks to 0.7474 / 0.7466 seconds but changes trajectories; larger-cache timing differences on shared hosts are not an isolated speedup. All arms fit within 32 GB.

Verified 2026-09-20T22:12:20.442280+00:00. Source [3e319f1](https://github.com/fywu85/lorr/commit/3e319f175859396b91f97364b320c6794a85cbb7); binary `e3ec32ecdfdaf06f7db139da3857af8fa0ad43b98bd67479f7b9bd969dc1302c`.

Every case completes5000steps with10000robots, zero planner/scheduler errors and timeouts, and independently reconciled50million robot actions. All28source/test hashes, fixed work, distinct 8-core bindings,5s entry deadline and32decimalGB RSS checks pass. Exact prior controls reproduce their complete trajectory hashes. Shared-host timings do not certify the competition1s limit or isolate speedups.

| Variant / seed | Tasks | Paired change | Final1000 | Mean entry ms | Max entry s | Peak RSS GB | Mean CPU cores /8 | Full minutes |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| trick_prewarm_base /0 | 155,173 | +0 | 34,110 | 466.331 | 0.995564 | 11.949 | 1.512 | 40.04 |
| trick_prewarm_cache /0 | 155,173 | +0 | 34,110 | 443.041 | 0.985542 | 15.317 | 1.535 | 38.10 |
| trick_prewarm_all /0 | 155,135 | -38 | 34,085 | 431.230 | 0.747373 | 19.694 | 1.587 | 37.51 |
| trick_prewarm_base /2 | 155,090 | +0 | 34,087 | 462.029 | 0.993628 | 11.968 | 1.514 | 39.68 |
| trick_prewarm_cache /2 | 155,090 | +0 | 34,087 | 445.368 | 0.986335 | 15.313 | 1.536 | 38.29 |
| trick_prewarm_all /2 | 154,767 | -323 | 34,000 | 434.004 | 0.746602 | 19.692 | 1.591 | 37.75 |

| Variant / seed | Oldest never assigned | Outstanding age p90 /max | First-half tasks incomplete | Initial never assigned | Max retargets | After-pickup assignments |
|---|---:|---:|---:|---:|---:|---:|
| trick_prewarm_base /0 | 189 | 533 /1072 | 0 | 0 | 1 | 0 |
| trick_prewarm_cache /0 | 189 | 533 /1072 | 0 | 0 | 1 | 0 |
| trick_prewarm_all /0 | 181 | 535 /1016 | 0 | 0 | 1 | 0 |
| trick_prewarm_base /2 | 187 | 536 /1071 | 0 | 0 | 1 | 0 |
| trick_prewarm_cache /2 | 187 | 536 /1071 | 0 | 0 | 1 | 0 |
| trick_prewarm_all /2 | 184 | 536 /1065 | 0 | 0 | 1 | 0 |

Ordinary HRRN and forced-oldest admission remain enabled, short preference remains OFF, and held/started/primary/recovery protections remain. Finite observations on solver seeds of one fixed input do not prove starvation freedom. Tail regressions are retained in the table.

| Candidate / seed | Empty robot-work change | Unfinished task-work change | Forward change | Turn change | Wait change |
|---|---:|---:|---:|---:|---:|
| trick_prewarm_cache /0 | +0 | +0 | +0 | +0 | +0 |
| trick_prewarm_all /0 | -1,620 | +11,780 | -7,873 | -210 | +8,083 |
| trick_prewarm_cache /2 | +0 | +0 | +0 | +0 | +0 |
| trick_prewarm_all /2 | -6,242 | +8,472 | -92,421 | +5,116 | +87,305 |

Unfinished-task work overlaps the empty/loaded and action partitions; never add these as separate costs. Changed task cohorts and trajectories prevent a causal savings interpretation. The assumed5000-step horizon and all Warehouse policies require `--trick WAREHOUSE`.

[Full verification and exact per-case configuration](verification.json), [fairness](fairness.json), [physical work](work-budget.json), [sampled policy work](work-samples.json).

The prewarm component takes 23.576560 / 23.476961 seconds and completes during the unchanged 30-second initialization limit. Both all-goal runs retain all 38,586 tables and use map data only; the first 10,000 task assignments are unchanged. Actual peak process RSS grows to **19.694 / 19.692 GB** over the full runs, above the 15.957 GB startup-screen measurement. These are eight-core allocations for initialization; ordinary entry work still uses one global worker and four preparation, regional and pickup threads.

The 25 GiB logical cache without prewarming peaks at 15.317 / 15.313 GB and preserves full trajectory hashes, all action counts and fairness results. It is a candidate for isolated runtime measurement, with no throughput improvement claimed and no change to the four-core canonical record configuration.
