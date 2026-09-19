# Continuing warehouse work

Goal: at least 152,981 completed tasks over all 5,000 steps, repeatably over six
seeds, complete one-second decisions, process RSS below 32,000,000,000 bytes.
No map-name branches, aisle templates, supplied weights, future tasks or map-specific
fleet caps. The local KittyKnight target used 16 physical / 32 logical CPUs and
38.858 GB RSS, above our memory limit; it is not an equal-resource official score.
The active goal is not complete.

## Established results and decisions

- No-flow fixed 4M: **109,244 / 109,249 / 109,182** at seeds 0–2, mean **109,225**.
  Every full run passes deadlines and memory, and final-window rates stay stable.
- Frozen strength-1, margin-50, warmup-128 flow: **122,896 / 122,195 / 70,171**,
  mean **105,087.3**, **3.79% below the matched 4M control**. The large seed-2
  collapse is introduced by the flow policy in this comparison. Which traffic
  edges/feedback mechanism causes it is unisolated. **Do not promote.**
- Regional two-round seed 0: 112,164, max entry 0.891 seconds. Global 25k plus
  two rounds: 111,997, max 0.725 seconds. Four cores, about 11.82 GB. Still only
  seed 0. Temperature changes and warm-start-only changes add no material gain.
- Corrected full guides all lose: unit 39,066, opposing-cost 42,051, reconnect
  28,071, refine 33,507, against 109,244. All runs are valid; no guide profile is
  promoted. Reconnect's optional movement logs end at 4,800 because normal
  `_exit` can discard stdout, but all 5,000 trajectory/timing records exist.
  Recovery explicitly marks partial diagnostics; V29 fixes periodic flushing.
- Compact turn tables preserve complete control trajectories while lowering RSS
  from 16.208 to 11.883 GB. V27 regional-only candidate filtering preserves short
  trajectories and improves an isolated regional kernel 6.9%; no full speedup
  is established. V28 parallel preparation preserves all short trajectory fields
  and lowers single-pair 200-step wall times 13.9% / 8.3%; full checks are pending.

## Active full queue

Every matrix reserves 24 GiB total and runs two independent instances concurrently
on reserved EPYC 9354 physical cores. Matrices are serialized. Each case uses
5,000 steps, a complete one-second decision and the 32 GB process limit.

1. **8898554/8898555**, frozen v20-r1, one core per instance: margin-50 control
   against margins 25/75 and strength 2, seed 0. All screens pass; full running.
2. **8898624/8898625**, frozen v30, four cores per instance: frozen flow against
   cumulative refresh intervals 512/1024, seeds 0 and 2. All preparation work
   completes on four threads before unchanged fixed-work search. Tests and
   screens pass; no full refresh result exists yet. See FLOW_REFRESH.md.
3. **8898566/8898567**, frozen v27, four-core allowances: exact full validation
   of regional candidate filtering, global 4M and global 25k/two regional rounds.
   This job was verified pending and moved after the refresh diagnosis; no
   running benchmark was interrupted. See results/flow-refresh-queue-update.json.
4. **8898606/8898607**, frozen v28, four cores per instance: paired one-/four-worker
   preparation for the same two policies. Follows v27. See PARALLEL_PREPARATION.md.

Completed no-flow confirmation **8898558/8898559** establishes the matched
three-seed control. Completed guide full **8898550**, original failed analysis
8898552, and explicit recovery **8898620** are preserved with all limitations.

## Current source and next decisions

V30 adds optional `CGAR_FLOW_REFRESH_INTERVAL` (default 0). Complete cumulative
fields publish only after fixed numbers of consecutive observations. A changed
metric discards stale tables; an unchanged field preserves them. Cache rebuilding
is a real cost to measure. Build 8898622 passes independent hand-counted traffic,
640 transformed-edge checks, skipped-observation handling and protected serial/
four-thread warm-start episodes. Its source patch reconstructs all hashes.

Screen 8898623 passes default/64/512 intervals. Only interval 64 actually
refreshes within 200 steps (at 192); interval 512 exactly matches the default
prefix and must test its first refresh in the full run. No prefix-throughput
ranking is used. Load-cost guide screen 8898605 also passes four cases, but no
full load-cost guide performance is established. Existing guides remain off.

After each full matrix, preserve failures and check all entry timings, actual
RSS, trajectory fingerprints, final-window rates, age and movement efficiency.
For a promising refresh setting, confirm against its same-build frozen control
and stable no-flow controls before broader seed validation. Require six seeds
before claiming the goal. Defaults remain unchanged until validated.

Fable's guide CLI review emitted findings but failed its configured cost budget;
it is not a completed signoff. Visible findings and independent assessments are
in fable-guides/. The confirmed waypoint defect is fixed. Route concentration,
retry backoff and salted guide ties remain hypotheses/future experiments.
