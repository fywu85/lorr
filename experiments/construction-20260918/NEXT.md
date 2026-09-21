# Continue CGAR across all ten competition instances

Updated 2026-09-21T05:21:45.750662+00:00. Actual user scope supersedes the obsolete formal Warehouse160k
goal: all10instances, NMS target, throughput primary, fairness secondary. Pursue
general improvements and explicit instance tricks together. Individual seeds
may set bests. Tricks require --trick INSTANCE and labelled commits/logs.

Ownership: never edit/stage/build/collect/interrupt random05/, RANDOM05_PROGRESS.md,
PILOT_PROGRESS.md, that agent's jobs or held-out streams50001..50008. Our CGAR
runs use runs/cgar-*. No internal subagents. Persistent Fable CLI session
1ebb1075-3538-49d1-93d1-a00c94fa256a is authorized; turn46 lacked credits,
so do not retry without an availability change.

All shell calls need require_escalated; Python3.7; use quoted Python edits.
Heavy builds/tests/raw analysis on GRID. Shared main/index: explicit owned paths,
git commit --only. Never remove locks, amend, force, or stage other work.
Commit/push authorized; public repository remains public.

## Current selected full strict1000ms records

WAREHOUSE155173; SORTATION150333; CITY01=8427; CITY02=16315; GAME23977;
R01=621; R02=1188; R03=1902; R04=1947; R05=2877. All below32decimalGB.
Warehouse/Sortation8physicalcores, others4. Warehouse has exclusive-host
qualification and independent action replay; other rows have simulator validation
and complete movement/waiting reconciliation. Published NMS is a historical target,
not a matched local comparison. All current selected profiles are TRICK.

Canonical selected-full-results.json contains exact configurations/source/binary/
seed/timestamps/evidence for all10. CGAR_PROGRESS.md is the concise overview,
BEST_HISTORY.md preserves records, TRICK_ROADMAP.md contains next ideas.

## Recent results

GAME uniform2750/3000/3250/3500 seed0=21742/23010/23977/22556. 23977 beats
published23274 by3.02%, max842.09ms, RSS10.556GB. Whole2750 control matches earlier.
Only3250robots get new tasks, all6500remain movable. Independentmask/auditpasses;
this deliberate fairness tradeoff is not starvation-free. Earlier2750 improves
all3seeds by37.75%mean versusfullfleet; finer3250 replication is running.

R04 KKturn6/2/4/8/12=1801/1947/1795/1741/1340. Turn2 vs6 onseed0/2/4:
1947/1904/1796 vs1801/1803/1935; two gains one loss, +1.95%mean.
Temperature1000/0/100/5000 atturn6=1801/1768/1636/1885. Atturn2,
temperature1000/0/5000/10000=1947/1793/1924/1828. Controlwholehashesmatch.
R03turn20/6/12/32=1902/1779/1774/1674. R01cadence1 loses617vs621.
R02cadence1budgets1/2/4/8=1183/1188/1188/1188. Keep selected profiles.
R05round4vs8 seed0/2/4:2806/2809/2718 ->2877/2865/2836, allpositive.

Geometric known-horizon source51aab58 tested1056independent BFS chains plus22400
realR04actions. R04control1743/bound1740/mean1755/p901693; R05controlFAILEDstep0,
bound2806/mean2806/p902819. Identicalcontrolretry2806 exactoldwholetrajectory,
max509.50ms. Keeporiginalfailure; causeunresolved. Crossmatrixaudit17matrices,
22overlapping-timehostpairs, no overlappingboundphysicalgroups. Not proofabout
unrelatedjobs/cache/OS; don't claim contention caused timeout.

## New general mechanism, code currently frozen for build

