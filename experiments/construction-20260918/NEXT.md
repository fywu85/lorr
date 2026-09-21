# Continue CGAR across all ten competition instances

Updated 2026-09-21T03:39:47.408727+00:00. Actual user scope supersedes
obsolete formal Warehouse160k objective: all ten instances, NMS target,
throughput primary, fairness secondary. Pursue general improvements and explicit
instance tricks together. Every trick requires `--trick INSTANCE`; use labelled
commits/logs. Individual seeds may set best records, without robustness claims.

## Ownership and execution

Do not edit, stage, build, collect or interrupt `random05/`, `RANDOM05_PROGRESS.md`,
that agent's jobs, or held-out streams50001..50008. CGAR runs on RANDOM05 live in
`runs/cgar-*`. No internal subagents. Fable CLI persistent session
1ebb1075-3538-49d1-93d1-a00c94fa256a is authorized, but turn46 ran out of credits;
no quota change and no retry. Next turn47 only if availability changes.

Shell tools need require_escalated. Use quoted Python edits; apply_patch fails.
Python3.7. Heavy builds, tests and analysis on GRID. Shared main/index: commit
only explicit owned paths using `git commit --only`. Never remove locks, amend,
force, or stage other-agent edits. Commit/push authorized. Keep public repo public.

Use complete fixed work or explicit timeout, never partial-quality success.
Per-process RSS below32decimalGB. Shared hosts allowed, exact bound physical
cores and no quota still required. GRID sometimes ignores binding when allocations
overlap/exhaust a host: preserve rejection and retry unchanged on fresh allocation.
Research43/44/57 EPYC9354 preferred. Do not weaken resource guards.

## Current results and evidence

Canonical CGAR_PROGRESS.md, allmaps/BEST_HISTORY.md and selected-full-results.json.
Published targets in TARGETS.md are not matched local NMS comparisons. Only
Warehouse has full independent action replay; later results have simulator
validation plus complete movement/waiting accounting and source/binary proof.

| Instance | Selected full-run tasks | Qualification |
|---|---:|---|
| WAREHOUSE |155173|TRICK, strict1s,8cores, max945/958ms, RSS15.4GB; preserve |
| SORTATION |150353|TRICK,5s development,max1152ms; strict full work underway |
| CITY01 |8386|TRICK,pickup8,seed6,max755ms; seed0/2/4=8378/8367/8374 |
| CITY02 |16159|TRICK,pickup12,seed0,max826ms |
| GAME |14664|TRICK,adapted field+squared chain ranks,seed0,max897ms |
| RANDOM01 |621|TRICK,NMS arrows+pickup4,seed0,max202ms; generic613 |
| RANDOM02 |1160|TRICK,NMS arrows+pickup4,seed0,max186ms; generic1084 |
| RANDOM03 |1890|TRICK,KK forward+chain ranks+pickup4,seed0,max198ms; generic1613 |
| RANDOM04 |1645|TRICK,squared ranks,lanesOFF,seed2; generic1503 |
| RANDOM05 |2608|TRICK,current field+pickup4/match64,seed2 |

All except Sortation strict1000ms; all later runs4physical cores unless specified.
Full600/600/800/1000/2000 RANDOM horizons,3000 CITY and5000 GAME/SORTATION.
Bests are different declared configurations, not one universal preset.

## Current code and frozen builds

CGAR code clean and tested at acaf634b537ff655715aae28ed43056e0c62a1de (other-agent
commits may be later HEAD). New RANDOM reference providers live in
cgar/tricks/random_reference.hpp. Provider1: unchanged NMS arrows20/60,turn20.
Provider2: KK forward fields forR02/03/04; R03/04 have variable stay costs in KK,
so current uniform-turn transfer is explicitly a forward-only adaptation.
Provider0 retains standalone-derived dense field. Exact map/fleet and CLI gates.
All build checks in random-reference/build-v2; failed v1 retained (turn-bound
parser/init mismatch fixed by acaf634). Native reference turn1..64, legacy1..16.

* reference buildv2: runs/cgar-random-reference-build-v2-20260921; sourceacaf634;
  binary17b4f7054cec52f665abad5abb04291369983281ece30a479910e10f0f3835f2.
* CITY dispatch build: runs/cgar-promise-guard-build-v1-20260921; source85c3f0f;
  binary726eedf767129d409fad5d869e124368b6048fa74adbb5b5ae37fc4939d600f7.
* GAME rank build: runs/cgar-squared-rank-build-v1-20260921; sourcea573aa5;
  binary4d4c12250d0adf1f2db42a67b943540321d12583936f0444e3f6ffb59d2b99b5.
* SORTATION build: runs/cgar-sortation-native-build-v1-20260920; sourcedf94a520;
  binarya23a3c81195d71a7a13c9ee518d9896c0a1e130d985037080a18871ad74b606c.

