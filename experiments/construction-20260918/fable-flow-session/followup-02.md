Continue the SAME read-only CGAR consultation. New validated full results and a
bounded diagnosis request; no code execution or map-specific policy suggestions.

All runs are full 5000-step/10000-robot WAREHOUSE, fixed candidate work, 1-second
complete decisions, four reserved EPYC9354 cores per instance, <12GB RSS.
Frozen controls exactly reproduce the previous full trajectory hashes.

Complete refresh matrix (independent trajectory analysis passes all six):
seed0: frozen122896, refresh512=134511, refresh1024=133652.
seed2: frozen70171, refresh512=134519, refresh1024=131316.
Refresh512 final1000 counts28215/28214, agep90=653/652,
max entry.765267/.762019. Six-seed confirmation versus same-build no-flow is queued.
Frozen+warm-start reuse, independent analysis complete:
seed0=123373, seed2=123251; final1000=25344/25281. Both recover steady rates.
Thus the initial field is not inevitably fatal under every local construction.

Your wait-seed tie issue is independently reproduced: blocked diagonal-goal robot,
4rotations x4costscales,128alternating tied rotations. Strict improvement removes
all128 while retaining16useful turns. Production regressions verify9600 actions,
protection, parallel preparation, warm reuse, refreshed flow, independent rotation
counter conservation. Default source remains bitwise-action-equivalent on screens.
Full seed2 summaries (not yet full independent trajectory analysis):
frozen strict0=70171 ->strict1=121534; refresh512 strict0=134519 ->strict1=133672.
All four pass full timing/RSS/simulator validity. Seed0 still running.
Avoid asserting the tied turns uniquely explain collapse; local trajectory changes
can also avoid an unstable state. Strict+refresh is not an established additive gain.

Next bounded experiment proposed: CGAR_FLOW_CACHE_ONLY_REFRESH=1 with the existing
fixed observation refresh interval. First128observations publish exactly the ordinary
field. Later publications keep that first cost vector unchanged but clear ordinary
and speculative orientation caches at128+k*512. No future-run information, wall-time
adaptation, partial output, map-specific rules or RNG changes. Compare frozen,
cache-only512, genuine-refresh512 on seeds2/0; preserve exact control hashes.
FlowGuidance keeps the original metric; the oracle exposes clear_tables preserving
metric/base units. Tests cover hand-counted reversed later observations with constant
costs, duplicate/gap timestamps, invalid configuration, prefetch invalidation, exact
rebuilt distances and protected serial/parallel production episodes.

Please give a concise second opinion (<=900words):
1. Any important causal-control or implementation flaw in this design?
2. Given refreshed rates~28k/1000 versus our152981full local target, select at most
   TWO next generic changes with a concrete mechanism and falsifying experiment.
3. Reassess whether branching displacement is now justified or what counter must
   be measured first. Keep established facts separate from inference.
Changed source excerpts follow automatically. Unchanged files are omitted.
