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

Full GRID8899258 plus independent analysis8899259 now completes seeds0/2 at
144,510/144,107tasks, exactly matching both reference trajectories and all
recorded global/regional/preparation work counters. All10,000complete entries
meet1s; processRSS stays below11.334GB. The22source/test hashes match
`f1c3c8abc4717ec921ca6e241366b63f77cea3e3`. No production repair policy changed.
[Full verification and exhaustive witnesses](../results/group-snapshot-full-v52/audit.json).

Each seed records74frames and1,184groups, split592/592 between root-plus-one and
root-plus-two owners. Across both seeds:

| Group size | Sampled groups | Any feasible forward replacement | Scalar improvement | Scalar + aggregate potential improvement |
|---|---:|---:|---:|---:|
| 2robots |1,184|560|37|22|
| 3robots |1,184|466|32|22|

Thus44/2,368sampled groups (1.86%) have a replacement improving both scores.
In the final1,000steps this is8/480groups (1.67%). Feasibility alone is much more
common; the tradeoff with displaced robots' costs removes most candidates.
These are stratified samples of qualifying stationary roots, not population
prevalence or a throughput bound. The aggregate potential includes guidance,
not observed physical work. Candidate0, search order and rollback/depth rules
mean a witness may be unavailable to the production DFS. Larger groups, changed
outside reservations and longer horizons remain outside this diagnostic.

The evidence lowers the priority of another tiny-group pass relative to testing
guidance and fresh-task coverage; it neither proves a repair ceiling nor claims
an achieved throughput gain.
