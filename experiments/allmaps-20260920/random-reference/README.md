# Explicit NMS and KK guidance on RANDOM instances

The generator executes the unchanged archived NMS `GuidanceMap` reader and
`GraphGuidance` constructor on the same first arrow layout. It independently
checks every forward value against the arrows and every turn/wait value, then
scales all NMS costs by10: forward20/60, turn20. The complete occupancy must
match the archived819-free-cell map. No independent RANDOM05 source was edited.

KK's active files are random_weight_101.w at200 robots, random_weight_040.w at400,
and random_700_w002.w at700. Their E/S/W/N forward costs scale by20 with numerical
error<0.00001. At200, uniform stay1 becomes turn20 exactly. At400/700, varying
stay/turn costs are **not transferred**: these are forward-only adaptations with
uniform turn20. KK's RANDOM01 configuration has no field; RANDOM05 uses another
planner, so no KK asset is assigned to those two cases.

[Immutable generation evidence](generation-v1/generation.json),
[source/input hashes](generation-v1/request.json). Four4096-byte forward fields,
wall entries normalized20. This proves field construction, not throughput.

`CGAR_TRICK_RANDOM_REFERENCE=1` selects NMS,2 selects the appropriate KK forward
field;0 retains the old standalone-derived dense field. Static sparse guidance
requires a positive selector. Every use requires explicit `--trick RANDOM-XX`,
exact occupancy and100/200/400/700/800 robots. Native metric, remaining-flow and
normalization20 are retained; reference default turn20. Ordinary no-trick runs
remain untouched. Generic controls use explicit lanesOFF.

The regression checks malformed providers, incompatible names/fleets/fields,
independent oriented heap distances including below-base edges, and real
scheduling/planning with every supported provider. Full tests/build must pass
before throughput runs. First compare full sparse generic controls against NMS
and KK references, plus a native uniform-cost control to separate scalar/scoring
conventions from guidance effects. Do not use partial task counts as full scores.
