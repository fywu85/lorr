# Continue CGAR across the competition, with dense-map transfer now emphasized

Updated 2026-09-21 around 00:35 UTC. The latest actual user scope supersedes stale
formal goal metadata: Warehouse strict1s is resolved, now target all ten2024
instances, NMS primary, throughput primary, fairness secondary. The user most
recently explicitly requested studying the separate RANDOM-05 algorithm and
borrowing mechanisms for CGAR RANDOM-04/05. Do not chase Warehouse160k now or
falsely mark that stale formal goal complete.

The other agent owns random05/ and RANDOM05_PROGRESS.md. Never edit, stage,
collect, build or interrupt its files/jobs. Read-only study is authorized.
Our newly authorized CGAR RANDOM-05 benchmarks use cgar binaries and separate
runs/cgar-* directories; the standalone solver is untouched. Do not label its
scores as CGAR. No internal subagents. Fable session1ebb1075-3538-49d1-93d1-a00c94fa256a
last exhausted credits atturn46; no quota change, no retry. Next would be47.
Shell calls need require_escalated (bwrap broken). Shared main/index: explicit
paths, git commit --only; never clear locks or stage the other agent's changes.

## Warehouse strict1s resolved and pushed

V123 source3e319f175859396b91f97364b320c6794a85cbb7,
binarye3ec32ecdfdaf06f7db139da3857af8fa0ad43b98bd67479f7b9bd969dc1302c.
Full seeds0/2 exactly reproduce155173/155090 and V110whole trajectories;
10000complete entries /100million independently replayed actions. Max944.766/
957.827ms, mean428.056/425.740ms, RSS15.396/15.393GB, mean1.770CPUcores.
8physical cores perprocess, exclusiveEPYC9354research44, enforced1000ms.
8threadprep/prefetch/pickup and25GiBlogicalunwarmedcache; all-goalprewarmOFF;
original search work unchanged. Evidence construction results strict-runtime-full-v123
and -outcomes, raw runs/cgar-strict-runtime-full-v123-20260920. Commitab388c9pushed.
Original best155173source37f592a and timestamp20:36:47UTC preserved. No newpeak.

## Completed general transfer results, all verified and committed

Campaign experiments/allmaps-20260920; root CGAR_PROGRESS.md. Published NMS
2024-MainCombined targets frozen TARGETS.md / published-targets.json:
WAREHOUSE154795,SORTATION152714,CITY01=8420,CITY02=16787,GAME23274,
R01=639,R02=1221,R03=2334,R04=2547,R05=3050. Historical targets are not fresh
matched local comparisons. NMS Warehouse/Sortation/Game published timeout tags
retained. Old local leaders are capped/mixed hardware/over32GB; not cleancontrols.

Regional-cap sourceeb9938090d7cfb331fdc124ce19b475626356502; binary
19155f20da9f518cfac5955c4bcc4a3af9926ff4828e552f3bd69026201b5c3f.
Build-v1-r2 full regressions and all28hashes pass. Optional perregion/perround
CGAR_TEMPORAL_REGION_CANDIDATE_LIMIT defaults0, checks complete attempt boundaries;
last attempt mayovershoot, timeout alwaysfailure. Default0 preserves Warehouse.
Full8instance4Mvs8Mmatrix8900861/verifier8900862 COMPLETE16valid,0failed.
Results region-budget-full-v1. Bestperinstance among those rows:
SORT146460,CITY01=7305,CITY02=14068,GAME6519,R01=588,R02=988,R03=1381,R04=940.
All5sdevelopment,4physicalcorescase,EPYC9354,32decimalGB. Sortation max1.031/1.045s;
other maxima below1s observationally, not strict qualification.

Generic scheduling/rank factors8900894/verifier8900895 COMPLETE24valid,0failed.
Sourceeb99380samebinary,6profiles same4Mregional/4Mglobal budget, seed0R01..04:
control[588,988,1381,842];chain_order[587,992,1395,909];
chain_rank[582,1020,1435,1249];match64[601,1021,1368,938];
direct_cost[587,1052,1433,936];match64_direct[611,1084,1484,982].
Evidence results/generic-factors-random-full-v1. Means125-147ms, maxima<=225ms.
Direct cost worsens waiting tails, explicitly secondary; forced-oldest remains.
Current selected table uses different exploratory profiles, NOT one universal
preset or automatic per-map dispatcher. BEST_HISTORY.md and selected-full-results.json
have timestamps (result-file completion mtimeUTC), source links and exactsettings.

