# A finite example of goal-holding incentives

The offline probe evaluates the production129-operation pool and TemporalGeometry
score on a3x7 open grid. Unit and turn cost are4, with exact independently computed
oriented distances and no traffic tolls. For each sampled input it exhaustively
minimizes the sum of the two current-errand scores over all collision-free candidate
pairs. This is score analysis, not a full CGAR search episode or a warehouse result.

One reproducible example uses zero-based(row,column) coordinates:

- Robot A starts at(1,4), facing west. Its current errand is(1,3), followed by the
  known errand(0,3).
- Robot B starts at(0,5), facing south. Its current errand is(1,1).

| Plan | A actions | B actions | Joint current-errand cost |
|---|---|---|---:|
|Exact minimum of the native score|F W W W W|R F F F F|−1040|
|Collision-free continuation witness|F R W F W|F R F F F|−912|

Lower cost is preferred. Both plans serve A's first errand in the first slot, and
A's first action is identical. The witness also serves its known second errand.
B's witness path improves its current-errand physical objective by one unit, but
conflicts with A's native held destination in a later slot. Letting A depart makes
the pair collision-free; the present score still prefers the hold/detour pair.

The fixed sample sequence examined987 valid two-robot inputs and found two such
examples. It establishes a realizable limitation of the current-errand objective.
It does not measure warehouse frequency, guarantee what bounded production search
selects, or quantify throughput recovered by another objective. The new read-only
[post-service audit](POST_SERVICE_AUDIT.md) measures related full-run opportunities.

Fable turn12 separately proposed a3x9 hand-derived example. Exact evaluation of that
example gives a first-forward choice for B (operation85), so its claimed native
first rotation did not reproduce under this minimization. The independently found
3x7 examples above are retained instead.

[Probe source](goal_holding_probe.cpp), [outputs, commands and hashes](results/goal-holding-probe-v1.json).
