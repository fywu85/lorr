# Dense-map scheduling transfer, predeclared factorial screen

The reference RANDOM-05 scheduler emphasizes pickup travel: approach +0.25chain
cost, joint idle/unopened assignment and a small keep bonus. CGAR's existing
machinery supplies a bounded first comparison before implementing a different
full-pool assignment algorithm.

Use the eight-worker, noise50, nonpersistent chain-rank control and compare:

* Original HRRN, pickup weight1, no unopened matching.
* Direct cost (HRRN0) with pickup weight4, equivalent relative chain weight0.25
  in the existing CGAR score. This is a declared bundle of two settings.
* Original HRRN/weight1 with existing 64-group unopened matching.
* Both direct pickup4 and matching.

Run all four generic profiles on RANDOM-04 and RANDOM-05. Separately run the
four field profiles on both maps, explicitly flagged with the named trick.
The field itself is unchanged. All cases: source8352226, seed0, full1000/2000
steps, enforced1000ms, four disjoint physical cores per process, shared EPYC9354,
32decimalGB. Full controls must reproduce earlier whole trajectory hashes.

This is **not** the reference's exact full-pool Hungarian scheduler: CGAR still
uses local unopened permutations, a20step cooldown and at most one retarget.
Started tasks and primary/recovery protections remain; forced-oldest admission
also remains. The weight test uses CGAR's current chain estimates, not the
reference's exact oriented whole-chain dynamic program. Fairness is secondary
and all waiting tails are retained. No known-horizon or short-task flag is added.
Any timeout, simulator error or memory excess is a failed case, not a score.


## First outcomes and replication

All16full strict1s cases passed. Generic R04 control/direct4/match/both:
1306/1343/1309/1503; generic R05:2036/2038/2062/2068. Field R04:
1353/1481/1356/1338; field R05:2457/2421/2544/2574. All complete control
trajectories match earlier runs. The combinations interact with the guidance;
there is no universal winning preset.

Replicate the generic R04 control and combined setting at planner seeds2/4.
For R05 replicate field control, matching-only and the combined setting at2/4:
matching-only was close enough to retain. Profiles are unchanged, four physical
cores per case, same archived inputs, strict1000ms, full horizons,32decimalGB.
Combine with seed0 and retain all failures; seeds change planner randomness,
not the archived start/task streams. No further tuning on those seeds before
reporting this comparison.
