# Continuing warehouse work

Goal: at least152981 tasks over5000 steps with10000 robots, repeatably over six
seeds, complete one-second decisions, process RSS below32000000000 bytes and
isolated GRID physical cores. No map identities, aisle templates, supplied weights,
future tasks or map-specific fleet caps. Local KittyKnight target used38.858GB RSS;
this is not an official equal-resource comparison. Goal remains active and unmet.
Maintain ../../WAREHOUSE_PROGRESS.md with exact source/commit/time provenance.

## Current confirmed reference

Margin50 with turn-build128, explicit warehouse-reference-variants.json:
137060/137271/136841/137227/136949/137092 on seeds0..5, mean137073.3 (+1.845%
versus matched build32). All six totals and final windows improve; all12paired
runs validate and all six build32 controls exactly repeat. It also beats margin25/
build32 on every seed. Maximum entry0.873055s, mean234.8–269.9ms, CPU1.182–
1.210cores out of four reserved, peak12.013GB RSS and20.80–23.75minute full runs.
No new p99 aggregation. Exact source5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27.
Evidence results/turn-build-limit-six-seed-v39.json.

Margin25/build32 was an intermediate six-seed reference:136888/135838/135960/
136885/136582/136404, mean136426.2 (+1.364%). All totals/final windows improve
and task-age p90 falls against margin50/build32. Maximum0.787348s and peak
14.052GB. Exact source45ebea2, results/flow-refresh-margin25-six-seed-v36.json.
Original margin50/build32 mean134590.3 remains a stable historical control.

## New complete batch evidence

V40 all eight full cases complete and independently validate. Batch128 serial and
parallel BOTH score138003/136701. Entire trajectories, orientation counts and
sampled admission/coverage exactly match on each seed. Four-thread construction
reduces full wall time5.864%/6.374%, from1425.910/1335.982s to1342.289/1250.833s.
Parallel mean235.1–253.5ms, CPU1.281–1.300cores, peak12.621GB. Two-seed mean
137352 is2.109% above old build32 but only0.293% above build128, with one win
and one loss against that stronger control. Further confirmation is required.

Batch512 scores137848/119685; seed2 final1000 falls to16880. It is rejected.
All25 sampled steps have100% exact guidance in both batch512 seeds; more sampled
coverage is not monotonically related to throughput. Batch128 averages98.62%/
98.64%. Exact source/test tree50e8e6e4cef1495a4054590e613cf762eb39009c.
New peak138003 completes15:38:41UTC in parallel seed0; earlier137848 at15:37:18
is retained as a historical record despite its policy regression. Thirty records.
Evidence results/temporal-table-batch-full-v40/comparison.json.

## Running full evaluations

- 8898759 /8898760: margin50/25 crossed with build32/128, seeds0/2, frozenv39.
  Raw runs/cgar-flow-turn-interaction-full-v39-20260919, start15:33:19UTC,
  exclusive research38. New combination screen passes; three controls exact.
- 8898765 /8898766: batch128parallel plus demand32 against batch0 plus demand128,
  seeds1/3/4/5, frozenv40. Raw runs/cgar-temporal-table-batch-confirm-v40-20260919,
  start15:47:09UTC, exclusive research46. Stronger control hashes must exactly
  reproduce all four previous build128 cases before combining six-seed evidence.
- 8898768 /8898769: binary margin50/25 against graded strength4/scale4 margin25/0,
  all at build128, seeds0/2, frozenv39. Raw
  runs/cgar-flow-graded-turn128-full-v39-20260919. See FLOW_GRADED.md.
  Screen8898767 passes all, max0.874s/RSS5.226GB, both binary controls exact.

Each matrix has eight concurrent cases, four disjoint physical cores per planner,
128GiB aggregate reservation and32decimalGB individual limit. All C++ builds are
complete. No C++ edits have been made since v40; live runs use frozen binaries.

## Persistent Fable and next implementation

Same Claude CLI session1ebb1075-3538-49d1-93d1-a00c94fa256a, Fable5.1 max,
via fable-flow-session/run_review.py. User explicitly permits source/info sharing.
Turns01–08 complete; no Fable process remains running. Turn08 completed
15:34:44UTC,612.547s, actual16.015591USD despite requested12USD CLI setting.
Read-only review of five changed excerpts (92771bytes), no tests executed, no
reproducible v40 correctness counterexample. Visible review/metadata/status are
archived; raw protocol stays ignored. Never restart the persistent session.

Checked findings: stable table pointers, deterministic rank/admission, immutable
worker inputs. Mode2's32-step admission lag is irrelevant to production mode1.
Prefetched_hits conflates old speculation and new batch admissions. Per-item
compute/thread-creation failures join; tiny worker bucket allocation is outside
that catch, so do not claim all possible exceptions are proved safe. The1024
screen actually fails inside four-thread table computation and exits124 correctly.
No partial actions are returned. New private-cache retry semantics are not promised.

Reject Fable's blanket claim that search directions are spent and any linear
coverage-to-task extrapolation. Its assignment/motion split is not causal across
cohorts. Pool predictions were overoptimistic: six-seed pool mean134840.8 (+0.186%)
has four wins/two losses and worse ages throughout. Transactions mean134202.5
(-0.288%) wins three/loses three. Neither is promoted. Direct-cost pickup5's
136149/52444 pair is not robust; seed2 final window4447. HRRN0 adds17%empty travel.

Next concrete implementation hypothesis: align local pickup selection with heading
and the complete published traffic cost field, rather than unit BFS. Existing
schedule() local candidate search is unit BFS capped2048cell pops/16tasks; fallback
uses unit table or Manhattan estimates. Generic forward Dijkstra over(cell,heading)
can return only settled pickup costs with a prescribed work bound, leaving full
scheduling/planning/deadline validation mandatory. Preserve old mode by default,
all started/primary/fair protections, robot/task uniqueness and fair admissions.

Before editing, resolve cost units (scaled pickup versus unit chain), physical
free-graph versus certificate/pocket domain, search bounds versus shortlist reach,
and fallback approximate versus exact local costs. Scheduler runs before plan's
flow publication; explicitly define the last-published snapshot or share a
once-per-observation publication helper rather than silently changing phase order.
A fixed warmup/publication condition is allowed; a clock-based policy is not.
The historical runs do not log every candidate shortlist, so exact16-candidate
counterfactuals cannot be recovered merely from aggregate audits. Fable's5% gate
and0.5–2% expected gain are hypotheses, not evidence or required approval gates.

Implement a bounded optional policy with independent small-graph Dijkstra checks,
heading/toll cases, fixed work and timeout propagation, default action equivalence,
and scheduler integration/fairness checks. Then screen and run matched full cases.
Keep C++ unchanged during each GRID build; archive and independently reconstruct
all source hashes before attributing results to a commit.

## Next decisions

1. Analyze full interaction and graded-cost results with exact controls, all5000
   steps, complete-entry timing and RSS. No short-prefix performance rankings.
2. Complete batch128 confirmation against the stronger build128 control. All
   source changes are optional; adopt a reference only from six-seed evidence.
3. Implement the pickup-cost alignment experiment while existing full jobs run.
4. Commit/push validated checkpoints, update historical records and current
   reference configuration, and keep the goal active until all target checks pass.