CGAR RANDOM05 baseline8900922/verifier8900924 COMPLETE2valid,0failed:
control1534,chain_rank1847. Mean123/124ms,max183/178ms,RSS191MB. Sourceeb99380,
seed0,full2000steps,5sdevelopment. Results random05-baseline-full-v1.
Committed6307964; root progress/history updated. Standalone R05 untouched.

## Explicit SORTATION native field implemented, full comparison running

Source df94a520c77fcb3a73a4a8c74b5b13cab542d1d6; binary
a23a3c81195d71a7a13c9ee518d9896c0a1e130d985037080a18871ad74b606c.
Build runs/cgar-sortation-native-build-v1-20260920,29source/testhashes andfullsuitepass.
Generator compiled unchanged NMS GraphGuidance(MapTypeSORTATION), verified all
280000forward +840000nonforwardentries and independentrow/colformula. Fieldasset
cgar/tricks/sortation_native.hpp, source/proof experiments/allmaps-20260920/sortation.
Exact140x500occupancygate prevents Warehouse/Sortation mix. NewFieldAsset selector
preserves Warehouse defaultAPI/hash; driver and benchmark receipt parsers accept
--trick SORTATION. Genericdefaultunchanged; explicitlanesOFFcontrol tested8ticks.
Independentorientedheapchecks bothbands, corruptedfield/wrongmap/unknownR05rejected.

Fulljob8900903, verifier8900905running/held. Raw
runs/cgar-sortation-native-full-v1-20260920; expected results/sortation-native-full-v1.
3cases seed0,4physicalcoreseach,sharedresearch44,5sdeadline,full5000:
trick_sortation_control lanesOFF; native20/200 bands0; native20/200 bands1.
Samegenericregional4Mprofile, pickupweight1,HRRN1,matchingOFF,knownhorizonOFF.
Control whole trajectory must reproduce regional4M generic146460 when complete.
No nativeSortation quality promotion yet. Profiles frozen sortation/variants.json.

## RANDOM05 source study and first actual mechanism transfer

Read frozen independent source233f5bfc26cffef8dc039632b96391b50a55fee6 (3872record
code); /tmp/cgar-random05-source69-review contains engine.cpp/hpp,operations.cpp,
README/RESEARCH/tests. Ownership preserved. Review andhashes under
experiments/allmaps-20260920/random-transfer/REVIEW.md,source-review.json.
Main differences: exact oriented full-chainDP;2step movement promises/pre-rotation;
persistent/noisy priority vectors; average commonfuturecontinuations with same
firstdecision; joint fullpool idle/unopened matching; safe occupiedcycle motions;
tuned allpairs flowfield; knownhorizontriage. Lasttwo tricks. Standalone without
cutoff3503stillhasfieldtrick. Fresh25.42%matchedgain is standaloneevidenceonly.
Negative standalone directions include3stepoperations, componentrecombination,
completionbonuses andage-retention. Do not blindly copy failed settings.

New generic optional PRIORITY PORTFOLIO source
5d3c5bf51147d9739de9c457a679d727e8492848; binary
878d47d086a120a866bd68fb0068850dee4db1085f514084f8c5c38a1bcf3a70.
Build runs/cgar-priority-portfolio-build-v1-20260920, all30source/testhashes match,
fullregressionspassed; proof random-transfer/build-v1, commit040460d.
Header temporal_priorities.hpp + cgar.hpp/cpp/temporal_adapter integration.
CGAR_TEMPORAL_PRIORITY_NOISE defaults0, integer0..1000000 inbaseforwardunits;
PERSIST defaults0; MUTATION default30percent. Atleast2workers andorder1/2required.
Worker0 reevaluatesparent; everyfourthalternativefresh;othersmutateparent.
Optionalwinningoffsetsretainedonlynextconsecutivestep. Workerorderchanges ONLY;
sharedpower/scores/reservations/CGARprimary/recovery/protectedseedsfixed. Complete
workers beforeselection, timeoutfailure. RNGseparatefromexistingrepair streams.
Regression720zero/defaultactions,3840serial/parallelactions, lifecycle/prefix,
range/invalidconfig anddeadlinehistory. Not the R05 pipeline orcontinuationsearch.

