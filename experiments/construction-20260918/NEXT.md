# Continue CGAR; user asleep, active overnight work

Updated 2026-09-21T09:08:18.069431+00:00. Latest user objectives supersede the obsolete formal Warehouse160k
goal: +10% over max(publishedNMS,KK) on RANDOM01-05, stretch+5%on otherfive. Throughput
primary; fairnesssecondary; selected-seed maximaqualify. Continue until achieved;
do not pause/complete the active goal because an old target has been matched.

## Scope and execution

Never edit/stage/build/collect/interrupt random05/, RANDOM05_PROGRESS.md,
PILOT_PROGRESS.md or its jobs/builds/heldout streams. Immutable read-only references
and transfers into CGAR are authorized. No internal subagents. Claude/Fable CLI
persistent session1ebb1075-3538-49d1-93d1-a00c94fa256a authorized; lastcallturn46failed
forcredits, noavailabilitychange. All shelltoolsrequire require_escalated because
bwrapfails; Python3.7; editsviaquotedPython. Heavybuild/test/rawanalysis on GRID.

research44/57:64physical/128logical. scheduler-slots-per-core2;4physical/case means
8slots x4GiB;8physical means16slots x2GiB. Disjointphysicalbinding/noquota,32decimalGB
processcap, strict1000ms wholeentry. Sharedhosts authorized. Timeoutisfailure;
neverpartialsuccess. Analyzeraccepts4/8cores. No production/tests edits duringbuild.
Sharedmain/index: explicitownedpaths and git commit --only; noamend/force/lockdelete.
Repo deliberatelypublic. Commit/pushauthorized using credentialhelper gh asbelow.

## Selected frontier

{'WAREHOUSE': 155173, 'SORTATION': 150894, 'CITY-01': 8427, 'CITY-02': 16315, 'GAME': 24447, 'RANDOM-01': 668, 'RANDOM-02': 1215, 'RANDOM-03': 1939, 'RANDOM-04': 2023, 'RANDOM-05': 3065}

Targets:162535/160350/8868/17847/24438/757/1386/2568/2802/3355. Only GAME meetsnew
margin. AllselectedrowsTRICK, fullhorizons,strict1s/32GB. Warehouse/Sortation8cores,
others4. Warehouseexclusive+100Mactionreplay; otherssimulator+complete movement/wait
reconciliation. No starvation-freeclaim. Canonical allmaps/selected-full-results.json,
append-onlyBEST_HISTORY, CGAR_PROGRESS andTARGETS. LatestprooflinkedfromCGAR_PROGRESS.

Recent replication: R01window658/662/648 versus645/647/595 (+4.29%, allpositive).
R02window1180/1215/1202 vs1188/1189/1197 (+0.64%, oneloss). R03historyoff1939/1900/1870
vs1902/1798/1848 (+2.90%, allpositive), max633ms. Seed2/4controlsfirstmatchedruns,
notclaimedpriorrepeats. R03keep0/keep6wholetraces identical. R01/R02shortkeephistory
neverretained; keep0wholetrace identical; longkeep19loses onallthree.

NewR01uniformfullwork668seed2 versus662identicalcontrol; matcheduniform4096work
wasnotpreviouslytested (olduniform2048=656). turn40losesbothfields. Seeds0/4repending.
R02turn40andwait20lose. PILOTfieldonR01/02/03loses. Lanecontrast12/8/4losesCITY01/02
andGAME, withGAMEcollapsingat4/8. Keepqualifiedoriginalfields.
R05chainmode1+knownhorizonmean selectedseed10=3065; priorchainreplicated+3.59%, horizon
marginadds0.19%aggregatewithoneloss. Seedscan2988/3019/3065/2913,notalgorithmgain.
Sortationruntimealternative-v3=150865,max894.99ms; frontier150894unchanged.

## Qualified builds

