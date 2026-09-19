# Continuing warehouse work

Updated2026-09-19 21:43UTC. Active unbudgeted goal: full5,000-step/10,000-robot
warehouse>=local KittyKnight152,981 repeatably over six seeds, complete entries<=1s
or explicit failure,RSS<32decimalGB, isolated GRID physical cores, generic policies.
KK uses38.858GB; this is not an official equal-resource SoTA claim. Goal remains
active/unmet. User hopes for progress by midnight; no promise or shortcut authorized.

## Confirmed progress: distinguish record from mean

Best SINGLE RUN remains144,510. No new single-run frontier was set this turn.
NEW confirmed SIX-SEED MEAN143,941.1667 versus142,474.8333 (+1.02919%).
Seeds0..5:144510/143933/144107/143134/143934/144029. All six full totals win;
five tails/ages improve, seed1tail−108/agep90+1. Mean remains5.909% belowKK.
The user asked why143941was a win when frontier144k; explicitly explained it is
confirmation of the prior two-seed regional gain, not a new best single run.

Working reference now warehouse-reference-variants.json: one4M worker/prep4,
gradedstrength4/scale4,margin25/refresh512,turnbuild128/cache8GiB,HRRN1/weight1,
pickupflowON/64completefields/4fieldthreads/ageawarekey0,4regions/4threads/
2rounds/25k attempts perregionperround,temperature1000ppm. Warm/mixed/surcharge/
next-errandOFF. ExactV44 sourcef6d0ae448504762136c4f5d85f25d7d207207965,all22hashes;
frozen runs/cgar-fractional-mixed-build-v44-20260919. Six-seed combined report
results/pickup-full-regions-six-seed-v44.json. All12candidate/control runs valid,
60kentries<=1s,all6controltrajectories exactV42/64,regionalcounters checked.
Candidate mean401.2–419.4ms,max962.689ms,CPU1.726–1.770cores/4reserved,
RSS13.909GB,full34.50–35.93min. Loadedturns/waits lowerall6; notcausaldecomposition.
Progress42increasingrecordsUNCHANGED,8confirmedmilestones. Latestmilestonecompleted
2026-09-19T21:14:15.691307UTC. WAREHOUSE_PROGRESS.md links exactsource andresults.
Preserve user. WAREHOUSE_PROGRESS.md.swp (actualfilename has no space afterdot),
exclude fromgit; read/compareprogress beforeediting. User may have editoropen.

Quota64/128/256 full0/2 all6complete: EXACT same fulltrajectories asV42/64fourthreads,
141829/142988,all30kentries<=1s,max892.703ms,RSS12.159GB. V46exact0b8361c1d28429f8fd9918a3470046d4414ad83f.
No observedbenefit fromlargerquotas/16fieldthreads; retain64/fourthreads. Notproof
quota64neverbinds elsewhere. results/pickup-full-quota-full-v46.json;PICKUP_FULL_QUOTA.md.

## V47 next-errand prototype and live full run

OptionalCGAR_TEMPORAL_NEXT_ERRAND default0,onlyrevealed distinct next errand.
Nonpinned/exactoriented/nonguided,existingresident complete next table via serial
non-LRUpeeks afterallbuilds. CommonB=min_heading D_next(currentgoal,h), arriving
choicesuse−4U+D_next_star(endpoint)−B+ALLpaidturn/forwardextras; nativeopterm and
virtualtrailingturns. Nonarriving/nativefallback,G=N,pinned unchanged. Noextrabuilds,
nohiddenfutures/noreservationrelease/nopartialplans. Approximate nextterminalpotential
persistsaftervisitingnextandlaterleaving. Finalseedturnprojectionstillcurrentgoal.
Countereligibleincludesfarawayrobots; changedchoicesnotexecutedactions.

Build8899126 COMPLETE/allregressionspassed,22frozenhashesreconstructedfrom3a5594a,
archivebuild-provenance/v47,rawruns/cgar-next-errand-build-v47-20260919;
binary4e7fac8db72a4c4ac24f8c673b54cb32ba74d1cd30b851f38d8eaf666a946467.
54,378independentweightedproduction-scorechecks,twoexactjointfixtures,sixnative
fallbacks,46realservices/repeated-locationservice/flowpublications,320serialparallel
robotactions,4800protectedregionalactions,existingprimary/pocket/capacitychecks.
Alltests passed; no activebuild. V47needs exactcommitbindingaftercheckpointcommit.

Screen8899133 COMPLETE,both200stepsvalid,OFFhash exactregional2a4bfe67f610e2451bb40e68da0e9fad0b75985d3f7701528334fee73f93cea8b.
ONhash0eea594b7d0264f9924673e30067f80514df62327a6ba4075f01b608255926d5;
maxOFF961.196ms/ON959.324ms,RSS5.273GB,disjoint4physicalcoreseach,noquota.
ONknown709016,eligible114178,unavailable594838,arriving153885,changed122104.
Noqualityranking. results/next-errand-screen-v47/equivalence.json.

