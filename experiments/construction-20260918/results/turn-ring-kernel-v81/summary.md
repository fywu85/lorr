# General implementation: backward Dial relative bucket indexing

Replace runtime remainder with a current ring index and one bounded subtraction.
Every positive edge is below the bucket count, so the selected bucket and LIFO
order are unchanged. Public initialization, field replacement and parallel
prefetch all allocate from the current maximum edge cost. Source85faca5, compared
with fd2d23b. The complete production regression suite passes, including independent
native distances, serial/parallel tables, wide storage, eviction and closed-loop
service/action checks.

One bound AMD EPYC9354 core,24alternating timed pairs per profile after two warmups.
Production compute bodies are mechanically extracted and source-hash bound;
only their class/method names change in the paired harness. All14,817,024compared
state distances and deadline-check counts agree. The immutable harness treats
Warehouse free cells as core; actual certificate handling is tested separately.

| Field | Before median CPU ms | After median CPU ms | Change |
|---|---:|---:|---:|
|uniform1_bfs_control|0.838370|0.844910|+0.78%|
|legacy4_16_turn4|1.545921|1.397305|-9.61%|
|native20_200_turn1|1.772036|1.536385|-13.30%|
|native20_200_bands_turn1|1.849716|1.556555|-15.85%|

Weighted table kernels improve9.6–15.8%; the untouched uniform BFS varies+0.8%.
These timings exclude cache management/compression and do not establish a full
step latency or throughput gain. Native and generic trajectory controls are queued.
The queued job's host syntax correction is archived; source/work/resources were
unchanged, and actual execution has one bound core and no CPU quota.
[Paired measurements](comparison.json), [exact source binding](exact-source-commit.json),
[production build](../../build-provenance/v81/exact-source-commit.json).
