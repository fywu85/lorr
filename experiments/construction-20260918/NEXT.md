# Continuing warehouse work

Updated 2026-09-19 20:24 UTC. Active unbudgeted goal: meet/exceed local KittyKnight
152,981 tasks over 5,000 steps / 10,000 robots repeatably across six seeds. Every
complete schedule+plan entry <=1 second; explicit timeout failure; process RSS
<32,000,000,000 bytes; isolated GRID physical cores. Generic policies only: no map
identity/templates, supplied weights, hidden future tasks or map-specific fleet caps.
KittyKnight used 38.858 GB, so this is not an official equal-resource comparison.
Goal remains active and unmet. Work is progressing; no blocker.

## Confirmed reference and strongest observed results

The working reference remains graded strength4/scale4, margin25, refresh512,
turn-build128, 8GiB cache, HRRN1 / pickup weight1, pickup flow ON, 64 complete pickup
fields / four field threads, age-aware discovery key0, one planning worker4M and
four preparation threads. Explicit configuration: warehouse-reference-variants.json.
Exact V42 commit 4872d048dd8155927074ac2a4d27a55756266fd5, all22 source/test hashes
verified; frozen runs/cgar-pickup-full-build-v42-r1-20260919.

Seeds0..5: 141829 / 143325 / 142988 / 141802 / 141988 / 142917; mean142474.8333.
All six totals, final1,000-step windows and age improve over the preceding policy.
results/pickup-full-six-seed-v42.json; PICKUP_FULL.md. Reference resource cohort:
mean272.4–317.4ms, p99557.3–602.0ms, max874.138ms, CPU1.425–1.494 cores of4
reserved, peakRSS11.934GB, wall23.93–27.70min; all30,000 entries<=1s.
results/full64-reference-latency-six-seed-v42.json. CPU is whole-process
(user+system)/wall including startup, simulation and logging. User already answered.

Sixteen workers completed all six seeds: 143749 / 143241 / 143571 / 143643 /
141876 / 142587; mean143111.1667, +0.446629% versus reference and6.45% below KK.
Paired totals +1920 / -84 / +583 / +1841 / -112 / -330; tails +227 / -87 / +199 /
+280 / +47 / -101. Three total/two tail regressions: not promoted as working
reference. This is our highest observed six-seed mean, not an all-seed improvement.
All12 runs valid; all six one-worker controls exactly reproduce V42 full hashes;
all22 source/test hashes match exact V43 1661176ca3dd6854a7ba8d1a8a0ec8e33a2cffa9.
All16-worker cases reserve16 disjoint physical cores. Their mean326.1–392.0ms,
max954.119ms, CPU6.267–7.680 cores, peakRSS12.048GB. Quality comparison is not an
equal-resource runtime-speedup claim. results/pickup-full-workers16-six-seed-v43.json;
PICKUP_FULL_WORKER_SCALING.md. Jobs8898991/2 and8898993/4 are terminal complete.

Four workers: mean142742.1667, +0.187635%, five wins but seed5 loses3207 total,
1933 final-window tasks and agep90+53. Not promoted. Absolute best single run
143767, seed1, completed2026-09-19T19:05:34.390519+00:00, exact1661176.
Progress has40 increasing full-run records and seven confirmed reference milestones.
WAREHOUSE_PROGRESS.md records mixed evidence explicitly. User editor swap
.WAREHOUSE_PROGRESS.md.swp must stay excluded; do not delete. Preserve concurrent
markdown edits using read/compare-before-write.

## Current source checkpoint

Latest pushed commit47a0b12ad59c152db483289d24cde825af95e7d4 records worker scaling,
fractional/mixed negatives, reversal audit and Fable turn12 request. Public fywu85/lorr.
Exact V44 f6d0ae448504762136c4f5d85f25d7d207207965 is already pushed.

