# Continuing warehouse work

Goal: at least 152,981 tasks over all 5,000 steps, repeatably over six seeds,
complete one-second decisions, process RSS below 32,000,000,000 bytes, and
isolated GRID physical cores. No map-name branches, aisle templates, supplied
weights, future tasks, or map-specific fleet caps. The local KittyKnight target
used 38.858 GB RSS; it is a local reference, not an equal-resource official score.
The goal remains active. Maintain ../../WAREHOUSE_PROGRESS.md after full results.

## Current evidence

- Ordinary refresh512 is confirmed on all six seeds: 134,511 / 134,859 /
  134,519 / 134,061 / 134,626 / 134,966; mean **134,590.3**. All valid,
  final windows 28,167–28,368. Same-work no-flow mean **109,173**. Repeated
  trajectories exactly reproduce. This establishes six-seed consistency, not
  achievement of the leader target. See results/flow-refresh-six-seed-v30.json.
- Single-run peak **135,177**, strict wait turns plus refresh512, seed 0.
  Its two-seed mean slightly loses to ordinary refresh; do not promote it.
- Cache-only causal control is complete: real traffic updates beat cache-only
  in all four strict-mode/seed contrasts. In healthy frozen controls, real
  updates add 8.74–9.84% above cache-only. Legacy seed 2 still collapses late
  with cache-only. See FLOW_CACHE_ONLY.md; do not attribute refresh to flushing.
- Warm reuse plus refresh512 gains 0.283% on seeds 0/2. Refresh256 slightly
  loses. Gentler refreshed costs lose 1.16% / 2.74%. None is a large next gain.
- Four full-size fixed workers gain only 0.204% on seeds 0/2, using 2.60–2.63
  average CPU cores and 271–277 ms mean decisions. Four smaller 1M workers
  deteriorate severely; their mandatory construction often consumes or exceeds
  the nominal allowance, leaving no repair. This is not exact equal work.
- Ordinary refresh uses about 1.2 CPU cores, with four physical cores reserved.
  Six-seed mean entries 239–258 ms, max 776 ms; a separate repeat reaches
  793 ms. Peak RSS 13.12 GB. Full runs take 21.2–22.8 minutes.
- Full v34 audit reproduces both baseline hashes. The best forward candidate
  is rarely unblocked, but a non-best improving alternative may be. Do not use
  these snapshots as rejected-attempt histories or predicted task gains.
- Travel audit: loaded elapsed/shortest ratio 1.1745 versus local KK 1.0799;
  completed chain lengths 242.32 versus 233.84. Cohorts differ, so this is not
  a causal decomposition. Dwell concentration does not justify new cell tolls.

## Running work at the 13:55 UTC checkpoint

- **8898717 / 8898718**: v36 transaction repair, disabled / one-owner /
  two-owner, seeds 0/2. All six full cases run concurrently on exclusive
  research50, 24 physical cores / 72 GiB aggregate. Each has four disjoint cores
  and individual 1-second / 32 GB checks. Build8898706 and screen8898715 pass.
  Immutable input, separate reservation table, bounded backtracking and exact
  rollback; no partial result on deadline. See TEMPORAL_TRANSACTIONS.md.
- **8898721 / 8898722**: pickup-weight full matrix on frozen v33. HRRN1/weight1,
  direct-cost weights1/3/5, seeds0/2; eight concurrent cases, 32 physical cores
  and 128 GiB aggregate reservation. Screen8898719 passes all four cases,
  including both exact controls; maximum entry is 0.8084 seconds.

All earlier matrices are fully analyzed. The HRRN/refinement matrix loses with
HRRN disabled: 131199/130947 versus 134511/134519. Refinement makes no change to
both direct-cost trajectories and only a three-task change under HRRN. The travel
audit8898716 finds shorter completed chains235.27 versus242.32, but 17–18% more
empty robot-steps. Test stronger pickup weighting before crediting a shorter chain
mix as a throughput improvement. HRRN remains enabled in the leading policy.

Expanded v35 full audit8898688/8898689 is complete and exactly reproduces both
baseline trajectories. Final-window physical opportunities per step are roughly
4 free, 350–356 one-owner, 171–175 two-owner and 44–47 three-plus. No new best.
No C++ build remains active. Every running benchmark uses its own frozen binary.

## Next decisions

1. Finish the one-owner/two-owner transaction and pickup-weight matrices. Verify
   all full trajectories, complete timing samples, memory, task-age tails and
   final windows. Keep the current leading configuration until an improvement
   survives appropriate seed confirmation.
2. Develop a bounded exchange of an en-route robot's unopened task with an
   available task if the pickup results still leave this useful. Require a pickup
   saving independently of chain cost, and a non-worsening total estimate. Use
   consistent complete core distances; never compare Manhattan to exact BFS.
   Preserve primary, recovery, pocket, capacity and fair-admission protections,
   original reveal times, robot cooldowns and a finite release limit per task.
   Account for the released task remaining in the pool. No pool exchange is
   currently implemented. See Fable turn05 and its independent assessment.
3. Get a source review of the implemented transaction logic in the SAME Fable
   session, supplying only new/changed files and the relevant regression excerpt.
4. Keep full horizons for quality claims: both frozen flow and reduced search
   can collapse late. More CPU alone has not closed the gap. Do not claim that
   every alternative search neighborhood is saturated.

## Persistent Fable consultation

Use Claude Code CLI, claude-fable-5-1, effort max, SAME session
1ebb1075-3538-49d1-93d1-a00c94fa256a, via fable-flow-session/run_review.py.
Standing explicit user permission covers relevant project information and source.
Prepare bounded, hashed payloads; resume the existing session and send changed
source excerpts only. No tools or edits are allowed in the consultation. Raw
protocol/session data stays under ignored runs/. Archive visible reviews and
usage/status metadata only. Turns 01–05 complete. Independently assess advice and predictions; reviews are
not a substitute for complete benchmark and correctness evidence.


Fable turn05 is complete in the same UUID: 243.802 seconds, $7.90448025, three
changed/new source excerpts, 17973 payload bytes. It favors pool exchanges and
requires a pickup-saving guard independent of chain length. Its claimed snapshot
throughput ceiling and claim that this guard eliminates task-mix effects are not
established. See fable-flow-session/assessment.md for the independent assessment.
The existing TaskManager validates releasing unopened tasks and preserves their
reveal times; changing started tasks is rejected. A future pool pass must also
preserve primary, recovery, pocket, fair-admission and finite-retargeting rules.
