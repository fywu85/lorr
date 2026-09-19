# Continuing warehouse work

Goal: at least 152,981 completed tasks over all 5,000 steps, repeatably over six
seeds, complete one-second decisions, and measured process RSS below
32,000,000,000 bytes. No map-name branches, aisle templates, supplied weights or
map-specific fleet caps. The local leader reference exceeds the memory target;
retain that resource difference when comparing.

Established results:

- Equal-weight 50k averages 107,413.3 across seeds 0–2. Compact seed 0 preserves
  every full trajectory field at 107,457 and reduces RSS from 16.208 to 11.883 GB.
- Strongest completed seed-0 result: 112,164 with two regional rounds, maximum
  entry 0.891 seconds. One round reaches 111,411 at 0.680 seconds. Reducing global
  work to 25k with two rounds reaches 111,997 at 0.725 seconds. All use four cores
  and about 11.82 GB RSS. No six-seed confirmation.
- Regional gains persist in the final 1,000 steps and improve outstanding-task
  age. Earlier mixed scheduler/4M work reaches 111,118 but worsens the age tail.
- Corrected turn cost 2 yields 100,323; cost 4 fails at step 902, cost 8 fails its
  screen. No failed case gets a partial score. Prefetch remains off after finding
  no benefit. The approved Fable v11 review is complete and its proven issues fixed.

Active full queue:

1. **8898428**, frozen v16, runs control, learned-flow strengths 1/2/4, and distance
   scales 256/1024 separately. Two concurrent instances, one distinct physical core
   each, 24 GiB total. Actual EPYC 9354 bindings [0] and [1], no CPU quota. Analysis
   **8898429** follows. The control has reproduced 107,457; flow strengths 1 and 2
   fail late explicit repair deadlines and receive no accepted score. Flow 4 and
   distance-scale cases continue.
2. **8898475**, frozen v19, follows guidance. It tests control and warm starts
   with 50k/25k/10k fixed attempts. Two single-core instances, 24 GiB total.
   Analysis **8898476** follows. All 200-step screens pass, and disabled warm
   starts preserve all prior trajectory fields. See WARM_START.md.
3. **8898445**, frozen v17, follows the warm-start matrix and analysis. It tests one-round
   control, regional temperatures 100/0, and two rounds at 0. Two concurrent
   instances, four cores each, 24 GiB total. Analysis **8898447** follows. All four
   200-step screens pass; default trajectories exactly match v14. See TEMPERATURE.md.

The v18 static predecessor cache passes regressions and cuts isolated table
build time by 19–26% with matching checksums. All three integration screens pass with
exact prior trajectories over 200 steps; no whole-planner speedup claim yet. See ORACLE.md. Warm starts are now implemented and validated for safety/reuse;
full performance is pending. A separate fixed candidate-work quota may address
the learned-flow repair explosion, but has not been tested with flow yet.

After each matrix: preserve failures, check all 5,000 entry samples, errors,
timeouts, actual RSS, fingerprints, final-window rates and movement efficiency.
Keep deadline screens separate from throughput evidence. Compare standalone
variants before combining them. Six-seed confirmation remains required before
claiming repeatability or meeting the leader target. The active goal is not complete.
