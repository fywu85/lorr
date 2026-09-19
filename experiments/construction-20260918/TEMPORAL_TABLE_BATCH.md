# Complete turn-table batches before temporal scoring

V40 adds optional CGAR_TEMPORAL_TABLE_BATCH (0–1024, default0) and
CGAR_TEMPORAL_TABLE_THREADS (1–32, default1). After the original complete PIBT
plan and protected-support closure, the planner ranks missing current goals by
request count, oldest ticket and goal ID. It constructs a fixed number of tables,
then admits every selected complete result before preparing temporal candidates.
No map identity, template or future task enters the ranking.

This is additional work and a distinct admission policy from the original
CGAR_TURN_BUILD_LIMIT demand allowance, which remains32 in this experiment.
The old speculative prefetch could discard unused results. Here all selected
results enter the cache deterministically. Existing oracle workers build chunks
of at most32 tables; all workers join before admission, and complete chunks are
admitted in rank order. Batch preparation holds no candidate pointers into cache
entries. Failure of a later chunk fails the entire entry/run; private cache state
from earlier chunks is not promised to roll back for continued execution.

Thread count participates in affinity validation. A fixed count is completed or
the deadline raises an explicit failure. Elapsed time never chooses an action
subset or reduces the batch. Diagnostic batch time is also included in the
existing candidate-preparation phase, so these timings must not be added twice.

Build8898749 passes the full regression suite. The new fixture compares12800
robot actions exactly between one and four build threads, exercising multiple
chunks, changed goals, five traffic publications and cache invalidations.
Existing primary, recovery and capacity regressions also run with the batch
policy enabled. The source patch independently reconstructs all20 recorded
production and test hashes.

The200-step screen8898752 passes disabled,128serial,128parallel and512parallel.
Disabled exactly reproduces the prior control. Batch128 produces identical
complete trajectory/task/assignment fingerprints with one and four threads;
wall time is61.816s versus50.273s. This is a timing/equivalence screen, not a
quality ranking. Maximum complete entry times are0.786/0.932/0.856/0.946s.
Batch1024 fails explicitly at timestep128, elapsed1000.641ms, stage
turn_distance_table_complete, and is excluded from the next full matrix.

Full8898754/8898755 compares disabled,128serial,128parallel and512parallel over
seeds0/2. All eight run concurrently on disjoint four-core allocations, with
128GiB aggregate reserved and a32decimalGB per-process limit. Full serial/
parallel fingerprints, whole-entry deadlines, memory, final windows and throughput
must be checked before choosing a setting. No production default is changed.

[Build archive](build-provenance/v40/),
[screen and failure evidence](results/temporal-table-batch-screen-v40/).
