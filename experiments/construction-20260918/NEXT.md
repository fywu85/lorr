# Continuing warehouse work

Goal: at least152981 tasks over5000 steps with10000 robots, repeatably over six
seeds, complete one-second decisions, process RSS below32000000000 bytes and
isolated GRID physical cores. No map identities, aisle templates, supplied weights,
future tasks or map-specific fleet caps. The local KittyKnight target used38.858GB
RSS; it is a local reference, not an official equal-resource score. Goal is active.
Maintain ../../WAREHOUSE_PROGRESS.md with exact source/commit/time provenance.

## Completed evidence

- Current ordinary-refresh reference:134511/134859/134519/134061/134626/134966
  over seeds0..5, mean134590.3. All valid and exactly repeatable. Matched no-flow
  mean109173. Resource usage239–258ms average complete step,324–340ms p99 in a
  separate two-seed audit,793ms worst recent ordinary repeat,1.2average CPU cores
  out of four reserved,13.12GB peak RSS and21–23minute full runs.
- New full turn-build32/64/96/128 matrix8898743/8898744 is complete, all eight
  valid with exact controls. Means134515/136302.5/136600/136950.5 on seeds0/2.
  Every larger count wins both seeds. Count128 improves1.811%; count96seed0 sets
  the new137490 record at15:10:30UTC, exact source5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27.
  Sampled temporal exact-metric coverage rises from75.7% to97.4% at128. These are
  every200-step samples, not a full-step causal decomposition. See TURN_BUILD_LIMIT.md.
- Refreshed margin25 versus50 completes8898732/8898733:136888/135960 versus
  134511/134519, mean136424 (+1.419%), both final windows improve. Margin75 and
  doubled strength lose. Frozen-field losses did not predict refreshed behavior.
  Source45ebea2680e393a1e3ae060f29194b494bedc254 exactly matches recorded production hashes.
- Pool exchange completes all six paired seeds8898745/8898746:135409/135476/
  134762/135078/134311/134009, mean134840.8 (+0.186%), wins four and loses two.
  Not promoted. Outstanding age p90 worsens on every seed; all tasks retarget at
  most once and never after pickup. Full controls exactly repeat. Source24a576c.
- Pool travel audit8898747 is complete. Empty robot-steps improve only0.455%/
  0.430% on seeds0/2; completed chains are slightly longer. Latest-assignment
  pickup delay falls about10%, but that clock resets at retarget. First-assignment
  delay rises slightly. Do not claim those restarted clocks measure fleet savings.
- Two-owner transactions complete six seeds with mean134202.5, -0.288%; wins
  three, loses three. All12valid; all six controls exactly repeat. Not promoted.
- Direct-cost pickup5 has a historical136149 seed0 peak, but52444 seed2 with4447
  in its final1000steps. Rejected. Pickup3 slightly loses too. HRRN0 shortens
  loaded chains but adds17%empty travel. More full search workers add only0.204%
  on two seeds; reducing each worker's work collapses. No saturation claim.

## Running full evaluations

- 8898750 /8898751: refreshed margin25 versus50, seeds1/3/4/5, frozenv36,
  raw runs/cgar-flow-refresh-margin25-confirm-v36-20260919. Started15:06:19UTC.
- 8898754 /8898755: temporal table batches disabled/128serial/128parallel/
  512parallel, seeds0/2, frozenv40, raw runs/cgar-temporal-table-batch-full-v40-20260919.
  Started15:16:19UTC, exclusive research57.
- 8898756 /8898757: turn-build128 versus32, seeds1/3/4/5, frozenv39,
  raw runs/cgar-turn-build-limit-confirm-v39-20260919. Started15:16:49UTC,
  exclusive research50.

Each matrix has eight simultaneous cases with four disjoint physical cores per
planner and128GiB aggregate reservation. Individual memory limit stays32decimalGB.
All C++ builds are complete; running matrices use frozen binaries.

## New implementation and verification

V40 optionally admits a fixed batch of complete current-goal turn tables after
PIBT and protected-support closure, before temporal scoring. Ranked by demand,
oldest ticket and goal ID. Existing workers construct chunks up to32, then every
selected result is admitted in rank order after joining. Additional work is distinct
from the old demand policy, whose default build limit32 remains unchanged.
CGAR_TEMPORAL_TABLE_BATCH defaults0/off; CGAR_TEMPORAL_TABLE_THREADS defaults1.
Thread count is validated against affinity. Deadline failure rejects the whole run.
See TEMPORAL_TABLE_BATCH.md.

Build8898749 passes all regressions. New tests compare12800 exact robot actions
between one/four table threads, multiple chunks and five field publications;
primary/recovery/capacity tests also run enabled. Archivedv40 patch independently
reconstructs all20 recorded hashes. Screen8898752 passes disabled,128serial,
128parallel,512parallel. Disabled fingerprint c7585218... is exact;128serial and
parallel both hash01d2b4c3... . Batch1024 fails explicitly at timestep128, elapsed
1000.641ms, stage turn_distance_table_complete. Excluded from full testing.
No short-prefix quality ranking; no partial decision accepted.

V38 transaction safety tests cover multi-root commit/rollback/reuse with fractional
scores, later budget exhaustion preserving earlier commits, edge-only conflicts,
size boundaries and512 tiny fixtures with57200 enumerated combinations. No
fractional-score drift counterexample was reproduced. Production logic unchanged.

## Next decisions

1. Independently analyze completed full matrices, requiring exact controls, all
   steps, explicit deadline and RSS checks. Batch128 serial/parallel must reproduce
   full fingerprints before a runtime speedup is claimed.
2. Confirm margin25 and turn-build128 over all six seeds. If useful, test their
   interaction with matched constituent controls; do not assume gains add.
3. Examine actual batch build counts, sampled coverage and component timings.
   More guidance helps these two tested seeds, but traffic feedback can reverse
   a gain. No per-map rule is needed for current selection/admission methods.
4. Keep pool/transactions optional; current six-seed gains do not justify a new
   default. Apply concrete review defects with meaningful regression coverage.
5. Commit/push validated checkpoints and maintain historical records, including
   rejected policies. Only mark the goal complete after full six-seed target evidence.

## Persistent Fable consultation

Claude Code CLI, claude-fable-5-1, effort max, SAME UUID
1ebb1075-3538-49d1-93d1-a00c94fa256a via fable-flow-session/run_review.py.
Standing permission covers relevant project source/info. Prepare bounded hashed
payloads, update excerpt ranges after source edits, and send only changed context.
Tools and edits are disabled. Archive visible review/usage/status only; raw protocol
remains under ignored runs/. Turns01–07 complete. Turn08 is running in the same
session from15:24:28UTC, raw runs/fable-cgar-session/turn08, exec session37448.
Initialized model is claude-fable-5-1 with tools disabled. The92771-byte hashed
payload contains five changed source excerpts and the corrected completed results.

Fable07 found no concrete pool safety defect. Its pending-primary context concern
was resolved: select_primary does not exclude pending robots merely due to locks
or recovery. Claims about global fairness, all robot cycles and static-score savings
remain qualified. Suggested extra boundary/round-trip tests are not observed bugs.
Turn08 reviews v40 admission and joined parallel behavior, with completed six-seed
transaction/pool evidence and new turn-build/margin gains. Await the existing
process; do not restart the session. Archive only visible review/metadata/status.

Checkpoint50e8e6e4cef1495a4054590e613cf762eb39009c is pushed and exactly matches
all20 recorded v40 production/test hashes. All three full matrices remain active;
no full batch cases have completed as of15:24:58UTC.
