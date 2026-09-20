# Read-only current best work audit

`run.py` freezes the existing independent `decompose.py` / `decompose.cpp`,
verified full V110 inputs and supporting accounting, then runs on one bound
GRID core. It changes no solver or benchmark input. Require five hand-counted
fixtures, exact original trajectory-file hashes, exact production phase/action
counts and matching independent unfinished/empty work. It groups shortest-path
queries by goal and uses complete unweighted BFS on the original free graph.

V121 passed on20September2026 at21:43:41UTC. See
[the refreshed result](../results/current-best-work-audit-v121/summary.md).
The saved NMS figures are diagnostic only: that run had a timeout and exceeded
our memory budget. Different completed-task cohorts prevent causal attribution
or a claim that any component gap can be fully recovered.
