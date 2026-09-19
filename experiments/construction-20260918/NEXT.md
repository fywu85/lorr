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

## Running work at the 13:34 UTC checkpoint

- **8898684 / 8898685**, full assignment matrix on exclusive research46:
  HRRN 0/1 crossed with chain-cost refinement 0/1, seeds 0/2. Eight cases run
  concurrently on 32 physical cores and 128 GiB aggregate allocation; each
  process gets four disjoint cores and its own 32 GB RSS validity check.
  All four short screens pass. Default matches the prior fingerprint.
  Frozen v33 binary. See FLOW_REFRESH_DISPATCH.md.
- **8898688 / 8898689**, expanded read-only forward audit on research38:
  v35, stride67, seeds 0/2, two concurrent cases, four physical cores each,
  24 GiB aggregate reservation. Find the easiest alternative among all strict
  improvements, separately requiring one full physical objective unit after
  removing operation tie terms. Full trajectory equivalence is still required.
  See FORWARD_CONFLICT_AUDIT.md.

All earlier matrices through refreshed scales and fixed workers are fully
analyzed. No C++ build remains active; all running benchmarks use frozen binaries.
V35 build8898686 passes all regressions and its patch reconstructs every source
hash. Screen8898687 preserves both exact fingerprints and partition conservation.
No branching search or free-forward sweep has been implemented.

## Next decisions

1. Finish assignment and expanded-audit analyses; verify full fingerprints,
   all 5,000 timing samples, memory, task-age tails and final windows.
2. If many stationary robots have an unblocked physical improvement, test a
   bounded deterministic monotone sweep. If easiest improvements require two
   movable owners, consider a bounded branching transaction with exact rollback.
   Non-fixed owners are not guaranteed to be displaceable.
3. Investigate exchanging an unopened assignment with an available task, using
   only current information. The simulator permits releasing unopened tasks;
   started tasks cannot move. Require consistent distance bases and preserve
   primary, recovery, pocket, capacity, fair-admission and finite-retargeting
   protections. First assess opportunities; no new release behavior is enabled.
4. Keep full horizons for quality claims: both frozen flow and reduced search
   work can look reasonable early and collapse late. More CPU alone has not
   closed the gap. Do not claim saturation of all search neighborhoods.

## Persistent Fable consultation

Use Claude Code CLI, claude-fable-5-1, effort max, SAME session
1ebb1075-3538-49d1-93d1-a00c94fa256a, via fable-flow-session/run_review.py.
Standing explicit user permission covers relevant project information and source.
Prepare bounded, hashed payloads; resume the existing session and send changed
source excerpts only. No tools or edits are allowed in the consultation. Raw
protocol/session data stays under ignored runs/. Archive visible reviews and
usage/status metadata only. Turns 01–04 complete; turn04 uses one CLI turn,
325.870 seconds and $6.29122725. Independently assess advice and predictions.