Portfolio fulljob8900935, verifier8900936, raw
runs/cgar-priority-portfolio-dense-full-v1-20260920, results priority-portfolio-dense-full-v1.
5profiles seed0fullR04/05;parallel2suitesx2instancesx4cores=16physical,sharedresearch57.
Controlchain_rank1worker4M;8workers500keach;noise50cold;noise50persistent;
noise200persistent. Regions4x4Mx2 andscheduleridentical. Constructioncanovershoot
candidatecaps; nominalaggregate4M is NOT an exactequalworkclaim.
COMPLETE: 10 valid, 0 failed. One-worker 1249/1847; eight-worker 1330/1989;
noise50 cold 1306/2036; noise50 persistent 1193/2003; noise200 persistent 1187/1954.
Best per-instance mean decision times 101.89/100.24 ms, max 141.56/291.41 ms.
Persistence is not selected. Complete one-worker trajectory hashes exactly
reproduce old source eb99380 on both maps; proof random-transfer/disabled-full-equivalence.json.

Objective benchmark 8900949 COMPLETE: six valid, zero failed. Control 1249/1847;
remaining-potential 1243/1749; partial-next-errand 1207/1760. Both changes lost.
Verifier 8900951 failed allocation preflight before analysis; unchanged frozen
helper retry 8900966 on research44 passed. Retain failure evidence; the old helper
did not log which exact allocation invariant failed. Results dense-objectives-full-v1.

Strict replication: source 5d3c5bf, job 8900997 running on research44, held verifier
8900998. Raw runs/cgar-priority-portfolio-strict-seeds-v2-20260920, results
priority-portfolio-strict-seeds-v2. Three profiles (one worker, eight workers,
noise50 cold), seeds 0/2/4, both maps, enforced 1000 ms, four physical cores per
case, full horizons. Compare seed0 whole trajectories with the 5s runs. Planner
seeds only, not independent input streams. Protocol random-transfer/STRICT_SEEDS.md.

Read-only reference guidance export COMPLETE, not yet integrated into CGAR.
Raw runs/cgar-random-guidance-asset-v1-r2-20260920, successful job 8900967.
Original job 8900960 failed resource preflight before compiling; retained evidence.
Frozen source 233f5bf, full float field identical at 1 and 4 threads. Integer
adaptation rounds half-up at scale10: forward 5..43, turn6, walls20, max error
0.04999218 in original units. Proof and float/integer artifacts under
random-transfer/guidance. This is preparation for an explicit map trick, not a
CGAR quality claim. No standalone working-tree writes.

## Next actions

Collect pending fullmatrices and verifier receipts, retainfailures if any. Analyze
partial results only as provisional. Freeze/recheckpromising configs onfreshplanner
seeds andstrict1s. Updatebest/historywithsource+UTC even singleseedmaxima;
fairnesssecondary. Compareunchangedfullcontroltrajectoryhashes, notjustcounts.
The reusable analyzer analyze_matrix.py freezes helpers/profiles/inputs, validates
sourcehashes/binary/physicaldisjointcores/deadlines/RSS/movement/eventwaiting,
recordspriorityactivationreceipts; --allow-random05 explicitlyallowsourCGARcase.
It is NOT independentfullactionreplay. Earlier analyzer supports8maporiginalonly.

Potentialnext meaningful transfer: fullchain-orientedcost or pipeline proposals
reconciledwithCGARprotectedpaths, thenmultiplecommonfuturecontinuations. Existing
R05 guidance could be exported read-only fromfrozencode and adapted behindnew
--trick RANDOM-04/05 gates; current CLI supports ONLY Warehouse/Sortation, so
NEVER silently loadrandomfield. R04/05shareoccupancy butdifferentrobotcounts;
validate namedinstance too. BestR05field settings:flowseed15,average1,normalize1,
power3,betweenness.75,outputpenalty2.4,oneflipseed5,turn.6. WeightsareNONUNIFORM
floatsafterloadnormalization, notjust2/6.8. Any integeradaptation needs declared
quantization andvalidation. Generator is complete; RANDOM CLI/field integration is next.
Do nottouchheldout standaloneinputs50001..50008for tuning.

Public fywu85/lorr push remains authorized; do not change visibility. The last
completed push advanced through 6307964. Use GIT_ASKPASS=/bin/false,
GIT_TERMINAL_PROMPT=0 and the gh auth git-credential helper for subsequent pushes.
