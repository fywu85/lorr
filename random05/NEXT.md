# RANDOM-05 continuation

Updated: 2026-09-20 17:33 UTC.

Goal ACTIVE. User: keep pushing throughput until evening; order latency is
secondary, with no fairness constraint that sacrifices throughput. Independent
Random05 combined solver guided by the colleague's pasted log, not their code.
Target approximately27–28% matched NMS gain onfourcores, then improve further.

## Verified records

- Overall /32workers:3705, source05559b7/build-v55, K8192/B8/start2/local0,
  gen4/E8/persist8, plannerseed3. Mean240ms,max361ms,RSS591MB. +16.8% vsNMS3172.
- Fourphysicalcores:3655, source05559b7/build-v55, same exceptK5120.
  Mean790.7ms,max931.1ms,RSS309MB. +25.4% vsstrongestNMS4=2914.
  Evidence:results/persistent-parents-four-split-full-v55/k5120-elites8-persist8/.
- The latter is a selected record, NOT a replicated mean improvement:
  E8/P8 seeds0–4=3583/3495/3614/3655/3602,mean3589.8,1/5positive vsE1/P1.
  E1/P1 seeds=3611/3526/3648/3637/3608,mean3606.0. Fast32 seed3 reproduces
  everyaction/schedule/event/task of3655four.
- Fresh validationV3 COMPLETE: frozen81bdfbd,source6ce9312/build-v52,
  K5120/B8/gen4/E1/P1,plannerseed3 (3637 development run).
  Input50005:3698 vsNMS2926/2900;50006:3619 vs2906/2831.
  +26.38%/+24.54%, aggregate+25.46% against strongerrepeat, all6valid.
  results/fresh-validation-v3/audit.json. Never substitute laterrecords.
  Inputs50001–50006 are heldout and MUST NOT be used for tuning.
- Cutoff-free oldbests3285four(K2048/gen1) and3408on32(K8192/gen1),stillguidance
  trick. Known-horizon controls3501/3596. No newfairnessbound.

All records full2000steps,strict1s,30spreprocess,32decimalGBguard,noerrors/timeouts.
4workers=4physicalcores;32workers=16physicalcores/SMT2. EPYC9354 verifiedGRIDbinding,
sharedhostsallowed. Fixedwork;timeout exit124,no partialsearch. NMS4=2914 strongest
repeat(others2902/2903),NMS32=3172. Sameinput/machineallocation; publishedscoresindirect.
NMSsource retains documented constructorfix,4workerbuildonlychangesTHREADS32->4.

Common bestenv: field15 generatedflow,average/normalize,power3,between.75,
penalty2.4,oneflippedpair/flipseed5;turn.6/wait.5;depth8/noise200/dispersion.8;
Hungarian1000,guidedmatching,keep.5/length.25;horizon2000/triage1.5;stepRNG.
Exactflags shareprefix,packedorder,fastdispersion,scratchreuse,goalcache,radixorder,
candidatecache. Defaultcache64. Mutation/futuremutation.3;decay1;risk0.
--trick RANDOM-05 gates field/horizon/agecap. Algorithm/cachechangesgeneral.
See best*.json for frozen source/config/evidence, not current defaultflags.

## Completed latest tests

v55 persistentelites: K8192/E8/P8=3705 vsP1=3689,P4=3671.
FourK5120/E8/P8=3655,P4=3599,vsP1=3610. E1/P1reference3637.
Higherrootmutation.5/1 fails: K5120/E1=3545/3460vs3637;
K8192/E8/P8=3537/3472vs3705. Preserve.3.

v56 0bd0a41 mutationdecay.25/.5/.75failsmainbudgets;defaultcontrols3637/3705exact.
v57 0295ae9 FIRST_K5120 reservesstartuproom;regular5632/5888fourvalid3650/3591,
max930/968ms. Regular6144/cache64failedstep2at1000.533ms,exit124,nopartialscore.
Fast32counterparts3650/3591/3545;fulltrajectoriesequalwherefourvalid.
IncreasingKisnotmonotonicallybetter.
v58 bdc051f cachecapacity64/128/256/512 preserves3637 fulltrajectory.
Fourmeans790.96/778.00/775.68/772.03ms,max920.19/914.66/906.41/901.01ms.
RSS300860/317468/358912/435544KiB. Slots512saves2.4% meantime.
FIRST_K5120/regular6144with128/256/512allVALID3545(max989/997/982ms).
Allthreeexactlyequalfast32trajectory. Earlierfailed64attemptretained.
results/ranking-capacity-four-split-full-v58/6144-equivalence.json.

v59 **d6a3e0f**, new R05_FUTURE_ELITE_BLEND default0,range[0,1],positive
requirespersist>=2. Percontinuation,chooseonepastpriorityvectorcyclically.
Atoriginalfuturemutationmaskpositions,blendnewrandomoffsetwithretainedoffset.
SameRNGdraws/masks,branch0constant,emptyhistoryfallsbackrandom. No stalescores,
actions orreservations. Samefixedwork. Tests8.63sPASS includingzero-mutation
invariance,dense taskturnover/virtualassignment,worker1/2determinism.
Build-v59 SHA7ab2ed25fa8795ae367c58a9104e95b59a3648b03f21cdbbb023a8721b5d40c4.
FulldefaultcontrolsMUSTreproduce3655/3705. No resultor gainclaimyet.

