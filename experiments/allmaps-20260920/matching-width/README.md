# Wider complete joint assignment groups

The current matching pass splits resident unopened assignments into groups of
32robots. Larger complete permutations can connect useful swaps across those
partitions. This follows the joint-assignment principle in NMS and PILOT while
retaining CGAR's own eligibility, primary/recovery, fairness, commitment, finite
task-retarget budget,20-step cooldown and resident-only oriented distances.

General `CGAR_REASSIGN_MATCH_GROUP_SIZE` defaults32, accepts2..256 and requires
matching when nondefault. Fixed group and2048node quotas remain. A whole group
is solved, its disjoint cycles are checked, and all chosen groups finish before
committing any task changes. No elapsed-time fallback or partial permutation.
The generic Hungarian primitive expands its maximum to256; integer arithmetic
near INT_MAX and full-width row-minimum cycle optima are checked at32/64/128/256.
Production fixtures check actual wide matrices, explicit32identity, task
metadata/bijections, primary protection, parsing and fixed-work bounds.

First full comparisons: RANDOM01seed4(693), RANDOM02seed2(1215),
RANDOM03seed2(1982), CITY01seed2(8440), CITY02seed0(16315). Match width/group
quota pairs32/64,64/32,128/16,256/8keep the maximum participant quota at2048.
Other selected settings stay fixed. Larger matrices still cost more arithmetic;
strict1000ms deadlines decide which complete runs qualify. Four physical cores,
32decimalGB, explicit --trick INSTANCE and node-local log spooling.

Full CITY results: CITY01 width32/64/128/256=8440/8440/8435/8434; CITY02=16315/16315/16260/16203. All eight complete and valid, max825.85ms. Width64 repeats the entire control trajectory on both. No promotion. Actual groups are usually smaller than32; reduced group quotas lose coverage. RANDOM01 all widths693, RANDOM02 wider1194/1154/1154vs1215, RANDOM03 all wider1938vs1982. Keep width32.
