# Explicit RANDOM guidance-strength comparison

Declared before benchmarks. KK's imported RANDOM03 costs range2..62 and RANDOM04
6..64, against a nominal forward20; NMS uses20/60. Cheap and expensive routes can
favor detours. This test retains the existing field geometry but blends each
forward cost toward uniform20. It may trade fewer detours for more congestion;
no throughput gain is assumed.

Strength25/50/75/100 uses round_half_up(20 + strength/100*(original-20)). The original
100 is byte-identical. Turns and service durations remain unchanged. All routing,
pickup and chain metrics see the same installed vector. Only explicit --trick
RANDOM instance, native static lanes and NMS/KK references1/2 are accepted; uniform
mode and other providers reject this flag. No new map layout or task-stream fit.

The generator verifies all four original field SHA/FNV identities, all49,152
nondefault transformed values, positive bounds and unchanged uniform entries.
Regression compares against an independent rounded formula and oriented heap,
checks compact/wide oracles, production pickup/planning, omitted/100 action and
assignment identity, corrupt assets and strict configuration gates.

After complete source qualification: full RANDOM02seed2, RANDOM03seed2 and
RANDOM04seed4 matrices compare their selected controls with25/50/75. RANDOM04
retains its newly selected crowd16 and motion commitments; RANDOM02/03 retain
current windows. Four bound physical cores per case, strict1000ms,32decimalGB,
full600/800/1000steps. Replicate positive signals, keep all failures.

Pre-submission amendment 2026-09-21T13:21:23.684948+00:00: an independent crowd-cost replication promoted RANDOM04 to2321seed4 before any reference-strength jobs were submitted. The declared RANDOM04 control now uses seed4 and the same crowd16 profile. RANDOM02seed2 and RANDOM03seed2 remain unchanged. Source6fc0cc50 passed all41 source/test hashes and full GRID regression before submission.

Completed: RANDOM02 control1215 versus1172/1189/1212 at25/50/75; RANDOM03 1982 versus1430/1668/1717; RANDOM04 2321 versus1680/2111/2063. Every arm is full and valid; all three controls reproduce exact whole trajectories. Retain100 and stop weakening this field.
