# Continue CGAR across ten instances

Updated 2026-09-21T12:23:19.646725+00:00. User asleep and explicitly requests continued iteration. Targets:
+10% over max(published NMS,KK) on RANDOM01–05; stretch +5% on other five.
Throughput primary, fairness secondary. Selected-seed maxima count; distinguish
replicated improvements. The active formal Warehouse160k goal text is obsolete;
do not stop while the current all-map scope remains. No internal subagents.

## Ownership and authorization

NEVER edit/stage/build/collect/interrupt random05/, RANDOM05_PROGRESS.md,
PILOT_PROGRESS.md or independent jobs/builds/held-out streams. Immutable git
source references and algorithmic transfers into CGAR are authorized. Fable CLI
persistent session1ebb1075-3538-49d1-93d1-a00c94fa256a, Fable5.1max, authorized.
Turn47 failed credits at2026-09-21T10:31:58Z; no findings. Do not retry absent an
availability change. Receipt allmaps/fable-followup/turn47. Public repo deliberate;
owned commits and normal push authorized. Shared main/index, explicit paths and
commit --only; no amend, force, lock deletion or staging another agent's changes.

## Execution

All shell calls require_escalated (default bwrap fails), Python3.7. Heavy builds,
regressions and raw analysis on GRID. research44/57 use slots-per-core2:
4physical=8slotsx4GiB;8physical=16slotsx2GiB. Disjoint physical binding/no quota,
32decimalGB process cap, strict1000ms complete entry. Shared hosts authorized;
do not claim exclusivity. Timeout is failure, never partial best. No source/test
edits during a live build; frozen binaries permit later independent edits.

## Current frontier and evidence

WAREHOUSE 155173 (seed0); SORTATION 150894 (seed0); CITY-01 8440 (seed2); CITY-02 16315 (seed0); GAME 24447 (seed4); RANDOM-01 693 (seed4); RANDOM-02 1215 (seed2); RANDOM-03 1982 (seed2); RANDOM-04 2298 (seed2); RANDOM-05 3571 (seed10)

All are full horizon TRICK profiles, strict1s/32decimalGB; WH/Sortation8physical,
others4. GAME and RANDOM05 meet selected-seed margin targets. Competitor scores
are historical published targets, not matched reruns. No starvation-free claim.
R05 parks31capacityrobots; GAME uses fleet/rank restrictions. WH has exclusive
qualification and100Mactionreplay; other runs simulator and full movement audits.
Canonical selected-full-results.json, BEST_HISTORY.md, active-targets.json,
CGAR_PROGRESS.md. Latest all10 proof selected-results-checks-20260921-122152.json.
Exact profiles: history-rollout/best-random01.json, rolling-window/best-random02,
window-delay/best-random03, move-promises/best-random04 and best-random05.

Movement commitments: R04 controls2059/1901/1843, mode2 2237/2298/2215 on seeds0/2/4,
+16.319%aggregate, all positive; forward-only+11.529%. Selected2298seed2. R05seed10
3065control→3391mode1→3571mode2, max712.321ms; 17.082%above published3050. No new
cap in this best; old guidance/horizon/parking tricks remain. R05replication pending.
General option CGAR_TEMPORAL_MOVE_PROMISES0/1/2 carries only next occupied cell,
exact pose/task/goal guarded. Protected CGAR actions override; collision closure
resets dependent components; replans tail; stationary promise leaves heading free.
This remains CGAR temporal construction/repair, not a replacement PILOT pipeline.

## Finished follow-ups

R04H20/no-futures seeds0/2/4:2237/2298/2215 vs2239/2219/2270. No-futures loses22
aggregate tasks; selected H20 stays. With mode2/no-futures, terminal chain2231 and
paidchain2044 lose to2239control. Sparse window composition all loses:
R01 693/687/688;R02 1215/1197/1172;R03 1982/1848/1942 (control/mode1/mode2).
All full runs valid and available control trajectories match exactly.
Cap700 alone replicates+1.129%aggregate on seeds0/2/10 (3033/3063/3130 versus
3043/3015/3065), one loss. Nearby625/650/675 yield3042/3084/3106 below3130control.
Cap-only results superseded by3571; cap+mode2 composition remains pending.
Earlier exact chain assignment, paidchain, sparse action-price changes, wider
matching on RANDOM/CITY, CITY horizon, regional futures all lost; don't repeat
unchanged arms. Detailed negative logs in feature subdirectories.
Earlier replicated sparse gains: R01uniform+2.69%, refreshedkeep10+1.88%, each3seeds;
R03delay3+3.293%allthree. Sparse current turnshare24–41%; CITY FW92–94%. Movement
bottlenecks/README is PRE-newR04/05 commitments; no causal ceiling claim.

## Builds

