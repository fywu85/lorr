# Continuing warehouse work

Goal: at least 152,981 completed tasks over all 5,000 steps, repeatably over six
seeds, complete one-second decisions, and measured process RSS below
32,000,000,000 bytes. No map-name branches, aisle templates, supplied weights or
map-specific fleet caps. The local leader reference exceeds the memory target;
retain that resource difference when comparing.

Established results:

- Equal-weight 50k averages 107,413.3 across seeds 0–2. The compact seed-0
  implementation preserves every full trajectory field at 107,457 and reduces
  RSS from 16.208 to 11.883 GB. Two full instances now run concurrently on
  disjoint reserved cores inside one 24 GiB total allocation.
- Strongest exploratory seed-0 result: 111,118, using 4M candidate work plus
  direct-cost dispatch, pickup weight 5, global shortlist 64. Paired control:
  107,457. Its outstanding-task age is worse; no six-seed promotion.
- Corrected turn cost 2 yields 100,323. Fewer turns are outweighed by more waits
  and detours. Cost 4 fails explicitly at timestep 902 in temporal repair;
  cost 8 fails its screen. Neither failed case receives a partial score.
- The approved Fable v11 review is complete; its fallback scoring defect and
  regional observability/testing gaps are fixed. Prefetch found no benefit and
  stays off. See fable-regions/assessment.md and RESULTS.md.

Active full queue:

1. Job **8898387**, frozen v14, still runs the regional profiles: one round,
   two rounds, and 25k global plus two rounds. The control/turn cases above are
   done. Analysis **8898389** follows completion of the entire matrix.
2. Job **8898410**, frozen v16, is held after 8898387/8898389. It tests one
   control, learned-flow strengths 1/2/4, and distance scales 256/1024 separately.
   Two concurrent instances, four disjoint physical cores each, 24 GiB total.
   Analysis **8898411** follows.

The v16 complete regression suite passes, including 7,200 independent shortest
paths, 610,224 physical-progress bounds, 33,282 actual-scorer dominance pairs,
and protected/threaded episodes. All relevant 200-step screens pass. Default
control and flow-1 trajectories exactly match v15; disabled flow matches v14.
Every source archive reconstructs all tested hashes. No full traffic/scaling
performance result exists yet. See FLOW.md and SCALE.md.

After each matrix: preserve every failure, check all 5,000 entry samples,
errors/timeouts, actual RSS, trajectory fingerprints, final-1,000-step rates and
movement efficiency. Keep full-run throughput claims separate from short deadline
screens. Compare variants before combining features. Six-seed confirmation of
the strongest candidate remains required before claiming repeatability or meeting
the leader target. The active goal is not complete.
