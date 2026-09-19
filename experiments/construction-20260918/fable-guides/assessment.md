# Assessment of the emitted Fable findings

The read-only Claude Code run initialized `claude-fable-5-1` with effort max,
no tools, and only the verified-public 00bada6 source payload. It emitted six
findings but ended with `error_max_budget_usd`, not a successful final result.
The CLI reported $17.07397 against a configured $12 budget. Its reported spend
exceeded that setting, so it did not provide an exact spending cap here. No retry
was submitted.
Only visible findings and status/usage metadata are retained here; hidden
reasoning is not published. This is an incomplete external review, followed by
local verification, not a clean external-review sign-off.

1. **Waypoint crossing: independently reproduced and fixed.** On a straight
   corridor, start 5, waypoint 8, goal 40, the old guide assigns FFFFF a remaining
   distance of 38 and FFFWW a distance of 32. Their actual production costs are
   1772 and 1478, so it prefers parking. The corrected local multi-source metric
   gives distances 30 and 32 and costs 1372 and 1478, preferring forward progress.
   The standalone GRID probe compiles frozen old/corrected headers; it does not
   claim a throughput effect. New regressions cover 3,912 oriented states, four
   rotated forward-over-parking rankings, and an independent obstacle-graph BFS.
   The correction seeds all remaining in-box route states with their suffix
   length. Sorted seeds merged with unit-edge FIFO expansion compute the complete
   field without a priority heap. Internal storage uses int distances to avoid
   truncating potentially long suffix offsets.
2. **Sticky routes: plausible performance mechanism; optional experiment added.**
   V25 introduces a separate fixed-count round-robin refinement batch, default
   off. It excludes exact self-flow without mutating shared counts, accepts only
   a complete strictly cheaper route under current costs, and retains the old
   route after a limited search. Tests remove a now-unnecessary five-action
   detour, compare an independent shortest path, check conservation, fairness,
   equal-cost retention, stale goals/protection, and interrupted-search safety.
   Throughput improvement remains unmeasured.
3. **Candidate-work scope: true, but not the reported failure's cause.** The
   current limit applies to global repair only; regional calls have their own
   prescribed attempt counts and no candidate threshold. V23/v24 failing guide
   profiles have regions disabled. The follow-up never claimed a regional cap;
   all attempts, construction and worker joins must still complete. An individual
   attempt may overshoot the threshold. Per-region limits remain a possible
   future composition experiment, not an implemented fix or proven necessity.
4. **Repeated limited A*: mechanism confirmed by code, benefit unmeasured.**
   Missing routes can be retried after every admission rotation. Weight-2 route
   search greatly reduces limits in the observed screens, but this is not a
   proof that every later state is cheap. No backoff has been added yet.
5. **Concentrated routes/tie-breaking: hypothesis.** Neither salted ties nor a
   positive default load cost has been adopted. The corrected metric must be
   evaluated before attributing conflict work to route concentration. Existing
   load-cost screens changed both traffic and search work, not only tie-breaking.
6. **Warm reuse after guide changes: hypothesis.** Warm suffixes remain physically
   valid but can be suboptimal under a changed guide. Guide full profiles keep
   warm starts off; no reset-mask change is included in this revision.

The first corrected-window build revealed another integration issue: descending
its route-to-go field can travel alongside a route toward a distant join and
exhaust the connector limit despite a short nearby join. The existing
reconnection regression caught this. V26-r1 uses a bounded unit-action BFS to a
remaining route state with its outgoing orientation (any orientation at the
final goal), inside the cached local box. Preparation finishes before replacing
counts or paths. The original reconnection test remains unchanged and now passes.
The full regression suite also passes, including nonzero refinements and identical
serial/four-thread production actions with warm starts, plus protected primary,
pocket and capacity fixtures. No full-run guide throughput gain is yet established.

The review's broad invariants are not treated as proofs. Acyclic orientation-state
parent chains alone do not prove that a path cannot revisit a cell in a different
orientation. Likewise, a deadline can interrupt construction of an internal guide
window; the guaranteed behavior is failure of the entire decision without an
action result, not a transaction over all mutable guide state.

[Visible findings](emitted-findings.md), [actual status](status.json),
[usage and frozen source metadata](metadata.json),
[independent scoring probe](../results/guide-window-counterexample-v26/).