Earlier rejected: optimism risk-.25(mean3492.8vs3531.4),4parentsK2048neutralmean,
blindfields33–48alllosetofield15,predictivematching3450/3541vs3520/3637,
oldoperationkernelsfarbehind. ReviewRESEARCH/mainlogbefore repeating oldhypotheses.

## Live jobs at this checkpoint

- persistent-budget-split-full-v55:8900370 K16384/B8,8900371 K16384/B16,
  8900372 K8192/B16. Allgen4/E8/P8/seed3/32workers. Source05559b7.
  Tests roots-vs-continuationbudget against3705K8192/B8.
- elite-futures-split-full-v59:8900373/374/375 K5120blend0/.5/1;
  8900376/377/378 K8192blend0/.5/1. Allgen4/E8/P8/seed3/32workers.
  Controls3655/3705; allfixedstrict1s/full2000. Fourcoreconfirmonlyifpromising.

Readqstatandsummariesbeforeacting;do not rerunmissing/NFSdelayed cases.
Use split_grid.py collect, archiveonlycompactbatch/spec/allocation/summary/completion.
Use one-/two-levelglob,NEVERrecursiveNMSworkingdirectorycopy.

## Accounting / secondary metrics

Mainprogress now61rows audited. audit_progress.py validates fullsource/input/
trajectory/hash/allocation/latency and generates WAITING_PROGRESS.md.
Taskwaitauditmanifest/report:results/task-waiting-frontiers-20260920T1612/.
3655maxcompleted1916,initialunfinished141/unopened98/started43.
3705max1922,initialunfinished135/unopened94. NMSfourmax1997,initialunfinished219;
NMS32max1976,unfinished206. Everyrunhasstep0ordersunfinishedat2000;eventualmaximum
unknown,atleast2000. Completed-onlymaximaexclude censoredtail.

Currentactionaudit results/action-audit-current-v55/:allfourtrajectoriesreplayvalid.
On2880commoncompletedtasks,fouroursuses24.16fewerturns,6.20fewerforwards,
16.99morewaits/task (net13.36fewerloadedsteps). On3114common32tasks,oursuses
13.63fewerturns,2.48fewerforwards,24.65morewaits(net8.54moreloadedsteps).
Assignments/congestionhistoriesdiffer;notcausaleffectsorrecoverablethroughput.
Triagealsosuppressesgoalslate,soactive-vs-suppressedwaitingdiagnosis mayhelp.

## Fable and workspace

Approved exact79KBpayload was attempted15:01UTC viaClaudeCode,max,samesession
27a4316e-b79d-46cf-86b4-41b0f558938a. Provider429/model_requires_usage_credits;
NO REVIEW,zerousage. Useralreadyinformed;optionalreviewnotblocking.
Payloadruns/random05/fable-review-01/payload.txt SHA
 ded0889fcaa2cb82a3f4fd9d5dc46f737bdf50cf7a9e466bb66f9114f9661d0f.
Old7506a snapshot;do not silentlyreplaceauthorizedpayload. Ifcreditsreturn,
retryexactbyteswith--resume innewoutputdirectory;preservefailedattempt.

SharedmainwithWarehouseagent. Onlyedit/stage/commit random05/ andRANDOM05_PROGRESS.md.
Leaveallotherchanges/jobsalone. No newsubagentdelegation. Publicfywu85/lorr,
pushauthorized. Lastoursd6a3e0f(17:29);notyetpushedthiscontinuation.
Commitusing git commit --only ... -- random05 RANDOM05_PROGRESS.md.
Shellrequiresrequire_escalated duebwrapnamespacefailure. Python3.7;env/bin/python
forNumPygenerator. apply_patchbroken,usePython. No relevantAGENTS/RTFfoundlocally.
Push GIT_TERMINAL_PROMPT=0 GIT_ASKPASS= git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main.

Next:collectnewfulltests,validatecontrolsfirst,confirmnewselectedrecordsonfourcores
wherefeasible,updatebest/log/waitauditforeachrecord. Preservefreshprotocolsandheldout
inputs. Commitcompactevidence/checkpointandpush. Goalremainsactive.


Latest update: full four-core profile job8900381 is running (build-v58,
K5120/E8/P8/cache512, expected3655; bothphaseandinternalpolicyprofiling).
The firstfive sampledsteps put about42%ofpolicytime in the twoPIBTpasses,
20%setup,13%cycles,11%actions,8%sortingand5%ranking. Prefixdiagnosisonly.
Newoptional R05_KINEMATIC_MASK caches sorted candidate eligibility and specializes
optimistic/kinematic passes. Sourceandtestsjustedited; build-v60 next. No speed
orvalidityclaimyet. Test dense equality includingpushcost,turnrestriction,
expansionlimits,pinnedcomponents,eviction,taskturnoverandworker1/2.
Goal-suppression auditcompleted: loadedW active/suppressed520097/79060four,
520986/8139532. All20native samplesmatch;about13%ofloadedwaitssuppressed.
Elite-futureK5120blend0/.5/1 completed3655/3528/3506; defaultfullcontrol must
be compared before archiving. K8192pending. LargerB16/K8192completed3568,
below3705;K16384casespending.