Optional CGAR_TEMPORAL_REGION_KEEP_PEAK=1, defaultOFF. Finish complete prescribed
repair work, then restore strictly better visited complete plan. Searchcontinues
fromcurrentannealingstate; no earlyreturns, timeoutalwaysfailure. Mapindependent.
Source90df94f0d1ca1e380acd1b4a9cf1a7cc851c4638 (after96b1037 implementation).
Build job8901628 runs/cgar-regional-peak-build-v2-20260921 completed allregressions.
All37source/testhashes match90df94f; binary92595db9fcfea8cbe89e5e65950578c8837128a97a5f0c63d7912c540120f0a5. Earlier buildv1 passed
fullsuite; v2corrects lost_improvements to compare returned state, preservinglegacy
behavior. Analytical64seedsrestore18knownpeaks; same300attemptsandcandidatework,
128disabledRNGcontinuations, timeoutafterpeakfails,4800parallelproductionactions,
regionmergesindependentlyreconstructed. Proofregional-search/build-v2/checks.json.

Frozenfullpeakprofiles regional-search/random-03/04/05-peak-v1.json. R04fourarms
turn2temperature1000/5000 xkeep0/1; R03andR05twoarms keep0/1 at1000. Auditenabledall.
Submitted withmatching source. Fourcoresstrict1s; R03full800,R04full1000,
R05full2000. Analyzer nowverifies keep-peak config/counters; do notassume higher
localpeakscoremeanshigherthroughput. Comparecontrolwholetrajectorybeforepromotion.

## Active matrices

| Suffix (runs/cgar-SUFFIX-20260921) | Matrix | Verifier |
|---|---|---|
| random03-regional-peak-full-v1 | 8901648 | 8901649 |
| random04-regional-peak-full-v1 | 8901650 | 8901651 |
| random05-regional-peak-full-v1 | 8901652 | 8901653 |
| sortation-dispatch-full-v1 | 8901586 | 8901587 |
| game-fleet-fine-full-v1 | 8901622 | 8901623 |
| game-fleet3250-seeds-full-v1 | 8901624 | 8901625 |
| random05-rounds-fine-full-v1 | 8901626 | 8901627 |

New GAME usesf644acc/build-attested, R05roundsfine usesaad422f. SORTATION uses51aab58.
Reports live experiments/allmaps-20260920/results/SUFFIX. Collect only our reports.

## Provenance and GRID recipes

GAME frozenbuild runs/cgar-game-fleet-build-v1-20260921, build-attested.json;
sourcef644accc7e00ceda7a455b7a45ed6eee4f970f8d, binary
0a83dc762511ea924c6433b634007bfd3e5227cb5a2ef9ea491f3794c6a0a30f.
Originalrequest raced otheragentindexlock; frozen36hashes match f644acc; preserve
originalreceipts andattestation. Never claim requestedcommitmatchedunmodified.

Finite retarget/frozen tuning build runs/cgar-rematch-budget-build-v1-20260921,
sourceaad422ff1160e1daae02eccf575209da1e4edd3a, binary
 dfec054298b7eff48c1634e70df377ae95542f08f5ddd07ec2990afcb4478997.
Geometricbuild runs/cgar-geometric-horizon-build-v1-20260921 source51aab58,
binary263610c6cb4b41794fa036263eca900b2aaa2c180878a60cc62c658cf3457400.

research44/57 queues countlogicalslots: --scheduler-slots-per-core2 mandatory
fornewmatrices/builds. Fourcorecase8slots*4GiB/slot; eightcorecase16slots*2GiB/slot.
Physicalbindingremainsactualcount; noquota. Verifiers2slots/bind1physical/6GiBslot.
Exactguardsstay. Fullrawanalysisrunsviaanalyze_matrix.py onGRID; originalfailed
analysisattempts preserved. Analyzer nowreadsdeclaredretargetbudgetandregional
parts*rounds (notobsolete1/8constants). Newselectorsverifiedviaindependentreceipts.

Checkpointexplicitownedpaths only: CGAR_PROGRESS.md, allmaps/, thisNEXT, changed
ownedanalysis/buildhelpers. Onlystagecompletedresultdirswithverification.json+
summary.md, full_horizonstrue. FailedcompletecasesretainedwithtasksNone. Do notstage
activepartialreports or otheragent files. `git diff --check` then commit --only.
Push with GIT_ASKPASS=/bin/false GIT_TERMINAL_PROMPT=0 and credentialhelper
!/user/fw2449/.local/bin/gh auth git-credential, noforce/amend.
