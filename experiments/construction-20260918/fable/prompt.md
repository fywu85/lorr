Please independently review the new CGAR temporal implementation for correctness
and the next useful generic improvements. Use Claude Fable 5.1 at max effort.
This is a read-only review of a concrete frozen prototype; do not edit or run code.

Read these first:
- experiments/construction-20260918/PROBE.md
- runs/cgar-temporal-build-v4-20260918/sources/cgar/cgar_planner/temporal_pibt.hpp
- runs/cgar-temporal-build-v4-20260918/sources/cgar/cgar_planner/temporal_geometry.hpp
- runs/cgar-temporal-build-v4-20260918/sources/cgar/cgar_planner/temporal_adapter.hpp
- runs/cgar-temporal-build-v4-20260918/sources/cgar/cgar_planner/cgar.cpp
- runs/cgar-temporal-build-v4-20260918/sources/cgar/tests/cgar_regression.cpp
- tools/benchmark_cgar.py and tools/benchmark_matrix.py for explicit core groups
- cgar/src/Entry.cpp and cgar/src/CompetitionSystem.cpp for deadline/timing boundaries

Context: warehouse only, full 5,000 steps / 10,000 robots. Generic rules only:
no map-name/category switches, aisle templates, supplied map weights or fleet
caps. Total process memory must stay below 32,000,000,000 bytes. A prescribed
fixed amount of work must complete; elapsed-time cuts are explicit Timeout,
never successful partial search. Every portfolio worker must finish before
selection by score and deterministic worker-index tie break.

The 8 GiB orientation-cache control scores 43,176 on six seeds. Full v1 seeds
0/1/2 with 10,000 search attempts score 89,093 / 89,518 / 89,006; with 50,000,
104,573 / 104,788 / 104,523. All report zero planner errors/timeouts and about
15.2 GiB peak RSS. A 150,000-attempt profile explicitly times out at decision
29/early decisions and has NO accepted throughput score. Four-worker full runs
and generic equal-weight/remaining-task-priority ablations are underway.

There is a timing measurement issue under investigation: some legacy
plannerTimes entries are about 29 seconds, despite zero external or internal
timeouts. BaseSystem::simulate times plan(), which logs "planner returns" after
future.get() and before returning. Entry already checks the actual deadline
before completion. We are adding a separate precise entry completion series
and retaining the outer timing series rather than deleting anomalous entries.
Do not presume the anomaly is a planner deadline failure or definitively claim
the precise source of the delay without evidence.

Direct comparison against frozen native NMS matches 51,600 geometry cases and
15,360 robot decisions across construction and 256-step repair. Native scores
and order are supplied for that kernel comparison; the full CGAR adapter has
additional primary/recovery constraints and bounded/cold caches. A regression
matches serial and parallel execution of four fixed workers on 1,920 robot
decisions. These finite tests are not a liveness proof.

Focus on: primary unfinished target and recovery protection; temporal-to-spatial
commitments; rollback and cycles; thread safety and deadline propagation; CPU
allocation correctness. Then suggest at most five high-value, map-independent
experiments for closing the remaining gap to the valid local KittyKnight score
of 152,981 (that reference uses 16 physical cores and exceeds our 32 GB target).
NMS's native warehouse policy uses equal robot weights and a 32-worker search;
the uniform reference sets map/test type NONE, so its rank weights differ. The
new equal-weight option is global and has no map-dependent branch.

Give concrete findings with file/line evidence and severity, distinguish proven
bugs from hypotheses, and identify what would substantiate performance claims.
Keep the review focused on this implementation and the current evidence.
