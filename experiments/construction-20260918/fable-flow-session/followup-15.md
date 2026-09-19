Continue the SAME persistent lorr-warehouse-cgar-fable51 session, turn16.
Fable5.1/max, read-only, no tools. Visible response <=1,000words. No claim of tests.
User permission to share sources persists. Source is unchanged since your turn15.

Goal unchanged: six full5000step/10000robot warehouse seeds >=152981tasks, complete
fixed-work decisions<=1s or explicit failure, processRSS<32decimalGB, isolated GRID
physical cores, generic policies. Current mean143941.2 and peak144510 unchanged.
Full temperature0 LOST both0/2:143600/144083 vs144510/144107 (−0.324%mean).
Full eightregions LOST both:144096/143553 (−0.335%). Allvalid/exactcontrols. Retain
fourregions/two rounds/25k attempts/1000ppm. Full audit reference lost9/10peaks among
40000batches each,600/662 discarded scalar units. Retention is not prioritized.
Your3M/fourround and2M/fourround suggestions passed200steps, maxima969.9/993.6ms,
control962.2ms. Full6cases(0/2 xcontrol/3M/2M) nowrunning.2M seed0already timedout.
Scale513(full physical heuristic outranks op term) passedscreen but seed0timedout
at timestep2230,1003.435ms,stage temporal_region_repair. At2200 its regional stage
was395ms,~51.6M candidates across200kattempts, compared140ms/200steps. Remaining
runscontinue; no partial throughput is accepted, no clock-selected partial solution.

Corrections to turn15: affected robot first-action invariance is false. V49owncell
fixture explicitly nativeCR -> known-nextW. Neutral may also change firstaction.
Resident goal-distance tables minimize terminal heading, so four peeks do not give
exact full-chain arrival/departure coupling. Your sub-ms claim was unmeasured.
Repeatedpartition geometry is not an inert round: plans/boundarymembership/RNGchange.
No neutral-tail implementation yet. Its formula identity is consistent with source.

We now need concrete algorithm-preserving speed improvements, not another policy
sweep. Please inspect the cached temporal_pibt.hpp / temporal_regions.hpp hot path.
Rank only 2-3 plausible exact optimizations, identify source-level invariants and
how to measure them without confounding changed search. Do NOT assume bottleneck
from source alone. Existing optimized regional candidate_order already filters
bydepth/region once per constructor; cannot re-propose it as new. Current blocker
checks all5cell then all5edge slots in this exact order and returns singleowner/-2.
Full five-slot reservation legality and unchanged floating score/RNG/search work
must remain. Every fixed attempt finishes or timeout propagates.

Local candidates to critique:
1. Owner/choice-region geometry is recomputed every round even if a phase repeats.
Cache classification per actualshift within one decision; still rebuild roots from
CURRENT selectedpaths eachround. Helps4rounds, little2rounds; no map-conditioned rule.
2. Constructor builds indexed candidates separately for each region, scans allrobot
metadata, reserves count*3, zeros dense reservation arrays and reinserts ALLrobots.
Could safely share immutable per-robot/per-horizon admissible indexlists within a
phase, or reuse scratch capacity, while preserving each worker's complete private
reservations? Which part likely matters and what extra instrumentation needed?
3. Deadline clock calls happen each repair attempt plus every16 recursion counter;
quantify with profiling before touching. Any granularity change must still enforce
finalcomplete-entry1s and explicitfailure; no evidence yet that this dominates.

Please suggest ONE bounded measurement (e.g standalone fixed temporal workload or
GRIDperf on warehouse) that identifies actual expensive functions, and an exact
optimization with plausible enough payoff to recover100ms on difficult regional
steps. If none appears grounded, say so. No speculative 2x claims or unlimited
new solver. Need retain production behavior and binary/source provenance in tests.