V45 is built and tested but not yet committed at this note's timestamp. It adds
only an optional const post-service reservation audit, defaultOFF. All22 source/test
hashes independently reconstruct from47a0b12 plus the248-line source.patch and match
the current workspace. Build8899006 terminal complete, every CGAR regression passes.
Frozen runs/cgar-service-audit-build-v45-20260919; binary SHA256
b3fbb35a2eec4968a5ea7bc5e8135c3d553597832eec14232b6a437661b3e18a.
Archive build-provenance/v45/, including independent-reconstruction.json. No build
active and no source edits since the V45 build. Next checkpoint should commit/push
V45 source/tests plus all verified reports/docs, then bind all22 hashes to that commit.

The audit observes the final complete reservation table, consumes no RNG, and changes
no cache/search/reservations. First selected current-goal hit is predicted service;
only later slots count as post-service. Eligible robots include selected-forward
robots. Candidate improvement removes operation ties and requires one full physical
heuristic unit, but need not improve the full native scalar score. Classify easiest
free / post-service-only / other conflicts; known-next / finishing / mixed owner tags.
Own reservations excluded; service-slot and fixed-owner conflicts are other. Both
cell and edge conflicts included. Gains overlap and are not removable-blocker or
recoverable-throughput bounds. Search action precedes the final seed-turn projection.

Thirteen hand-counted fixtures, unit boundaries, immutable subsequent RNG/search,
explicit interruption and4,800 protected serial/parallel production decisions pass,
along with all prior regressions. Screen8899012: audit0/67 both exact V44 200-step
hash0941646ba4985c0d4d18ad713e9563ac3edae7eac77aa50072c9fea59bcc0837, both valid,
max879.95ms, peakRSS5.193GB; sampled67/134 partitions conserve all counts.
POST_SERVICE_AUDIT.md; results/service-audit-screen-v45/equivalence.json.

## Live full experiments

- Regional repair8899002, held analysis8899003, started19:56:06UTC research46.
  Frozen V44 exactf6d0ae4. Six concurrent cases, seeds0/2 with disabled / one round /
  two rounds, four regions, four threads,25k fixed attempts per region per round.
  One complete4M-worker plan before repair; all confirmed64-field settings retained.
  Four disjoint physical cores/process,24 total,96GiB aggregate,32decimalGB cap.
  raw runs/cgar-pickup-full-regions-full-v44-20260919; future archive
  results/pickup-full-regions-full-v44/. Zero controls must exactly reproduce reference
  full hashes and141829/142988. Screen8899001 all3 valid, zero-control exact V44,
  max875.99/922.75/961.48ms, peak5.277GB; actual retained regional search verified.
  PICKUP_FULL_REGIONS.md; pickup-full-regions-variants.json.
- Post-service audit8899017, held analysis8899019, started20:07:54UTC research44.
  Frozen V45, only stride67, seeds0/2, two concurrent cases, four disjoint physical
  cores/process,8 total,32GiB aggregate,32decimalGB cap. raw
  runs/cgar-service-audit-full-v45-20260919; future results/service-audit-full-v45/.
  Each must exactly reproduce the reference full trajectory, tasks141829/142988.
  Seventy-four sampled frames per5,000 steps;67 is coprime to512. Check all robot/action
  partitions and report steady/full distributions with no additive counterfactual.
  service-audit-full-variants.json. No production continuation policy exists yet.

## Known-next-errand objective and review

Offline goal_holding_probe.cpp exhaustively minimizes production129-operation score
on two-robot small grids, with independent oriented distances. Fixed1500 draws yield
987 valid inputs and two3x7 examples where native objective prefers holding the
current goal while a collision-free witness continues a visible next errand and lets
the other robot first-forward. Native/witness cost -1040/-912 and -840/-564.
GOAL_HOLDING.md; results/goal-holding-probe-v1.json. Not a production search episode,
warehouse prevalence estimate or throughput gain. Fable's separate3x9 first-rotation
claim does not reproduce under exact native minimization; local3x7 examples do.

