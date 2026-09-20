# Complete small-group counterfactual

V52 adds a default-off, read-only snapshot of root plus one/two blocking owners.
Every member's candidate path, cost, operation, power and current selection is
recorded, together with cell and undirected-edge reservation owners, including
outsiders. Protected roots/owners are excluded. Capturing does not modify the
search state, RNG or candidate order. Diagnostics remain inside the complete
one-second entry deadline and propagate failure instead of returning partial work.

The capture samples at most8 roots per owner class every67steps in this experiment,
rotating the starting robot index deterministically. Roots are stationary in the
first slot and have a cheaper first-forward candidate with a full scaled
travel/turn-potential improvement after removing the operation preference term.
This is a stratified diagnostic sample, not an unbiased population estimate.

The [offline solver](joint_probe.cpp) filters choices conflicting with outside
reservations, checks pairwise cell/edge compatibility, and enumerates every
remaining complete product. It reports forward feasibility, weighted scalar
improvement and aggregate travel/turn-potential improvement separately. The
potential still includes guidance; it is not observed future travel time.
Candidate0 remains in owner domains. No claim about production DFS reachability
or future throughput follows from a witness. A null covers only this group,
these candidates, the five-slot horizon and these fixed outside reservations.

GRID build8899253 passes all CGAR regressions, including hand-counted one-owner,
two-owner, outside-owner and edge-only snapshots, fixed protection, deep copies,
timeout propagation, unchanged subsequent RNG/search, and4,800 serial/parallel
integration actions. [Build evidence](../build-provenance/v52/validation.json).

GRID screen8899255 runs snapshot-off/on on disjoint four-core EPYC9354 allocations.
Both complete200steps/2,888tasks with identical full trajectories:
`a4bfe67f610e2451bb40e68da0e9fad0b75985d3f7701528334fee73f93cea8b`.
All400complete entries meet1s; maxima936.241/931.907ms; peakRSS5.269GB.
This is an instrumentation screen, not a new performance score or a speedup.
[Screen evidence](../results/group-snapshot-screen-v52/metrics.json).

GRID offline probe8899257 passes six independent hand-counted fixtures:
complete joint improvement, an outside reservation, a joint vertex collision,
a joint edge collision, scalar versus potential improvement, and a protected member.
All32screen groups at steps67/134 are exhaustively checked. Among16two-robot
groups, none has a complete qualifying forward replacement. Among16three-robot
groups, two have one, one improves scalar score, and none improves both scalar
and aggregate potential by the required threshold. This early sample is not a
full-run or population conclusion. [Probe evidence](../results/joint-probe-screen-v52/summary.json).

The next step is full5,000-step seeds0/2 with capture enabled, exact reference
trajectory and work-counter verification, then the same exhaustive analysis.
No production repair policy has changed.
