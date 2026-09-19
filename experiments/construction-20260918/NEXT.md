# Continuing warehouse work

Active unbudgeted goal: >=152981tasks over5000steps/10000robots, repeatably over
six seeds, every complete schedule+plan entry<=1s, explicit timeout failure, RSS
<32000000000B, isolated GRID physical cores. No map identity/templates/supplied
weights/future tasks/map-specific fleet caps. Local KittyKnight used38.858GB;
this is not an official equal-resource comparison. Goal remains active and unmet.
Prior goal turns made progress: source/tests/builds, full evidence and pushed
checkpoints. No blocker. Maintain ../../WAREHOUSE_PROGRESS.md.

## Confirmed reference

Graded strength4/scale4, margin25, refresh512, turn-build128,8GiBcache, ordinary
HRRN1/weight1 and pickup flow OFF. warehouse-reference-variants.json is explicit.
Seeds0..5=138402/139716/139303/138506/138304/139457, mean138948 (+0.611% over
binary margin25/build128). All six totals improve; four final windows improve,
seeds2/4 lose153/134. All12paired runs valid, six controls exactly reproduce.
Source5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27, frozenv39.
Mean229.3–277.3ms, median198.2–257.5ms, p99 509.0–567.5ms, max0.874164s,
CPU1.174–1.215cores/4reserved, RSS11.936GB,20.33–24.36minutes.
results/flow-graded-six-seed-v39.json and graded-reference-latency-six-seed-v39.json.

WAREHOUSE_PROGRESS has33single-run records (latest139796is rejected margin0).
New confirmed-reference-progress.json tracks four increasing six-seed milestones
with actual last-candidate completion timestamps and exact source commits.
Preceding references: binary25/build128 mean138104.17; binary50/build128137073.33;
binary25/build32136426.17; binary50/build32134590.33.

## Completed evidence

V41 pickup flow, sourceeb5b3827b9daf9f91aba88c78bef7be0d17cd07c:
- HRRN1/weight1:137508/137923 vs137060/136841, mean+0.559%; both finalwindows up,
  whole-run empty robot-steps down~2.0%/1.5%, agep90 up653/661vs647/646.
- HRRN0/weight5:138242/49732 vs137603/136560. Seed2final4522; reject collapse.
- All8valid and HRRN1 controls exact. Snapshots at t4800 (not full-run counters)
  show~96.5%cached probes,~1270cells/search,~34%work-limited normal searches.
  Probe shares are not assignment shares; cost counters mix physical metrics.
  results/pickup-flow-full-v41/comparison.json and PICKUP_FLOW.md.

Cache8/16GiB at binary25/build128: all four full trajectories exactly match,
137149/139037 at both sizes. Guided/fallback counts also exact, but16GiB saves
197/217table rebuilds. Keep8GiB; no throughput benefit. This does not prove cache
capacity never matters in other policies. results/turn-cache-capacity-full-v39.

Graded margin0 remains rejected56537/139796. Churn audit8898800 completed all
four full traces and all40publications, matching move/penalized counts and movement
totals. At refresh640, collapsed seed0 has only3.87%more pair churn and7.50%more
sign flips than healthy seed2; later churn is lower. Simple global-churn precursor
not supported, no hysteresis change. See FLOW_CHURN.md. Production logs no full
field vector hashes; count matching is not field-hash verification.

Other rejected directions: batch128six-seed mean136957.33 vsstronger137073.33,
batch512seed2regresses despite100%sampled exact coverage; transactions/pool
have mixed6seed results. More coverage is not a monotone throughput measure.

## Running full comparisons

- V41graded normal pickup-flow OFF/ON seeds0/2 completed:138963/139677 vs exact
  OFF138402/139303, mean+0.337%, finalwindows+244/+6, empty robot-steps down
  1.63%/1.46%, agep90 up18/20steps. All4valid. comparison.json archived.
- 8898858 /8898860: remaining seeds1/3/4/5 of the same paired comparison,
  8concurrent cases x4cores,128GiB aggregate,32GB perprocess. Raw
  runs/cgar-graded-pickup-flow-confirm-v41-20260919. OFFmustmatchgradedreference.
- 8898851 /8898853: v42complete pickup fields0/16/32/64, all pickup-flow ON and
  graded, seeds0/2,8concurrent x4cores,128GiB aggregate,32decimalGB perprocess.
  Raw runs/cgar-pickup-full-full-v42-20260919. Control must exactly match v41ON.
  This control is experimental, not the confirmed graded/OFF reference.

## V42 checkpoint

Production adds pickup_full.hpp and optional CGAR_PICKUP_FULL_ROBOTS0–64
(default0), CGAR_PICKUP_FULL_THREADS1–32(default4), requiring pickup flow.
Complete positive-integer forward fields use17buckets and reusable per-worker
scratch. Every selected field finishes before mapping results to robots. Existing
run_temporal_preparation joins all workers on success/exception/thread failure.
No clock-selected partial fields. Rotating fixed quota; other free robots retain
existing searches/fallback and still receive assignments. Selected robots rank
all visible eligible tasks, keeping existing top16/top8 and total pair limits.
Finite field costs also serve their fair/fallback estimates. No map-specific policy.

Build8898839 passed all regressions. Test-only quota extension rebuilt8898842,
runs/cgar-pickup-full-build-v42-r1-20260919, ALL checks pass. Independent32256
costs now validate heap and complete bucket searches; additional tests check12fields
serial/parallel, injected in-search failure/joins, scratch reuse, invalid config,
warmup, better task beyond16nearest, fairness/started protection, and two free
robots under a one-field quota with unique assignments/metadata isolation.
Archive build-provenance/v42 independently reconstructs all22source/test hashes.
No C++ build is active and source has not changed since that build.

Screen8898846:5/5valid, max0.892714s, RSS5.224GB. Full0 exact v41ON prefix;
16serial/parallel whole prefixes exact. No short-prefix throughput rankings or
full-memory claims. results/pickup-full-screen-v42; details PICKUP_FULL.md.

## Persistent Fable

Session1ebb1075-3538-49d1-93d1-a00c94fa256a, Fable5.1max via Claude CLI;
explicit user source-sharing permission persists. Turns01–09 complete. Turn09
no reproducible v41bug, no tests run by Fable; visible review/metadata/status
archived. Raw protocol remains ignored. Actual25.497911USD/262.410s; requested
12USD flag is not a reliable hard cap. Qualified assessment rejects rare-cache/
half-reach/only-flow-lever assertions unsupported by full evidence.

Next review is turn10 with followup-09.md, once exact tested v42source is committed.
Check actual process/session status before a repeat; preserve same session and
send incremental source only. Do not include raw protocol/hidden reasoning in Git.

## Next actions

1. Commit/push validated v42source/archive/screens and completed v39/v41reports;
   verify the committed tree against all22requested source/test hashes.
2. Run persistent Fable turn10 review while full comparisons continue.
3. Analyze complete v41combined and v42wide-candidate outcomes, exact controls,
   all5000entry times/RSS/validity, finalwindows/ages and full travel. Extend
   promising improvements to the remaining four seeds before promotion.
4. Retain full objective and accurate source/timestamp history; no target achieved.
