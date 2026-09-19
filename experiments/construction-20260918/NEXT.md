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

## Current checkpoint: 14:28 UTC

- Transaction full8898717/8898718 is complete, all six cases valid and both
  disabled trajectories exact. One-owner mean134867.5 (+0.262%); two-owner
  mean134970.5 (+0.339%). Two-owner seed2 creates135357 at14:13:43UTC.
  Remaining seeds1/3/4/5 are running as **8898725 /8898726**, two profiles,
  eight concurrent cases,32physical cores/128GiB aggregate, exclusive research46.
- Pickup full8898721/8898722 is complete. All eight cases valid, four weight1
  control hashes exact. Weight3 mean134295.5 slightly loses. Weight5 seed0
  creates136149 at14:19:27UTC, but seed2 collapses to52444 (final4447).
  Neither weight is promoted. The chronological record retains both new peaks.
- Optional pool exchangev37 is implemented. Build8898723 and screen8898727 pass;
  disabled prefix is exact. Complete-source archive verified. Full **8898728 /8898729**
  compares disabled/exchange/exchange+two-owner on seeds0/2 concurrently. Four
  physical cores/process,72GiB aggregate, individual1-second/32GBchecks.
  See POOL_EXCHANGE.md for cost guards, protection, release and retarget semantics.
- Fable turn06 is complete, reviewed source45ebea2, same UUID. No concrete
  atomicity counterexample. Potential fractional-score drift and multi-root
  coverage need independent tests; no proven defect in tested integer profile.
  Preserve and assess the visible review as recorded in fable-flow-session/.

## Next decisions

1. Finish transaction six-seed confirmation and full exchange evaluation.
   Verify complete trajectory hashes, timing, RSS, final windows, task ages and
   release statistics. No short-prefix rankings or single-seed promotion.
2. Get a read-only review of the new pool pass in the SAME Fable session, sending
   only changed excerpts and completed-result corrections. Preserve source-based
   critique separately from performance predictions and unproven liveness claims.
3. Add meaningful transaction multi-root/unlock/rollback, edge-only conflict and
   fractional-weight regressions before altering the proposed floating-point
   comparison. Existing frozen binaries are unaffected by future source edits.
4. Investigate any exchange benefit or late failure before increasing search work.
   More CPU alone has yielded small gains; this does not prove all neighborhoods
   are saturated. Pickup5 collapse reinforces the need for full horizons.
5. Maintain ../../WAREHOUSE_PROGRESS.md and exact commit-linked source provenance;
   the best robust six-seed policy remains ordinary refresh512, mean134590.3.

## Persistent Fable consultation

Use Claude Code CLI, claude-fable-5-1, effort max, SAME session
1ebb1075-3538-49d1-93d1-a00c94fa256a, via fable-flow-session/run_review.py.
Standing explicit user permission covers relevant project information and source.
Prepare bounded, hashed payloads; resume the existing session and send changed
source excerpts only. No tools or edits are allowed in the consultation. Raw
protocol/session data stays under ignored runs/. Archive visible reviews and
usage/status metadata only. Turns 01–06 complete. Independently assess advice and predictions; reviews are
not a substitute for complete benchmark and correctness evidence.


Fable turn05 is complete in the same UUID: 243.802 seconds, $7.90448025, three
changed/new source excerpts, 17973 payload bytes. It favors pool exchanges and
requires a pickup-saving guard independent of chain length. Its claimed snapshot
throughput ceiling and claim that this guard eliminates task-mix effects are not
established. See fable-flow-session/assessment.md for the independent assessment.
The existing TaskManager validates releasing unopened tasks and preserves their
reveal times; changing started tasks is rejected. A future pool pass must also
preserve primary, recovery, pocket, fair-admission and finite-retargeting rules.

