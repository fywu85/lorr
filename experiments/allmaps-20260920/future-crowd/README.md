# Terminal crowd cost in complete CGAR futures

Declared before benchmarking. Transfer one general heuristic from the immutable
PILOT source review (engine.cpp, evaluate/rollout dispersion): penalize unordered
robot pairs within Chebyshev distance2 at each completed forecast endpoint. This
can distinguish traffic layouts with similar known-task progress. It is a forecast
heuristic, not a guarantee that lower local density improves eventual throughput.

CGAR_FUTURE_CROWD_COST defaults0; positive integer0..255 requires enabled common
futures. Every root sees the same declared continuation orders and RNG. Base cost
plus crowd_cost*pairs is summed across all branches; the minimum complete root is
chosen only after all work finishes. A deadline in any branch fails the entry.
All robots, including protected/goal-less ones, count. CGAR primary/recovery and
one-action commitments remain intact. No map lookup or geometry-specific field.

Counter work is O(cells+12*robots), independently checked against a quadratic
pair scan across490 varied layouts and row boundaries. A two-robot fixture has an
independently known objective: with two branches, stay costs16 and separate costs6
atpairprice8; price0 keeps the stationary incumbent. Tests check default/RNG
identity, exact accounting, serial/parallel choices, invalid values, deadline
propagation, production service and protected/motion-commitment composition.
Common-future roots must also agree on the new first-cell constraint vector.

First proposed full comparison: RANDOM04 selectedseed2 mode2H20, current2298,
against pairprices8/16/32. Nativeforward20 makes these0.4/0.8/1.6forward-step units
per pair. Keep all scheduling, fields, declarations and explicit --trick RANDOM-04
settings fixed. Full1000steps, strict1s32decimalGB,4bound physicalcores. Replicate
any positive selected-seed result. No throughput claim before verification.

First full RANDOM04 results: control2298; pairprice8=2268,16=2304,32=2292.
All valid and the control repeats its exact full trajectory. The six-task gain
at16 is a selected-seed record, not a robust improvement. A matched control/16
comparison on seeds0/4 is declared next. Max entry304.05ms for2304.