FULL8899141/analysis8899143 LIVE: seeds0/2xON/OFF,4concurrentcasesx4cores=16,
64GiBaggregatereservation/32decimalGBperprocess,exactfrozenV47. Config
next-errand-variants.json,rawruns/cgar-next-errand-full-v47-20260919;future
results/next-errand-full-v47. OFFmustmatchregional2fulltrajectories144510/144107.
Aftercomplete,verify_full.py +exactcontrols+25nextcounterframes,thencomparefull
throughput/tails/ages/motion/limits. Usefulcandidate needsremaining4seeds.
Do not inferqualityfromshortprefix.

## Fable and next useful work

PersistentClaudeCodeCLI session1ebb1075-3538-49d1-93d1-a00c94fa256a,
lorr-warehouse-cgar-fable51,Fable5.1max,userexplicitsourcepermissioncontinues.
Turn14 SUCCESS2026-09-19T21:40:57.435658UTC,242.668s,correctmodel/session,
no tools/denials. Reported74.9763435USD is cumulative sessioncost,notturncost.
Cap24,preparedpayload58,350bytes. Visible review/metadata/status/localassessment
archived fable-flow-session/turn14. Rawprotocolignored. No liveCLI request.
SourcecacheV47+regionalheader;nextrequestturn15/followup-14.md ifneeded.

FablefoundnodemonstrableV47bug;read-onlyreview,nounperformedtestclaims.
Suggestedalready-on-goal fixture,selectednext-path/reasoncounts ifuseful. Its
finite-goal/infinite-endpoint fixture needs checkingagainstactualcompletetable domain;
a validpath connectinggoaltoendpoint maymakeitunrealizable. Do not fabricateone.

Regionalrepair currently keepsfinalannealedstateifbetterthaninitial,elserollback.
Manyacceptedmoves/rollbacks DO NOT imply a betterintermediateplanwasdiscarded.
Nextstep: cheap optional readonly running-scoremaximum/counter,final/initialgap,
lostbestcount/attemptposition. NoRNG/statechange; disabledandauditedtrajectorychecks.
Then test existingtemperature0 versus1000ppm onconfirmedregionalbaseline withaudit
counters. Temperature0stillconsumesworseningproposaldrawsandrollsbackEQUALbatches;
do not repeatFableclaim'neverreverted' literally. Fixedprescribedwork/timeoutfailure.
Onlyifcountersjustifyitimplementbestcompletestate retention; nofullvectorcopyeachpeak,
noanytimeclockoutput. Higherheuristicscorenotproofhigherwarehousethroughput.

## Evidence to preserve

V45postserviceaudit full0/2 exactV42trajectories,all10kvalid,max884.17ms,RSS11.697GB.
After1000known-next-onlyblockerclass7.32/7.70robotspersnapshot(~0.073%eligible),
finishing-onlylarger23/frame. Smallclassnotthroughputbound;objectivechangesaudit
candidateuniverse. POST_SERVICE_AUDIT.md andresults/service-audit-full-v45/audit.json.
Offlinegoalholdingandcontinuationprobes checkedindependently. Commonbaselinescore
54,378weightedcases; FRFFF/RFCFWphysical−24/−16 versusrejectedbaseline−24/−20.
Neutral-tailseparatehypothesisonlytwojointfixtures,no productionimplementation.
CONTINUATION_SCORE.md,GOAL_HOLDING.md. V47regressionusesactualproductionhelper.

Do notrepeatunchangednegativeevidence:4/16globalworkersmixed6seed;fractionalturn
extras1/2 andwarm/mixedlosebothfirstpair;strength2mixed,6/8lose;age-removingcostkey
regresses/collapses;quota32seed0collapse;orientationcache8/16GiBsamefulltrajectories;
orientationbatch128mixedsix/512fails;pool/transactions mixedsix. Originalnoflow
resultsarenotproofundernewflowpolicy. >99.5%loadedturnsplanned,seedturnties54–83/full.

## Operation

All exec_command require sandbox_permissions=require_escalated (defaultbwrapbroken).
No proactive subagents. Fullhorizonforquality;200stepsonlyfeasibility/equivalence.
HeavyfulltrajectoryreplayonGRID;smalllogs/JSON/toyworklocallyfine. Allfixedworkfinishes
orentryfails. Physicalcoresisolated;aggregatejobmemorymayexceed32GB,processRSSmaynot.
verify_full.py validates22source/testhashes,actualbinary,5000timings,32GBRSS,
50Mactionconservation,noquota/coreoverlap;addcontrolhashandfeaturecounterchecks.
Commit/pushauthorized,deliberatelyPUBLICfywu85/lorr. Excluderawruns/env/binaries/
protocol/swap. Sourcefilesfrozenbeforebuild;do noteditwhilebuildlive.
