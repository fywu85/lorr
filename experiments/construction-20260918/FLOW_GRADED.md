# Graded observed-traffic penalties

The measured margin50 to25 gain motivates testing how penalties treat weaker
versus stronger directional evidence. This is an existing generic cost rule,
with no map identity, topology template or future-task information.

The usual strength1/scale1 charges a full extra unit against any direction that
passes the count and margin thresholds. Strength4/scale4 charges one to four
extra quarter-units in physical-cost scale according to the observed imbalance.
A low margin admits weaker evidence while preserving the full penalty on strongly
polarized edges. Every direction remains usable; this does not establish a
statistical-confidence threshold or guarantee better routing.

Initial screen8898764 compares the confirmed intermediate margin25/build32
control against graded margin25 and0. All three pass below0.778s, with exact
control reproduction. While that screen ran, the build128 six-seed confirmation
established a stronger reference. Full testing therefore uses build128 with a
separate screen, rather than inferring its timing from the old lower work count.

Screen8898767 passes four profiles at build128: binary margin50/25 and graded
margin25/0. Both binary controls exactly reproduce their prior fingerprints.
Maximum complete entries are0.869/0.871/0.873/0.872s, RSS below5.226GB. The
short task totals are not a policy ranking.

Full8898768 and analysis8898769 evaluate all four on seeds0/2 with frozenv39,
exact source5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27. All eight cases run
concurrently on disjoint four-core allocations,128GiB aggregate reservation and
32decimalGB per-process limit. Complete-entry deadline remains one second.
Both binary controls must repeat; promising graded policies need the other four
seeds before adoption.

[Initial screen](results/flow-graded-screen-v39/),
[build128 screen](results/flow-graded-turn128-screen-v39/),
[full configuration](flow-graded-turn128-variants.json).

## Full seeds0/2 completed

All eight runs independently validate; all four binary-control trajectories are
exact. Graded margin25 completes138402/139303 (mean138852.5), versus binary
margin25's137149/139037 (mean138093): +0.550% in the mean. Seed2's final1000
window is153tasks lower, so full totals alone do not establish uniformly better
steady throughput. Graded margin0 scores56537/139796 and collapses on seed0
(final1000=5770), despite valid one-second decisions. Reject margin0.

[Full comparison](results/flow-graded-turn128-full-v39/comparison.json).
Remaining four seeds run as8898785 with analysis8898786, comparing graded and
binary margin25 on the same frozenv39 source. The confirmed reference remains
margin50/build128 until six-seed evidence supports a change.