Before implementing continuation: use only revealed current-task errands, advance
at most one errand per physical tick, charge every actual turn and forward surcharge,
and keep a comparable objective across current-goal arrival headings. Fable's
candidate-dependent next-leg baseline could reward a bad approach; investigate with
an independent oracle. Post-service holds are not automatically removable. Existing
full audit measures a related opportunity, not a bound on a different policy.
Task_chain_cost still uses spatial known-leg costs; cached oriented chain pricing is
another unimplemented hypothesis. TurnDistanceOracle::peek exists and does not touch
LRU. Any such change needs common units and no extra table builds or hidden future.

Persistent Fable session1ebb1075-3538-49d1-93d1-a00c94fa256a, name
lorr-warehouse-cgar-fable51, viaClaudeCodeCLI, explicit source-sharing authorization.
Turns01–11 successful. Turn12 terminal FAILED19:47:26UTC, error_max_budget_usd,
configured12USD, CLI reportedtotal59.438321,387.751s, no permissions denied. Visible
findings, metadata/status and qualified assessment archived fable-flow-session/turn12/;
no successful review.md. Reported cost may be cumulative for the resumed session.
No live CLI process. Do not restart turn12. Source-cache advanced toV44 despite
failure; next request is turn13 / followup-12.md and should refresh source-spec for
V45 audit and shifted lines. Do not commit raw protocol or hidden reasoning.
Runner fable-flow-session/run_review.py. Findings prompted regional interaction and
known-next audit, not a reproduced implementation bug. User encourages more Fable.

## Established negative evidence

- V44 fractional turn extras1/2 both seeds lose; means141773/141931 vsone-worker
  142408.5. Extra2 cuts loaded turns9.04/9.54% but raises waits12.86/10.93%.
  Four-worker warm/mixed means142381.5/142770.5 versusfour-cold143374.5, both seeds
  lose. All12 runs valid, all4 cold controls exact V43, all22 hashes exactf6d0ae4.
  Keep surcharge0, warmOFF/mixedOFF. FRACTIONAL_MIXED.md and full comparison.
- Flow strength2/6/8: mixed +.0604% / -.308% / -1.052%; retain4. No six-seed extension.
- Pickup discovery key1: fields32 loses2.248% versus healthy zero-field; fields64
  seed2 collapses79601. Retain original age-aware key0. PICKUP_COST_KEY.md.
- Quota32 seed0 collapse70043 reproduced; quota response nonmonotone. Quota16 wins
  all six versus0 but64 beats16 every full total. Higher128/256 not tested.
- 8/16GiB cache has exact full trajectories despite fewer rebuilds. Keep8GiB.
- Graded margin0 collapses; sampled publication churn did not predict collapse.
- Orientation table batch128 mixed six seeds;512 fails despite100% sampled coverage.
  These are distinct from complete pickup-field quota.
- Pool and transaction experiments mixed six seeds; not promoted. Old no-flow
  distance-coefficient/turn-weight failures do not prove failure under current policy.
- Opposite-turn and loaded action audits conserve all50M robot-steps and match logs.
  Fewer reversals associate with some wins, but this is not causal proof. Native
  planner selects >99.5% of loaded turns; final tied-wait turns only54–83 per full run.
  TURN_REVERSALS.md; LOADED_MOTION.md. Completed-cohort decomposition is censored
  and not an additive cross-policy savings estimate.

## Operating rules

Every configured search completes or the entry/run fails; no clock-selected partial
solution. Full5,000-step horizon for quality;200 steps only timing/equivalence screens.
Before interpretation verify all timings, validation, RSS, exact source/binary hashes,
disjoint physical bindings, no quota and full control trajectory hashes. Compare
full totals, final1,000, outstanding ages and motion. Heavy replay belongs onGRID.
Per-process32decimalGB cap is distinct from aggregate job memory reservation.
All exec_command calls require sandbox_permissions=require_escalated; local bwrap is
broken. No proactive subagents. Avoid tools/waits blocking commentary for>60 seconds.
Commit/push authorized; exclude binaries, environments, raw trajectories/protocol.
Do not change deliberate public repository visibility. Keep goal active/unmet.
