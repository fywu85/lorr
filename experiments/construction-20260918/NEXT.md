# Continuing warehouse work

Goal: at least152981tasks over all5000steps with10000robots, repeatably over six
seeds, complete one-second decisions, process RSS below32000000000bytes, isolated
GRID physical cores. No map identities, aisle templates, supplied weights, future
tasks or map-specific fleet caps. The local KittyKnight target used38.858GBRSS;
it is a local reference, not an equal-resource official score. Goal remains active.
Maintain ../../WAREHOUSE_PROGRESS.md and exact source/commit/timestamp provenance.

## Confirmed evidence

- Leading ordinary refresh512:134511/134859/134519/134061/134626/134966 over
  seeds0..5, mean134590.3. Matched no-flow mean109173. All decisions valid; exact
  repeated trajectories; stable final windows. Still below the target.
- Historical peak136149: direct-cost pickup5seed0, but seed2collapses to52444,
  including4447in the final1000. Not promoted. Pickup3mean134295.5also loses.
- Two-owner transactions finish all six seeds:134584/135299/135357/132125/
  133325/134525, mean134202.5, -0.288% versus matched disabled. Wins3of6and
  loses3of6. All12paired cases valid; all six controls exactly reproduce.
  Not promoted. One-owner has only a small two-seed gain, not six-seed proof.
- All prior cache-only, gentler cost, worker, HRRN/refinement and audit matrices
  are complete. Actual traffic updates matter beyond cache flushing. Four full
  workers add0.204% on two seeds; reduced per-worker work collapses. HRRN0
  shortens chains but adds about17%empty travel. No broad search-saturation claim.
- Confirmed resource usage:239–258ms mean complete step,324–340ms p99in a
  separate two-seed audit,793ms worst recent repeat,1.2average CPU cores with
  four reserved,13.12GBpeak RSS,21–23minute full runs. Hosts isolated/unthrottled.

## Running work

- Pool full8898728/8898729is complete, all six valid. Pool-only135409/134762,
  mean135085.5 (+0.424%); combined134902/134926. Exact disabled controls. Empty
  robot-steps fall only0.455%/0.430%; task-age p90rises to702/722. Each of14546/
  14665reassigned tasks changes owner at most once; no changes after pickup.
- **8898745 /8898746**: pool-only versus disabled confirmation, seeds1/3/4/5,
  eight concurrent cases, four physical cores/process,128GiBaggregate, source24a576c.
- Pool travel audit8898747is complete and archived in results/pool-exchange-travel-v1.
  Completed chains are slightly longer. Latest-assignment pickup delay falls
  about10%, but first-assignment delay rises slightly and total empty travel
  falls only0.4%. Do not interpret restarted pickup clocks as full-fleet savings.
- **8898732 /8898733**: full refreshed flow neighbors on frozen v36 source,
  strength1margin50control, strength1margin25/75, strength2margin50, seeds0/2,
  eight concurrent cases,32physical cores/128GiBaggregate, exclusive research50.
  Screen8898730 passes all four with exact control. Earlier losing frozen-field
  versions do not settle effects under repeated actual field updates.
- **8898743 /8898744**: full turn-table build allowances32/64/96/128, seeds0/2,
  eight concurrent cases,32physical cores/160GiBaggregate. Per-process limit
  remains32decimalGB. Screen8898739passes all, max0.881s/RSS5.21GB, default
  prefix exact. Build8898734/archivedv39passes. See TURN_BUILD_LIMIT.md.

All C++ builds are complete. Running benchmarks use frozen binaries. No Fable
process remains running: turn07completed at14:33:37UTC in the SAME session.

## New diagnosis and implementation

Every200-step samples of ordinary refreshed controls have only75.7%exact
turn-aware metric coverage on average, as low as16.1–16.8%after invalidations.
The old hardcoded32table rebuilds/step is now optional CGAR_TURN_BUILD_LIMIT,
range0–256, default32. Cache space alone cannot remove that count limit.
No partial table enters the cache and a missed complete-entry deadline fails.
V39changes only this configurable count and its diagnostics; no performance gain
is claimed before full results. Source patch independently reconstructs all hashes.

V38adds transaction soundness coverage: equal/fractional multi-root commit,
rollback and owner reuse; retention of earlier commits after later work exhaustion;
edge-only swap conflicts; exact size bounds;512tiny cases with57200enumerated
candidate combinations and211valid accepted changes. Production logic unchanged;
no fractional-score drift counterexample was reproduced. V39retains these tests.

Fable07finds no concrete pool safety defect. Direct inspection of select_primary
resolves its missing-context concern: locks/recovery do not exclude the oldest
pending robot. Timeout recovery within a continuing run is not promised. Static
pickup guards ignore heading/tolls and released-task future cost; task-mix and
fairness claims remain qualified. All six initial full simulator runs validate. Age tails worsen while empty
travel improves slightly; completed-chain decomposition and six-seed confirmation
remain necessary before promotion.

## Next decisions

1. Finish each full matrix and independently analyze trajectories, complete timing,
   RSS, task ages, last1000steps and release counters. Controls must reproduce.
   Do not rank200-step prefixes or promote a single high seed.
2. If pool exchange helps, examine empty travel, changed task mix, release counts
   and age tails; confirm promising settings on all six seeds. The pure two-owner
   policy is rejected, though its interaction arm is already running.
3. For larger table allowances, check actual exact/fallback coverage and build
   counts before attributing any throughput effect. If helpful, improve the
   construction implementation or parallel allocation based on measured cost;
   do not assume more cache or speculative prefetch is equivalent.
4. Apply concrete safety findings with meaningful regressions. Additional pool
   fresh-assignment/round-trip/guard-boundary tests are useful if the policy is
   retained. Do not describe successful local-score search as proof of task gains.
5. Commit/push validated checkpoints and update throughput history, including
   reversals. Only declare the goal complete after full six-seed target evidence.

## Persistent Fable consultation

Claude Code CLI, claude-fable-5-1, effort max, SAME UUID
1ebb1075-3538-49d1-93d1-a00c94fa256a via fable-flow-session/run_review.py.
Standing user permission covers relevant project source/info. Prepare bounded,
hashed payloads and send only changed excerpts with corrected completed evidence.
Tools and edits are disabled. Archive visible review/usage/status only; raw
protocol stays under ignored runs/. Turns01–07complete. No restart is warranted
on an observation timeout. Current source-spec line ranges need refreshing before
any new payload because regression/source line positions changed after turn07.

Potential next implementation: parallel admission of a fixed number of missing
current-goal turn tables before temporal candidate preparation. Existing prefetch
computes chunks of32in parallel but leaves them invisible until normal demand;
that old speculation discarded93.47%. A future optional batch would admit every
chosen complete table deterministically, then score candidates. Compare serial/
parallel batch admission separately from the old demand policy. Keep the default
unchanged, include thread count in allocation validation, join failures, and reject
late complete decisions. This is not implemented. Wait for rebuild-limit evidence
before choosing the production count; more guidance is a hypothesis, not a gain.
The older assigned-to-assigned swap flag has never been tested with refresh512;
it is another possible generic assignment ablation, not a proven improvement.

Validated source checkpoint5c7f0fa is pushed; v39exactly matches all20recorded
production and test hashes. All C++ sources are currently complete and tested.
