# Warehouse: exact best trajectories within the strict one-second limit

The execution profile reproduces the full prior 155173 and 155090 trajectories and sampled search work at an enforced1000ms deadline. Eight-thread prefetch/preparation/pickup and a25GiB logical unwarmed cache preserve throughput. Two full runs qualify on the exclusive host; this is an execution confirmation, not a new throughput peak or a guarantee for untested seeds and hardware.

Verified 2026-09-20T23:23:31.935423+00:00. Source [3e319f1](https://github.com/fywu85/lorr/commit/3e319f175859396b91f97364b320c6794a85cbb7); binary `e3ec32ecdfdaf06f7db139da3857af8fa0ad43b98bd67479f7b9bd969dc1302c`.

Every case completes5000steps with10000robots, zero planner/scheduler errors and timeouts, and independently reconciled50million robot actions. All28source/test hashes, fixed work, distinct 8-core bindings,1000ms entry deadline and32decimalGB RSS checks pass. Exact prior controls reproduce their complete trajectory hashes. The complete runs pass the enforced competition1s entry limit on an exclusive host. This validates these runs, not every seed or machine; timing differences do not isolate individual optimizations.

| Variant / seed | Tasks | Paired change | Final1000 | Mean entry ms | Max entry s | Peak RSS GB | Mean CPU cores /8 | Full minutes |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| trick_runtime_combined8 /0 | 155,173 | +0 | 34,110 | 428.056 | 0.944766 | 15.396 | 1.770 | 36.83 |
| trick_runtime_combined8 /2 | 155,090 | +0 | 34,087 | 425.740 | 0.957827 | 15.393 | 1.770 | 36.63 |

| Variant / seed | Oldest never assigned | Outstanding age p90 /max | First-half tasks incomplete | Initial never assigned | Max retargets | After-pickup assignments |
|---|---:|---:|---:|---:|---:|---:|
| trick_runtime_combined8 /0 | 189 | 533 /1072 | 0 | 0 | 1 | 0 |
| trick_runtime_combined8 /2 | 187 | 536 /1071 | 0 | 0 | 1 | 0 |

Ordinary HRRN and forced-oldest admission remain enabled, short preference remains OFF, and held/started/primary/recovery protections remain. Finite observations on solver seeds of one fixed input do not prove starvation freedom. Tail regressions are retained in the table.

| Candidate / seed | Empty robot-work change | Unfinished task-work change | Forward change | Turn change | Wait change |
|---|---:|---:|---:|---:|---:|

Unfinished-task work overlaps the empty/loaded and action partitions; never add these as separate costs. Changed task cohorts and trajectories prevent a causal savings interpretation. The assumed5000-step horizon and all Warehouse policies require `--trick WAREHOUSE`.

[Full verification and exact per-case configuration](verification.json), [fairness](fairness.json), [physical work](work-budget.json), [sampled policy work](work-samples.json).
