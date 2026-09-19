# Continuing warehouse work

Goal: at least 152,981 completed tasks over all 5,000 steps, repeatably over six
seeds, complete one-second decisions, and measured process RSS below
32,000,000,000 bytes. No map-name branches, aisle templates, supplied weights or
map-specific fleet caps. The local leader reference exceeds the memory target;
retain that resource difference when comparing.

The established equal-weight 50k profile averages 107,413.3 over seeds 0–2.
The strongest exploratory seed-0 result is 111,118: 4M fixed candidate work plus
direct-cost dispatch, pickup weight 5, and a global shortlist of 64. Its paired
control is 107,457. All complete 5,000 steps and pass timing/memory/error checks,
but direct-cost dispatch leaves older outstanding tasks. This is not a six-seed
promotion. See [RESULTS.md](RESULTS.md).

Active queue:

- Full reviewed matrix **8898387**, frozen build v14: six profiles, two concurrent
  instances, four disjoint physical cores each, 24 GiB total reserved memory.
  Compare compact control, turn costs 2/4, one/two regional rounds, and 25k global
  plus two regional rounds. Cost 8 failed its screen and is excluded.
- One-core analysis **8898389** after completion.
- The approved Fable review is complete. Both old fallback corner defects are
  reproduced and fixed; regional diagnostics and non-vacuous tests pass. See
  fable-regions/assessment.md. Prefetch/compact storage were outside review scope.

Build v14 passes the complete regressions and six 200-step deadline screens.
All three unit-cost profiles preserve exact v13 50-step trajectories. Compact
storage has verified lossless overflow fallback, but actual full RSS and complete
control trajectory equality still need checking. Prefetch had no measured
benefit and stays off. Earlier held full jobs were replaced with specs and
cancellation records preserved; no running experiment was canceled.

Prioritize loaded-motion efficiency: the earlier audit found 1.4888 steps per
shortest-path cell for CGAR versus 1.0799 for KittyKnight. The 4M profile reduces
loaded moves away from the spatial goal from 3.335M to 3.183M, a modest gain.
Evaluate the weighted-turn and regional full results before combining features.
Generic learned directional traffic costs, using only observed executed movement
and a fixed warm-up before freezing costs, remain an unimplemented follow-up.

After each full matrix: check complete entry samples, all errors/timeouts, RSS,
trajectory fingerprints, final-1,000-step rates, and seed effects. Confirm the
strongest candidate across all six seeds before claiming repeatability.