339718b3bc0364afb1f780033304b5f88d6d3ce9 windowv4:
runs/cgar-rolling-window-build-v4-20260921, binary88e3e965e2da7d7102c9579803ed6612fc8e7bb4870985be072c146eac3758f7.
cd145411374c2009c98c82e5050ab7c2cc7676a1 fieldoptionsv1:
runs/cgar-field-options-build-v1-20260921, binary52154597249bf6b77d2d9ee090da8469cb74ff4f330608425637e0dd79e3ab6d.
965756fdbf5111441d2f40bf97d3b604f8a85a0b commonfuturesv1:
runs/cgar-common-futures-build-v1-20260921, binaryb7bb6ca80f310f1d099efc1405bb09a2b8af386a93b551b1e6425f0bfe42ac87.
Allhashes/fullregressionqualified; commonfutures40source/testfiles,7200serialparallel
actions, one-rootwholetrajectoryidentity, primary/pocket/capacity andlatetimeoutfailure.
No production edits pending at this checkpoint.

Commonfuturesheader +adapter: roots0disabled; H15/B4/T4/noise50default. Fixedcomplete
existingCGARrootprefix5+commonjointTemporalPibtchunks. Root0currentregional/transaction
incumbent,othersglobalranked. Onlyoriginalrootfirstactionexecutes. Pinnedfullpaths
frozen; nofuturetasks. Sameorders/seedsacrossroots, separateRNG. Paid+remainingchain
aggregate; incumbentwins ties. Cannotcomposewindow/guide/neutral/next/historypromise.
Cancomposechainmode andprioritypersistence (rememberselectedoriginworker).
R04seed0:1999control,1928r4H10,1890r8H10,**2023r4H15**, max268.97ms. Only1239of700k
firstactionschange. Newrecordpromoted; replication/depth/chaincompositionrunning.

Possible next general experiment: retained window prefix currently ends inwaits,
competes againstfresh20stepjointprojection; it mayrejectusefulhistory. Addoptional
completejointtailrefreshafterkeptprefix, separateRNG, defaultoff. Validateprotected
paths,firstactions,taskstages,serialparallel,wholework+latefailure. Notimplementedyet.

## Active jobs

- random03-window-horizons-full-v8: 8902385/8902386 sourcecd145411
- random05-common-futures-full-v1: 8902391/8902392 source965756fd
- random01-window-uniform-seeds-full-v1: 8902398/8902399 sourcecd145411
- city01-rematch-transfer-full-v2: 8902400/8902401 sourcecd145411
- random04-common-futures-seeds-full-v1: 8902403/8902404 source965756fd
- random04-common-futures-depth-full-v2: 8902405/8902407 source965756fd

ACTIVE_JOBS isauthoritative; inspect completedverificationreports before removing.
NFSnegativecachedelays: listingparent thenretry; donotrerunsolverformissingcachedpath.

Helpers: /tmp/cgar_submit_profile.py --instance INSTANCE --seeds S... --profile JSON
--suffix SUFFIX --build runs/... --source FULLHASH [--inputs MANIFEST] [--cores8].
Defaultsoldwindowv4/source339718b3. CITYinputsusecity-game/inputs-CITY-01.json,etc.
Itqueuesmatrix+verifierandupdatesACTIVE_JOBS sequentially. No mixed concurrentwriters.
/tmp/cgar_refresh_frontier.py --report SUFFIX (repeatable) --note 'honestnote' promotes
onlygreatercompletevalidstrict1sscores andupdatesall10proof/tables/history. Itdoesnot
updateprose/exactbestprofilesand removesALLfinishedverifiersfromACTIVE_JOBS, soinspect
unprocessedcompletedreports too. Sourceprofiles/builds mustqualifybefore submission.

CheckpointcommitexplicitownCGAR_PROGRESS, NEXT, allmaps files; includeonlyresultdirs
withverification.json. Avoidforeignfiles. Push:
GIT_ASKPASS=/bin/false GIT_TERMINAL_PROMPT=0 git -c 'credential.helper=!/user/fw2449/.local/bin/gh auth git-credential' push origin main
