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

The full replay completed on GRID8899278 at2026-09-20T03:22:11UTC using three
exclusive physical cores. All400million actions match independent accounting;
all vertex/edge actions are valid; all60generic publications match production
counters and independently recomputed edge costs; seven C++ fixtures pass.
The initial availability rejection and accepted isolated retry are preserved.
[Verification](../results/flow-coherence-v55/verification.json).

Across all60publications there are **zero mixed toll signs within a short chain**
and zero adjacent sign reversals. At the final publication, pooling would change
only202–240directed edge costs. Historical crossings over those changed directions
are0.036–0.044%of corridor crossings. These are historical crossings, not a
counterfactual trajectory or a bound on a possible effect. The result does not
support implementing short-chain pooling now.

Actual movement coverage is70.92–74.07%for generic CGAR,82.06%for KittyKnight and
77.08%for the diagnostic NMS run. Generic chain-minority shares are3.24–3.59%over
the full run and1.20–1.35%in the final1000steps; KittyKnight has0%, NMS about0.18%.
Every edge-minority numerator equals its chain-minority numerator on these saved
traces. This does not establish simultaneous opposing traffic or wasted work.
[Per-seed summary](../results/flow-coherence-v55/summary.json).

The [replay](flow_coherence.cpp) reconstructs actual positions and headings;
the [driver](run_flow_coherence.py) checks hashes, independent action accounting,
and publication counters. The chains have only2or3interior cells, so broader
coordination through junctions remains an open question. Epoch-level counts,
simultaneous heading exposure and maximal straight runs across junctions are
possible follow-ups; none has been measured by this replay.

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
production policy. This completed replay provides no compelling pooling signal, so no production
pooling experiment is being implemented.

The primary generic frontier remains144,510single-run and143,941.2six-seed mean.
Full TRICK runs are a separate experiment and cannot replace those records.
