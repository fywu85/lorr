# Continuing warehouse work

Goal: at least152981 tasks over5000 steps with10000 robots, repeatably over six
seeds, complete one-second decisions, process RSS below32000000000 bytes and
isolated GRID physical cores. No map identities, aisle templates, supplied weights,
future tasks or map-specific fleet caps. Local KittyKnight used38.858GB RSS; this
is not an official equal-resource comparison. Goal remains active and unmet.
Maintain ../../WAREHOUSE_PROGRESS.md with exact source/commit/time provenance.

## Current confirmed reference

Margin25/build128 in warehouse-reference-variants.json:
137149/138572/139037/137535/137875/138457, mean138104.17 (+0.752% vs margin50/
build128). All six full totals and five final1000windows improve; seed3's final
window is109tasks lower. All12paired runs valid and six controls exact.
Source5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27, frozenv39.
Mean228.0–265.1ms scheduling+planning, median197.3–242.4ms, p99 507.0–557.1ms,
max0.871954s at step0. CPU1.183–1.212cores/4reserved, peak11.933GB and20.23–
23.32minutes. results/flow-turn-interaction-six-seed-v39.json and
results/margin25-reference-latency-six-seed-v39.json (GRID8898798).
Preceding margin50/build128 mean137073.3; quantiles in
results/reference-latency-six-seed-v39.json (GRID8898790).

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
  results/flow-turn-interaction-six-seed-v39.json. Six-seed confirmation complete.
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

- 8898785 /8898786: graded strength4/scale4 margin25 versus binary margin25,
  seeds1/3/4/5, frozenv39; runs/cgar-flow-graded-turn128-confirm-v39-20260919,
  start16:23:13UTC, exclusive research31. Binary controls must match the new six-seed margin25 reference.
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
All21 match committed/pushed eb5b3827b9daf9f91aba88c78bef7be0d17cd07c.
Earlier attempts changed test fixtures only: warmup actually rotates idle robots;
heading fixture now uses observed heading. Toll fixture isolates a prescribed
physically valid observation trace; full simulator validates actual offered actions.

Screen8898784 has4/4valid, max0.875702s, peak5.204GB, exact disabled default
fingerprint a5e00bb26608e8aa797413db9267fdce11f1630a2ea8ffafbfc67ff6f739c71a.
No short-prefix performance rankings. results/pickup-flow-screen-v41/.

## Persistent Fable

Same Claude CLI session1ebb1075-3538-49d1-93d1-a00c94fa256a, Fable5.1 max,
via fable-flow-session/run_review.py. User explicitly permits source/info sharing.
Turns01–09 complete; no CLI process remains running. Turn09 finished16:34:51UTC,
262.410seconds, actual25.497911USD, sourceeb5b382,4changed excerpts/46617bytes.
The requested12USD flag is not a reliable hard cap. No bug reproduced and no tests
run by Fable. Visible review/metadata/status archived under fable-flow-session/turn09.

Fable highlights mixed cached/approximate fallback bases and search reach/work.
Its claims that cached probes are a minority or reach is half-sized are unmeasured;
running t2200 probes are ~96%cached, which is not an assignment selection share.
Its flow-is-the-only-lever claim omits temporal and table improvements. See our
qualified fable-flow-session/assessment.md. Await full v41 counters before acting.

GRID8898800 completed offline churn reconstruction on the four graded runs.
Raw runs/cgar-graded-flow-churn-v39-20260919; output results/graded-flow-churn-v39.json.
It checks production move/penalized counts at all publications and full movement
totals; no production field hash is available. Look for pre-decline sign/weight
churn before any sign-hysteresis change. This audit finds only3.87% early excess
pair churn and7.50% more sign flips in collapsed margin0 seed0; later churn is
lower. Simple large-churn precursor not supported; no hysteresis change. See
FLOW_CHURN.md and results/graded-flow-churn-comparison-v39.json.

Fable08's pickup alignment suggestion motivated v41; its predicted gain and any
claim that search directions are exhausted remain unproven. More table coverage
can lower throughput. No source change is justified solely by a review prediction.

## Next decisions

1. Current margin25/build128 reference is confirmed. Finish graded confirmation
   against it, with exact control hashes and every full entry/RSS check.
2. Complete graded and pickup full comparisons, including late windows and task
   age. Reject fragile settings; extend promising ones to the remaining seeds.
3. Review Fable09 findings against source/tests; implement reproducible corrections
   in a new frozen build without editing during a GRID build.
4. Commit/push validated checkpoints and update chronological throughput records
   with exact-source hashes. Keep the goal active until all target checks pass.


## Cache-capacity contrast

Screen8898807 passed both8/16GiB cache profiles. Both full200-step prefix hashes
exactly match the prior margin25/build128 reference. No short-prefix quality
ranking or full-horizon memory claim. Results: results/turn-cache-capacity-screen-v39.
Full seeds0/2 run8898808, analysis8898812, frozenv39. Raw
runs/cgar-turn-cache-capacity-full-v39-20260919, four concurrent cases with four
physical cores each,128GiB aggregate reservation and32decimalGB individual limit.
More cache capacity is a hypothesis, not a monotone-quality claim.
No C++ changes or active builds. Profile1 (used) applies ordinary LRU trimming;
profile2's current-goal-only admission is not active here. The map has38586free
cells ('.', 'E' and 'S' are traversable); counting only'.' undercounts the graph.
At16bytes per free cell, the conservative8GiB capacity is13913 complete tables.
Increasing it may avoid warm-epoch evictions; full-run measurements must decide.
