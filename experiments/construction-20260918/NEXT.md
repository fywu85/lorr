# Continue CGAR across the competition

Updated2026-09-20 23:15UTC. The latest user redirected work: finish Warehouse's
strict1s validation, then target all ten instances. Throughput is primary;
fairness is secondary and regressions must be reported but do not veto a faster
candidate. NMS is the primary target; KK supplies ideas/comparisons. The160000
Warehouse aspiration is deferred. Formal goal metadata may still contain that
superseded target: the latest user scope takes precedence; do not mark it achieved.

The other agent owns RANDOM-05. Do not touch its code, files, outputs, builds or
jobs. Read-only RANDOM05_PROGRESS.md and artifacts are allowed. No internal
subagents. Fable Claude session1ebb1075-3538-49d1-93d1-a00c94fa256a exhausted
credits at turn46; no change, so no retry. Next would be47. Shell calls require
require_escalated because bwrap fails. Shared main/index: explicit paths and
`git commit --only`, never remove index locks or include random05changes.

## Warehouse strict run in progress

V123 benchmark8900817 on exclusive research44,16physicalcores, two seeds0/2
with8cores each. Raw runs/cgar-strict-runtime-full-v123-20260920.
Full5000steps,10000robots,1000ms complete-entry timeout,32decimalGB RSS.
Frozen source3e319f175859396b91f97364b320c6794a85cbb7; binary
e3ec32ecdfdaf06f7db139da3857af8fa0ad43b98bd67479f7b9bd969dc1302c.
Profile strict_runtime/full-variants.json: old155173guardON best, with8thread
prefetch/preparation/pickup and25GiB logical unwarmed cache. No all-goal prewarm;
search work/decisions must exactly reproduce full V110155173/155090 trajectories.
Start22:46:35UTC; both beyond3600at23:14; no timeout so far. Expectfinish~23:23.
Verifier8900819, independent outcomes8900820 are held behind benchmark/verifier.
Results strict-runtime-full-v123 and strict-runtime-full-v123-outcomes.
Do not claim strict certification until both finish and all verification passes.

V122800step execution screen is verified after r2counter-check correction.
Allfour exact trajectories match; maxentries base.986230,prefetch8.960702,
prep/pickup8.984437,combined8+25GiBcache.953731. Shared timing not isolatedspeedup.
First verifier failure retained; only cache builds differ (intended fewerrebuilds).
Results strict-runtime-screen-v122 and -r2. Readytoarchive; helpersuncommitted.

Current record155173 source37f592a48bdb24b936f8d0be8be22ddd405b4ddb;
full hashb57ad3d8433ee9610c7af25214fe1d93d947aed9ec73211d4ed8d8cac9ae0ce0.
Seed2hash274d18be1db30d232c3fb50997ea281b5b41b6c60f4e5ec0fcf2f6c09506141b.
Fourguardtotals[155173,155056,155090,155008], mean155081.75; generic6seedmean144392.17.
Keep the original record's source/timestamp; add exact strict execution proof as
separate confirmation, not a fictitious throughput improvement. native_work/
summarize.py now uses recordeddeadline/exclusivity rather than hardcoded5s.
After outcomes complete, run work_budget.py then summarize.py. Update Warehouse
progress, rootREADME and cross-map status with actual timing/memory/CPU evidence.

## New generic regional candidate budget

Source eb9938090d7cfb331fdc124ce19b475626356502; binary
19155f20da9f518cfac5955c4bcc4a3af9926ff4828e552f3bd69026201b5c3f.
Build runs/cgar-allmaps-region-budget-build-v1-r2-20260920 complete, all28hashes
and fullregressions passed. Evidence experiments/allmaps-20260920/build-v1-r2.
New optionalCGAR_TEMPORAL_REGION_CANDIDATE_LIMIT default0; perregion/perround,
checked between complete attempts so final attempt canovershoot. It does not
return partial work on a deadline. Global4Mlimit already existed; regionshad
onlyattemptlimits. PriorRANDOM04step1000had1.187billioncandidates,4.122sregional.
Zero retains Warehouse/search defaults. Regressionfixturescovercollision-free
merges, primary/recoveryprotection, matching1/4threaddecisions, nonbindingcap
equivalence, measuredworkreduction, invalidsettings, timeoutfailure.
Initialbuild8900850failedan oldauditassertion assumingallattempts; correctedto
actualattemptcounts, retainingstrictoldassertionwhenuncapped. Failurearchived;
no benchmarksfromfailedattempt. Retry8900851passed.

## Full eight-instance matrix running

Job8900861 started23:09:47UTC on research57, sharedhost,32physicalcores,
8simultaneouscases4coreseach. Fullhorizons,5sdevelopmentdeadline,32decimalGB,
seed0, two profiles: perregion4Mvs8Mcandidates; allothergenericsettingssameas
priorgeneralization. Raw runs/cgar-allmaps-region-budget-full-v1-20260920.
AllinstancesexceptWAREHOUSEandRANDOM05. Verifier8900862heldbehindbenchmark;
results experiments/allmaps-20260920/results/region-budget-full-v1.
Frozenprofiles/inputhashes/helper/buildproofcommitted1a3de99. Newcode93cb18f,
regressioncorrectioneb99380. Source/testsmatchbuildexactly; no newbuildneeded.

Preliminary completed RANDOM rawtotals4M[588,988,1381,842],8M[588,985,1281,940].
Allmaxentries<=.254s. Originaluncapped[588,982,1409,997] had R04mean3.07s.
These are fastcompletepartialmatrixrows, not yetfullverified qualitypromotion.
Biggerregionalbudget is not uniformlybetter. Largefourmapsstillrunning.

## Next work

Read experiments/allmaps-20260920/{README,PROTOCOL,TRANSFER_REVIEW}.md and
CGAR_PROGRESS.md. New reference-inventory and reviewsourcehashes uncommitted.
Oldcompetitorreferences mostly1000stepcaps, mixed7532/9354CPUs andlarge-map
RSS32GBviolations; do not claim matched SoTA comparisons fromthem. NMS full
small-RANDOMreferences[638,1226,2350,2532] are16physical/32logicalEPYC7532,
where newCGARuses4physicalEPYC9354. Properfreshmatchedreferencesstillneeded.

Afterregionalmatrix, compare existingremaining-chainorder/unequalpower,
unopenedmatching and directcostHRRNoff as controlledgenericfactors. Preserve
CGARsafety/ownership; fairnessisnowsecondary. Uniformflowtestmustretain
schedulercandidatecoverage: justsettingFLOW_STRENGTH0alsoforbidsPICKUP_FLOW,
so avoidconfoundedon/offcomparison. NMSactiveguidanceisnativegeneratedfor
SORTATION/CITY/GAME, savedfieldforRANDOM; itsunusedconfigsLNSisnotactive.
KKactivecodeiswppl_planner, notmy_planner. Anyinstance-specificfieldor
configurationrequiresnewvalidated --trick INSTANCE gates and[trick]commits.
Do notpromotestandaloneR05scoresasCGARresults ormodifythatagent'scode.

Pushespublicfywu85/lorr authorized. UseGIT_ASKPASS=/bin/false
GIT_TERMINAL_PROMPT=0 git -c 'credential.helper=!/user/fw2449/.local/bin/gh auth git-credential' push origin main.
