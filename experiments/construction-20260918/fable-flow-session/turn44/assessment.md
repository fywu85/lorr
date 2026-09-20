# Fable turn44: deferred-selection equivalence review

Completed2026-09-20T13:37:14.221930UTC, same persistent Fable5.1/max Claude CLI
session1ebb1075-3538-49d1-93d1-a00c94fa256a. Read-only review of the exact isolated
patch and complete constructor/private-search context. No concrete counterexample
found, including construction recursion that revisits an ancestor, rollback,
score arithmetic, RNG draws, aliases and callback exceptions.

Accepted invariant: at blocker queries the current robot is unreserved; every
reserved robot has the same selected path in both versions. No skipped selection
store is observed before the next accepted reservation, recursive call or restore.
The review's phrase "at most one robot unreserved at any moment" is too broad:
remove(other) then add(r) briefly leaves two unreserved, without an intervening
query. The required invariant holds at query/call boundaries. Both call sites
were changed and the old blocker overload removed; complete V93 build and
regression suite pass (sourcebdf5e7a, all28source/test hashes exact).

All128 component timing observations preserve plan/score-bit/work checksums and
independent validity. General800-step and native same-core A/B/A200-step checks
both PASS13:44:29UTC with exact trajectories; all sampled native work counters and
scores are identical. Test-only construction-ancestor coverage PASS13:42:05UTC:
all eight settings contain9–9933actual recursive calls into an active ancestor,
with exact paired plans/score bits/work counts. The test
instrumentation never enters production and its timing is not performance data.
No full-run latency or throughput gain is claimed from component CPU savings.
