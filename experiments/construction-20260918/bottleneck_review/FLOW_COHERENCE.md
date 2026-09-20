# Generic guidance coherence diagnostic

This is a read-only replay of the six current generic references plus the saved
local KittyKnight and NMS trajectories. No trajectory, task forecast or resulting
field is passed back into a planner. NMS remains a diagnostic run with one timeout.

The topology-only measurement is complete: maximal straight degree-two chains
contain24,360/38,586free cells (63.13%), and their incident edges comprise
34,120/51,199undirected movement edges (66.64%). There are9,760chains, with two or
three interior cells each. The incident edges include each chain's two exits to
junctions. Four independent topology fixtures pass.
[Coverage evidence](../results/warehouse-degree2-corridor-coverage.json).

The full traffic replay is queued as GRID8899278 on three exclusive physical
cores, eight traces with at most three concurrent processes. An initial
submission failed because all eligible hosts had their exclusive resource in
use. The identical frozen request was enqueued with immediate-availability
verification disabled; CPU binding, isolation and memory limits remain. The
failure and accepted retry are both preserved in
`runs/flow-coherence-v55-20260920`. No C++ replay result is available yet.

The [replay](flow_coherence.cpp) reconstructs the actual positions and headings,
checks every vertex/edge action, and counts movement crossings in both directions
on each undirected edge. The [driver](run_flow_coherence.py) requires all400million
actions and saved input hashes to match previous independent accounting.
It also reconstructs all60CGAR publication frames with the unchanged production
FlowGuidance, checks every published cost against a separate count formula, and
compares publication times/counts/moves/penalized edges with the production logs.
Seven hand-counted C++ topology/count fixtures must pass before accepting outputs.
These are required checks, not claims that the queued run has already passed.

Two traffic ratios are deliberately kept distinct:

- **Edge minority share:** sum each edge's smaller directional count, divided
  by all crossings over covered edges.
- **Chain minority share:** first pool both directional counts across a chain,
  then sum each chain's smaller count, using the same denominator.

The second can be larger when different edges in a chain favor opposite
directions. Neither is an avoidable-conflict estimate: robots can start or stop
inside chains, and opposite trips need not occur at the same time. Static cell
coverage is not movement coverage, so the latter is measured independently.

For CGAR, each publication reports mixed toll signs, adjacent sign reversals,
neutral gaps, traffic through mixed-sign chains, and actual subsequent crossings
against published tolls. Competitor fields are not reconstructed as if they were
learned CGAR fields; only their executed traffic is compared.

An offline counterfactual pools observed directional counts over a chain while
retaining reference margin25, strength4 and base4. Minimum evidence is kept at
eight observations **per edge on average**, requiring pooled support at least
eight times the number of edges. It records which tolls would change on the
observed trace. This is not a measured throughput benefit or an implemented
production policy. A positive signal would justify a controlled default-off
generic experiment, with rotation/reflection and complete-decision tests.

The primary generic frontier remains144,510single-run and143,941.2six-seed mean.
Full TRICK runs are a separate experiment and cannot replace those records.
