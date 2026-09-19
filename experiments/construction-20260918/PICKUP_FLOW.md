# Heading and traffic costs for pickup selection

The optional `CGAR_PICKUP_FLOW=1` policy searches for nearby unassigned pickups
using the robot's current heading and the most recently published traffic costs.
It requires learned flow and remains off by default. The full evaluation uses the
confirmed margin50/build128 reference and compares enabled/disabled pickup flow
under ordinary HRRN/weight1 and direct cost/weight5. The historical weight5 collapse
used build32, so this new comparison does not reproduce that exact regime.

## Published metric and units

Scheduling runs before the planner observes and publishes the current step's
traffic update. Pickup selection reads the last complete publication; it neither
publishes early nor observes twice. Before the first publication it follows the
original scheduler. A publication boundary can therefore produce a one-call lag
between the scheduling and planning metrics, explicitly part of this policy.

Forward Dijkstra searches `(cell, heading)` states with positive turn and forward
costs, and emits a pickup only when the minimum over its terminal headings is
settled. `CGAR_PICKUP_FLOW_NODES` bounds heap pops (including stale entries), defaults
to8192, and accepts1–65536. The existing shortlist limits and assignment/fairness
logic remain. This is a different work primitive and shortlist ordering from the
old2048-cell BFS; equal numeric bounds would not establish equal work.

Pickup cost is in published metric units. Chain estimates and the age numerator
are multiplied by the base flow scale, including the minimum cost floor. The
reported pickup estimate is normalized back with ceiling division. Fallback and
fair/global estimates first preserve the old spatial-cache lookup, then use an
existing complete turn table without building a table or changing its LRU order.
If unavailable/unreachable, they use the former approximate estimate multiplied
by the same scale. Those fallback costs are not claimed to be exact weighted
shortest paths. Chain estimates remain spatial approximations.

The local search uses the physical free-cell graph, restricted to the certified
core in capacity mode. A capacity-mode start outside the core retains BFS.
Cached turn-table domains can differ around certificate pockets; this experiment
does not claim domain equivalence there. It uses no map identity, aisle template,
future task, supplied direction weights or clock-selected partial result.

## Validation and provenance

Frozen build `runs/cgar-pickup-flow-build-v41-r3-20260919` completed GRID8898778.
All existing regressions pass, plus32256 independent goal-cost comparisons against
repeated graph relaxation with directed costs, obstacles/disconnection, all headings
and scales1/2/4/8. Tests cover settled-only endpoints, fixed work, early endpoint
stop, a timeout injected inside the search and scratch reuse after that exception.
Scheduler cases cover heading and published-toll ranking, scaled units, cold
control equivalence, oldest-task admission, started-task protection, immutable
simulator metadata and invalid configuration.

The heading fixture places tasks relative to the actual independently replayed
warmup heading. Initial fixture attempts assumed an idle robot would preserve its
heading; the planner rotated it. The toll fixture ingests a prescribed physically
valid FW/CR/CR observation trace to isolate published-field ranking. It does not
claim those prescribed observations are the planner's offered actions. The full
simulator comparison separately checks actual offered/executed decisions.

[Build archive](build-provenance/v41/) reconstructs all21 requested source/test
SHA-256 hashes from its recorded base plus patch. Production source did not change
during the failed fixture-only attempts or the successful build. An exception
propagates to whole-entry failure; retrying the same planner after a failed entry
does not promise rollback of all private state.

GRID8898784 passed all four200-step screens: complete decisions, zero errors or
timeouts, maximum0.875702s and peak5.204GB RSS. The disabled HRRN1/weight1 control
exactly reproduces the earlier build128 trajectory fingerprint. Short-prefix task
counts are not used for performance ranking.
[Screen evidence](results/pickup-flow-screen-v41/).

Full5000-step seeds0/2 are running as8898787, with independent analysis8898788.
Eight simultaneous cases each have four disjoint physical cores, a32decimalGB
process limit, and128GiB aggregate reservation. Profiles are in
[pickup-flow-variants.json](pickup-flow-variants.json).


The tested source and all21 source/test hashes match
[eb5b382](https://github.com/fywu85/lorr/commit/eb5b3827b9daf9f91aba88c78bef7be0d17cd07c).
[Fable turn09](fable-flow-session/turn09/review.md) found no reproducible bug;
[our assessment](fable-flow-session/assessment.md) qualifies its measurement claims.
While this full experiment was running, margin25/build128 completed confirmation
as the stronger reference. This matrix retains its frozen margin50 baseline.


## Full results

All eight full cases independently validate. HRRN1/weight1 pickup flow improves
137060/136841 to137508/137923 (+0.559% in the two-seed mean), including both
final1000windows. Whole-run empty robot-steps fall about2.0%/1.5%, while task-age
p90 rises from647/646 to653/661. That is a paired policy result, not isolated
causal savings from a particular cost term. Mean entry latency rises about17ms.

HRRN0/weight5 pickup flow scores138242/49732 against stable disabled controls
137603/136560. Seed2 final1000 falls to4522 despite all valid complete decisions;
reject that profile. The failure is not explained by a planner timeout.

At the final t4800scheduler snapshot, normal weighted searches settle1274/1270
cells per search and about34% hit the8192pop bound. Cached fallback probes are
96.44%/96.64%, contrary to Fable's rare-cache assumption; these are probe shares,
not assignment-selection shares. Estimated pickup counters include different
cost terms across modes and are not a physical travel comparison.
[Full comparison](results/pickup-flow-full-v41/comparison.json).

The stronger graded reference now has six-seed confirmation. Normal pickup flow
is being compared with it on seeds0/2 as8898831/8898832. Its disabled200-step
control exactly reproduces v39graded costs. In parallel, v42tests complete pickup
fields and all-visible-task ranking; see [PICKUP_FULL.md](PICKUP_FULL.md).

## Graded reference combination

All four full graded strength4/scale4 margin25/build128 tests completed. Pickup
flow ON scores138963/139677 against exact OFF controls138402/139303: mean
139320vs138852.5 (+0.337%). Final1000windows improve244/6. Empty robot-steps
fall1.63%/1.46%, while outstanding agep90 rises18/20steps. All entries finish
within1second, all RSS below32GB; source is the same tested v41commit. This is
a promising two-seed result; retain the confirmed OFF reference pending the
remaining four seeds. [Comparison](results/graded-pickup-flow-full-v41/comparison.json).

## Six-seed graded confirmation

All12paired full runs are valid, below1second and32GB; all six OFFtrajectories
exactly reproduce the graded reference. ONscores138963/140002/139677/138671/
139241/139773, mean139387.8333 (+0.316545%). Paired gains561/286/374/165/937/
316; final1,000-step gains244/119/6/108/378/186. Empty robot-steps fall on all
six (1.46–2.15%), agep90 rises7–20steps. This modest improvement is promoted
as the new benchmark reference; no152981target claim. Exact sourceeb5b382.
Mean241.5–284.0ms,median210.1–262.9ms,p99529.8–570.6ms,max872.4ms,
CPU1.171–1.204cores,peak11.901GB,21.36–24.91minutes.
[Confirmation](results/graded-pickup-flow-six-seed-v41.json),
[full-step latency audit](results/graded-pickup-reference-latency-six-seed-v41.json).