Squared rank is CGAR_TRICK_RANK_SQUARED, explicitGAME/R04/R05 only, unequal
weights required. Applied consistently to complete global/regional/branch work.
Disabled controls are whole-trajectory identical. All regression proofs retained.

## Newly completed full comparisons

See sparse-search/full-comparison.json and field-interaction-selected.json.
General control/chain/portfolio8/both R01=611/607/613/603,
R02=1084/1080/1083/1075, R03=1484/1613/1439/1543. Same settings on all three;
no universal winner. Combine chain ranks with KK onR03 gives1841, pickup4=1890;
NMS+chain1716, genericchaincontrol1613 identical even under lane-off CLI gate.
NMS-field pickup4 yieldsR01=621,R02=1160; pickup8 loses to4. Controls identical.

Dense reference fields lose: R04 squared lanes-off1622 vsuniform1307,NMS1506,
KK1569; R05current2574 vsuniform1980,NMS2337,NMS+squared2377. Whole controls
identical. Keep selected1645/2608. All15R01..04 and4R05 cases valid strict1s.
R05 first allocation failed, retryv2 passed; binding failure proof retained.

R04 linear1503/1480/1494 vs squared1622/1645/1512: +6.75%mean, all3positive.
R05 linear2574/2608/2528 vs squared2596/2571/2583: +0.52%mean, mixed. Keep2608.

CITY01 pickup4=8213,pickup8=8378, seed6=8386. CITY02 pickup4=15797,
pickup8=16123,pickup12=16159. Whole controls identical. CITY current declarations
use supported limits: pickupweight<=16, matchinggroups<=64. CITY02 next factor
uses pickup-neighborhood grouping, not invalid256groups. No such invalid profiles
were submitted. CITY02 extensionv1 binding rejected before simulation; v2retry.

GAME equal10080, linear-current13314,squared-current14471,squared-chain14664.
Earlier dispatch branch11146 is weaker; combining it with squared ranks pending.

## Active matrices (check completion before repeating)

| Raw suffix (runs/cgar-SUFFIX-20260921) | Matrix | Analyzer | Last state |
|---|---|---|---|
| sortation-global-strict-full-v1 | 8901365 | 8901366 | pending |
| game-squared-interactions-v1 | 8901408 | 8901409 | pending |
| city-01-pickup-extension-v1 | 8901447 | 8901448 | pending |
| city-02-pickup-extension-v2 | 8901454 | 8901455 | pending |
| random03-fixed-work-full-v1 | 8901459 | 8901460 | pending |
| random04-fixed-work-full-v1 | 8901461 | 8901462 | pending |
| random05-fixed-work-full-v1 | 8901463 | 8901464 | pending |
| random03-chain-fields-seeds-v1 | 8901465 | 8901466 | pending |

SORTATION two full5000step variants global2M/1M on8cores (prep/pickup8), unchanged
regional4M. Original150353 uses4M and was not1s; no identity claim. Screening
200steps max967/877ms for2M/1M. Full strict result decides throughput/runtime.
GAME adapted/native × dispatch (four profiles), all squared chain, full5000steps.
CITY01 pickup8 vs10/12/16. CITY02 pickup12 vs16,16+short,12+pickup grouping.
R03 scaling4Mcontrol vs8M/16M/four4M proposals. R04/05 eight500k control vs
2M/4M each, or regional8M. Four cores, all fixed work, strict1s.
Separate R03 generic1613 vsKK/pickup4=1890 bundle on plannerseeds2/4.

## Next structural work being inspected, not yet implemented

KK GAME activeLNS/LaCAM2 uses `tabu_locs` and nominal2750 active robots. It excludes
robots starting in its tabu asset from the disabling pool, then shuffles others
and disables min(N-2750,pool size). Thus it can leave more than2750active. Read
kk/wppl_planner/src/LaCAM2/LaCAM2Solver.cpp and kk/data/map_weights/brc202d_tabu_locs.txt.
Potential CGAR adaptation: explicit GAME-only optional task admission mask, selected
once; idle disabled robots remain movable by CGAR, no held/started task dropped,
primary/recovery unaffected. Needs separate uniform/tabu ablation and meaningful
guard, ownership, full-action validity and disabled-identity tests. Not coded yet.

General full-chain oriented DP and coherent joint motion prefixes remain ideas.
The narrow after-turn promise prototype lost all relevant paired comparisons;
its negative evidence is in random-transfer/MOVEMENT_PROMISE_DESIGN.md. Native
neutral-tail scoring also lost. Do not repeat these as new hypotheses.
R05 capacity restrictions exclude427 of431 never-assigned tasks in the2036control;
disabling certificates lost. Throughput primary, but no starvation-free claim.

## Checkpoint actions

Commit/push only completed own report directories and configs. Do not stage live
partial results or other-agent work. Source snapshots/reports are authorized;
binaries and raw runs stay ignored. Generic and explicit trick bests both retained.
