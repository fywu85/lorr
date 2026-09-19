# Independent checks of the proposed continuation objective

This is an offline score experiment. The production CGAR planner still scores only
the current errand; no continuation or neutral-tail policy has been implemented.
Fable turn13 supplied the hypothesis, and the local probe checks its arithmetic and
small joint-choice examples. The rejected arrival-heading baseline was a proposal,
not an existing production bug.

For a robot with current goal G and visible next errand N, the common baseline is
B=min_heading D_N(G,heading). A candidate reaching G uses
`d = -4*U + D_N_terminal(endpoint) - B`, then pays extra real turn and forward costs
throughout the five-action path. Its cost is `50*(d + paid_extras) - op*U` here.
Terminal waits allow the same paid virtual rotations as the native objective.
Candidates that do not reach G retain the current score. Repeated G=N retains the
native score. These branch choices and keeping N as terminal even after visiting it
are approximations; this is not an exact multi-errand completion-time objective.

The independent oracle computes oriented distances by Bellman relaxation of an
explicit directed graph, then replays each complete weighted action sequence. It
enumerates wait/right/left replacements of up to two trailing waits rather than
copying the scorer's closed-form minimum. There are54,378 arriving-candidate checks
across units1/4/8, turn extras0/1/2 and zero/asymmetric forward tolls, all passing.
Another220,050 non-arriving cases take the unchanged native branch. These small-grid
checks are unrelated to full warehouse timing or quality.

The hand-derived heading pair now reproduces independently: on a3x7 grid, start
(2,0) north, G=(1,1), N=(1,6), compareFRFFF andRFCFW withU=T=4. Common-baseline
physical scores are−24/−16, a gap of8. The candidate-dependent arrival-heading
baseline gives−24/−20, a gap of4, refunding the worse arrival heading. These values
exclude the operation-index term. No production score used that rejected baseline.

For each earlier two-robot example, exhaustive minimization over all compatible
native129-operation pairs gives:

| Fixture | Objective | A operation | B operation | First actions | Minimum under that objective |
|---|---|---:|---:|---|---:|
|0|Current errand|58|102|forward / right turn|−1040|
|0|Known-next common baseline|80|111|forward / forward|−1564|
|0|Neutral tail|127|111|forward / forward|−1552|
|1|Current errand|58|102|forward / right turn|−840|
|1|Known-next common baseline|80|111|forward / forward|−1364|
|1|Neutral tail|128|111|forward / forward|−1356|

Each row minimizes its own objective; the absolute scores are not a cross-objective
quality ranking. The common-baseline plan reaches A's known next errand in these
examples. Neutral tail instead setsd=−4U after any first current-goal hit and charges
extras only to that first hit; it can prefer an arbitrary departure because of the
operation tie term. It is a separate hypothesis, checked here only on the two joint
fixtures, not by the weighted common-baseline oracle.

A production prototype still needs one metric decision for the whole robot candidate
set, cache/publication-safe non-LRU reads, missing/unreachable-table fallback, pinned
robot protection, serial/parallel preparation checks and exact disabled trajectories.
The post-service audit's eligible candidate set would change with a different score,
so its bucket movement would not by itself demonstrate a reservation improvement.
Full throughput, tail, age, deadline and memory checks remain required.

[Probe source](continuation_probe.cpp), [commands, output and source hashes](results/continuation-probe-v1.json),
[original goal-holding examples](GOAL_HOLDING.md), [Fable review and assessment](fable-flow-session/turn13/).
