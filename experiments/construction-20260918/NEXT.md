# Continuing warehouse work

Goal: at least152981 tasks over5000 steps with10000 robots, repeatably over six
seeds, complete one-second decisions, process RSS below32000000000 bytes and
isolated GRID physical cores. No map identities, aisle templates, supplied weights,
future tasks or map-specific fleet caps. Local KittyKnight used38.858GB RSS; this
is not an official equal-resource comparison. Goal remains active and unmet.
Maintain ../../WAREHOUSE_PROGRESS.md with exact source/commit/time provenance.

## Current confirmed reference

Margin50/build128 in warehouse-reference-variants.json:
137060/137271/136841/137227/136949/137092, mean137073.3 (+1.845% versus build32).
All six totals/final windows improve; all12paired runs valid and controls exact.
Source5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27, frozenv39.
Mean234.8–269.9ms per complete scheduling+planning step, max0.873055s,
CPU1.182–1.210cores out of four reserved, RSS12.013GB and20.80–23.75minutes.
No p99 aggregation for this cohort. See results/turn-build-limit-six-seed-v39.json.

## Completed comparisons

- Batch128 six seeds:138003/137388/136701/137477/134785/137390, mean136957.3,
  minus0.085% against demand-build128. Four wins/two losses; seed4 erases gain.
  All decisions valid, four v40 stronger-control repeats exact. NOT promoted.
  Source50e8e6e4cef1495a4054590e613cf762eb39009c.
  results/temporal-table-batch-six-seed-v40.json. Prior serial/parallel batch128
  trajectories match exactly and four threads cut wall time5.86–6.37%; that does
  not establish throughput gain over the stronger demand-build128 policy.
- Margin25/build128:137149/139037 seeds0/2, mean138093 (+0.834% vs50/128).
  Both totals/final windows improve. All six known constituents/controls exact.
  Factor effects overlap (difference-in-differences minus766.5tasks).
  results/flow-turn-interaction-full-v39/comparison.json. Four-seed confirmation live.
- Graded strength4/scale4 margin25:138402/139303, mean138852.5 (+0.550% vs binary
  margin25). Both full totals improve; seed2 final1000 is153tasks lower.
  All eight complete runs valid and four binary controls exact. Four-seed check live.
  Graded margin0:56537/139796. Seed0 final1000 collapses to5770; REJECTED despite
  historical peak139796. results/flow-graded-turn128-full-v39/comparison.json.
- Machine progress provenance has33 chronological records. Latest139303 completed
 16:13:44UTC, then rejected-policy139796 at16:13:53UTC, both exactv39 source.

Other rejected directions remain documented: transactions six-seed mean134202.5
(-0.288% vs old134590.3), unopened pool mean134840.8 (+0.186%, worse ages every
seed), direct cost/pickup5 seed2 collapse52444, batch512 seed2 regression119685
with100% sampled exact guidance. Coverage is not a monotone quality measure.

## Running full evaluations

- 8898776 /8898777: binary margin25/50 at build128, seeds1/3/4/5, frozenv39;
  runs/cgar-flow-turn-interaction-confirm-v39-20260919, start16:11:23UTC,
  exclusive research50. Verify four margin50 trajectories against six-seed reference.
- 8898785 /8898786: graded strength4/scale4 margin25 versus binary margin25,
  seeds1/3/4/5, frozenv39; runs/cgar-flow-graded-turn128-confirm-v39-20260919,
  start16:23:13UTC, exclusive research31. Binary controls should match8898776.
- 8898787 /8898788: pickup flow enabled/disabled under HRRN1/weight1 and
  HRRN0/weight5, all margin50/build128, seeds0/2, frozenv41;
  runs/cgar-pickup-flow-full-v41-20260919, start16:24:43UTC, research38.
  HRRN1/weight1 disabled control must exactly reproduce established build128.
  Historical weight5 collapse was build32; do not claim this recreates that regime.

Each matrix has eight simultaneous cases, four disjoint physical cores per
planner,128GiB aggregate reservation and32decimalGB individual limit. All C++
builds complete. Live runs use frozen binaries. No C++ build is active.

## Pickup implementation checkpoint

V41 optional CGAR_PICKUP_FLOW defaults off, requires learned flow. ForwardDijkstra
uses current heading and last complete publication, emitting only settled cell
minima. Fixed8192queue-pops default (1–65536), same endpoint shortlist limits.
Scheduler precedes publication; it intentionally sees the last completed field.
Pickup, chain and HRRN age terms use compatible base units. Fallback uses a cached
complete turn table when present, otherwise scaled former approximate costs;
no fallback exactness claim. Physical free graph/core restriction differs from
certificate-table pocket domains. See PICKUP_FLOW.md for implementation limits.

Build8898778 runs/cgar-pickup-flow-build-v41-r3-20260919 passed ALL regressions,
including32256 independent shortest-path checks and scheduler integration cases.
Archive build-provenance/v41 independently reconstructs all21 source/test hashes.
Earlier attempts changed test fixtures only: warmup actually rotates idle robots;
heading fixture now uses observed heading. Toll fixture isolates a prescribed
physically valid observation trace; full simulator validates actual offered actions.

Screen8898784 has4/4valid, max0.875702s, peak5.204GB, exact disabled default
fingerprint a5e00bb26608e8aa797413db9267fdce11f1630a2ea8ffafbfc67ff6f739c71a.
No short-prefix performance rankings. results/pickup-flow-screen-v41/.

## Persistent Fable

Same Claude CLI session1ebb1075-3538-49d1-93d1-a00c94fa256a, Fable5.1 max,
via fable-flow-session/run_review.py. User explicitly permits source/info sharing.
Turns01–08 complete. Turn08 finished15:34:44UTC,612.547s, actual16.015591USD
(the requested12USD flag is not a reliable hard cap). No reproducible v40 bug found;
Fable ran no tests. Visible review/metadata/status archived; raw protocol ignored.

Turn09 will review committed v41 in the same session using followup-08.md and
updated source-spec ranges. Check runs/fable-cgar-session/turn09/status.json before
starting anything; never restart session or resend the entire codebase.

Fable08's pickup alignment suggestion motivated v41; its predicted gain and any
claim that search directions are exhausted remain unproven. More table coverage
can lower throughput. No source change is justified solely by a review prediction.

## Next decisions

1. Confirm margin25/build128 over six seeds; verify every repeated control and
   full entry timing/RSS before changing the explicit benchmark reference.
2. Complete graded and pickup full comparisons, including late windows and task
   age. Reject fragile settings; extend promising ones to the remaining seeds.
3. Review Fable09 findings against source/tests; implement reproducible corrections
   in a new frozen build without editing during a GRID build.
4. Commit/push validated checkpoints and update chronological throughput records
   with exact-source hashes. Keep the goal active until all target checks pass.
