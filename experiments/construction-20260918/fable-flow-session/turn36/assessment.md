# Assessment of persistent Fable turn36

Completed 2026-09-20T09:50:35.536786 UTC, same Fable5.1/max session, no tools or
permission denials. This is a read-only source review, not independent execution.
Fable found no visible correctness failure in the native metric or raw score.

Local checks and actions:

- Parallel reverse-oracle scratch is allocated inside `prefetch` using the current
  `max_edge_cost_`, after field installation. The suspected stale bucket sizing is
  absent. New tests compare demanded prefetch tables against the independent heap
  at1/4threads under both native fields, including the20→200/201 transition.
- `TemporalChoice::cost` and the scalar are already int64. The fallback is clamped
  below kInf before multiplication; no32-bit stored-score narrowing was found.
- Matching's unit ceiling16 is a real compatibility blocker, but unreachable in
  the frozen V73 experiment because native+matching is rejected. V75 introduces
  an explicit optional capability while preserving default16, supplies native20
  only at the guarded call site, and adds scaling/default-bound/real-cycle tests.
  No existing full result is invalidated by this previously unreachable path.
- Runtime validation now fingerprints the actual native vector before installation
  against the independent generated FNV constant. The receipt reports that value.
  Corrupted/wrong-size vectors must fail. Static metric scale error text is fixed.
- Missing op0 coverage is added for all four real first actions, with independent
  replay. Four128-tick episodes with24robots compare serial/parallel preparations,
  collisions, one-service-per-tick, repeated errands, started-task protection,
  wait-seed rotations, complete work and absence of learned publications.
- Table fallback counts already exist in temporal preparation diagnostics. They
  count robots at the checked preparation, not every candidate distance read.
- The wide-table counter is cumulative admissions (including rebuilds), not live
  tables or distinct goals. README interpretation remains explicit.

The no-flag800-step control is exactly unchanged. Its observed maximum889.575ms
and wall336.491s versus the prior matching64 screen917.087ms/wall338.688s do not
show an obvious slowdown, but these runs used shared hosts at different times.
That is not a controlled timing effect. Replacing constant modulo with runtime
modulo may cost cycles; no speed claim or hot-loop rewrite is justified by these
aggregate timings alone. A targeted optimization can follow the best full policy.

Native scoring remains a compound metric, tie, service, fallback, idle, pickup and
chain-unit change. Old idle scoring and native idle scoring differ, as intended.
A subsequent local check matches all129 operation strings in order to the archived
NMS44950d1 source; see operation-order.json. Overall policy equivalence is not
claimed. Liveness still depends on the unchanged CGAR protection/recovery layer.

V75 build8899476 passes the full suite:1,234,752 additional prefetched state checks,
2,372 baseline seed macros, two real native matching cycles, and four closed-loop
episodes with276services,111repeated errands and125wait-seed rotations. V73/V74 comparisons continue
using frozen older source/binaries and immutable analysis snapshots.