No active build; no source edits pending. Latest qualified44986ddf54fc381f478ba5116b9660389c3fb693
(window-move-promises), rawruns/cgar-window-move-promises-build-v1-20260921,
binary7b37798ab513ec9cfbb66ad1f312854174f705d4facc0cfa98bc5efa03a0c1cf.
Complete40source/testhash+binary/fullregression qualification inwindow-move-promises/build-v1.
26400productionactions,3307services,8554retainedFW,4743wait,550collisionresets.
Window first_cells constrain boundedA*/validation/retainedhistory, first heading
remains free for stationary cell. Actual selectedwindow t2 supplies next promise.
Explicit CGAR_WINDOW_MOVE_PROMISES1 required. Default window/commitment combination
still rejected. Commonfutures/window mutuallyexclusive. Source guard TODO: include
new first_cells in common_futures.hpp root-problem equality (production always empty).

Prior move-promises e66daa2f065e2b9589f1e2c972c3437f2e636f2d,
rawruns/cgar-move-promises-build-v1-20260921,
binarya7c3a6d7b8419cd833ef75347d608803d055737b6a83beda26884ee014410fd4,
19200serial/parallelactions and complete safety/identity/regression qualification.
Older qualified source311d04800c369f227407b63641c1c4f0a7102186 chain-paid build is
used by pending WH/Sortation. All build identities inACTIVE_- warehouse-matching-width-full-v1: 8902771/8902772, 8physical/case, 311d0480
- sortation-matching-width-full-v1: 8902773/8902774, 8physical/case, 311d0480
- random05-move-promises-replicate-full-v2: 8902815/8902816, 4physical/case, e66daa2f
- random05-move-cap-composition-full-v3: 8902817/8902818, 4physical/case, e66daa2f and featureproofs.

## Pending matrices

- warehouse-matching-width-full-v1: 8902771/8902772, 8physical/case, 311d0480
- sortation-matching-width-full-v1: 8902773/8902774, 8physical/case, 311d0480
- random05-move-promises-replicate-full-v2: 8902815/8902816, 4physical/case, e66daa2f
- random05-move-cap-composition-full-v3: 8902817/8902818, 4physical/case, e66daa2f

## Next bounded work candidates

1. Stronger adapted CITY lane opposing prices24/32/48 (currentlyonly4/8/12/16;
all lowerprices lost). Requires strict explicittrick gate, new immutablefieldhashes,
cgar.cpp guidance_cost_limit_ increase onlyforchosenprice, tests/analyzer identity.
Defaultunchanged; full CITY01seed2/CITY02seed0 matched arms after qualification.
2. PILOT-style terminal5x5neighbor-pair penalty in completecommonfutureobjective,
generaldefault0, O(cells+12n)counter, nottimedpartialsearch. Nativeforward20:
trialweights8/16/32 on R04currentmode2H20. Needs independentpair/selector tests,
strictconfig/audit and defaultidentity. Neither candidate implemented yet.
PILOT immutable engine.cpp/hpp copies /tmp/cgar-pilot-readonly-d51, source
 d51bdbf2c4e6830f8cabacb52ac9cfa8234aa663; provenancepilot-transfer/source-review-20260921-1112.json.
Never inspect independent current run output.

## Helpers and checkpointing

/tmp/cgar_qualify_build.py --raw runs/... --proof experiments/allmaps-20260920/FEATURE/build-v1
verifies40hashes,commit,binary,fullregression,physicalbinding/noquota.
/tmp/cgar_submit_profile.py --instance INSTANCE --seeds ... --profile JSON --suffix SUFFIX
--build runs/... --source FULLHASH --local-log-spool [--inputs MANIFEST] [--cores 8]
Always override old default339718b3 build/source. CITY manifests city-game/inputs-CITY-01.json,etc.
WH manifestwarehouse-sortation-matching/inputs-WAREHOUSE.json. SequentialACTIVE_- warehouse-matching-width-full-v1: 8902771/8902772, 8physical/case, 311d0480
- sortation-matching-width-full-v1: 8902773/8902774, 8physical/case, 311d0480
- random05-move-promises-replicate-full-v2: 8902815/8902816, 4physical/case, e66daa2f
- random05-move-cap-composition-full-v3: 8902817/8902818, 4physical/case, e66daa2fwriters.
/tmp/cgar_refresh_frontier.py --report SUFFIX repeatable --note ... updatesfrontier/proof;
clears ONLY requested completedreports (fixed this continuation). Does NOT update
bestprofilefiles/narrative. Inspect allresults beforecalling.
/tmp/cgar_commit_checkpoint.py ownsCGAR_PROGRESS,NEXT,Fablehelper,allmaps/**only;
resultdirs requireverification; ignoredrawlogs excluded. Updatecommitmessagewhenneeded.
Poll exec_command's sessionID untilcommitcomplete BEFORE dependentpush/build.
Push: GIT_ASKPASS=/bin/false GIT_TERMINAL_PROMPT=0 git -c
'credential.helper=!/user/fw2449/.local/bin/gh auth git-credential' push origin main
NFSnegativecache:listparent/retry, don'trerun solver merelyforvisibility.
Frozen tools/benchmark_cgar.py SHA
f5049ddd0f2ac55f01f2493076f9538bca1acaa817beda5aff8a6a0c890234bc;
DO NOT edit while queued matrices awaitdispatch. Localspool archives fullfailedlogs,
strictentry stillincludeslogging. Earlier2632ms timeoutretained,causeunproven.
