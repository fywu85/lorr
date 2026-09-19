# Continuing warehouse work

Active unbudgeted goal: >=152981 tasks over5000 steps/10000 robots, repeatably on
six seeds, every complete schedule+plan entry<=1s, explicit timeout failure,
RSS<32000000000B, isolated GRID physical cores. No map identities/templates,
supplied weights, future tasks or map-specific fleet caps. Local KittyKnight used
38.858GB; this is not an official equal-resource comparison. Goal active and unmet.
Prior turns made concrete progress; no blocker. Maintain ../../WAREHOUSE_PROGRESS.md.

## Current confirmed reference

Graded strength4/scale4, margin25, refresh512, turn-build128,8GiBcache, HRRN1 and
pickup weight1, pickup flow ON,64 complete pickup fields/four field threads,
original age-aware key0, one planning worker4M with four preparation threads.
warehouse-reference-variants.json is explicit. Exact V42 source4872d048dd8155927074ac2a4d27a55756266fd5;
all22 source/test hashes verified. Frozen runs/cgar-pickup-full-build-v42-r1-20260919.

Seeds0..5:141829/143325/142988/141802/141988/142917, mean142474.8333, range
141802–143325, +2.214684% over preceding normal-pickup mean139387.8333.
All six totals/final1000 improve; empty robot-steps down7.43–7.84%, agep90 down5–14.
Quota16 also improves all six:139697/141925/141725/140734/140928/141878,
mean141147.8333 (+1.262664%); quota64 beats16 in every full total.
All18 paired runs valid; all six0-field full trajectories exactly match V41.
results/pickup-full-six-seed-v42.json; PICKUP_FULL.md.

Resources for64 across six seeds: mean272.4–317.4ms, median241.2–298.6ms,
p99557.3–602.0ms, max874.138ms, CPU1.425–1.494cores/4 reserved,
peakRSS11.934GB, full wall23.93–27.70minutes. All30000 entries<=1s.
CPU is whole-process user+system/wall (including startup/sim/logging), not an
instantaneous sample. results/full64-reference-latency-six-seed-v42.json,
audit8898959 complete. The user's resource question was answered with these new
confirmed figures in commentary. Do not keep repeating without a reason.

Progress now40 increasing full-run records, last143767 onfour-worker policy,
not promoted aftersixseeds. Previous confirmed high143325 completed18:13:40.691469UTC, exact
4872d04. Latest143767 completed19:05:34.390519UTC, exact1661176; source/hashvalidated.
confirmed-reference-progress.json has seven increasing six-seed milestones.
User editor swap .WAREHOUSE_PROGRESS.md.swp exists; exclude fromGit and do not
delete. Preserve concurrent user markdown edits with read/compare-before-write.

Latest checkpoints:0fb86dd (worker six-seed/flow-strength reports) and
f6d0ae448504762136c4f5d85f25d7d207207965 both pushed to PUBLIC fywu85/lorr.
The latter EXACTLYmatchesall22V44source/testhashes; binary
5e9c35b2a74e4cf441360a519167701e54db48e975152cba32defa0609a5e482.
No C++buildactive; sourceunchangedsincebuild8898977. Userresourceanswerstill
usesconfirmedone-worker64sixseedreference. Keepactivegoalunmet.

## New V44 source and review checkpoint

Optional CGAR_TURN_SURCHARGE adds scaled integer turn surcharge, default0,
requiring temporalflow andphysicalturn1, effectiveT<=16. Uniformeffectivecost
feeds reverseoracle, PIBTmacros, temporalscorer andbothpickupsearches. Fallback
remainsunitturn. New CGAR_TEMPORAL_MIXED_START default0 requireswarmstart and
>=2workers; worker0warm,otherscold, allfinish beforestrictscorecomparison.
Actualconstructorparticipation counters; allpriorprotection/joins/historychecks.
No wallclock-selected partial output. No new throughput claim.

Build8898977 complete, allCGARchecks pass. Frozen
runs/cgar-fractional-mixed-build-v44-20260919, archivebuild-provenance/v44,
312linepatch independently reconstructed fromd4ece2ebbf483678b6a8f837692ec58228c8b0ff;
all22source/testhashesverified againstarchiveandcurrentfiles. No C++buildactive.
Tests88704independentpickupgoal costs+reverse/bounded/completebasisagreement,
12288weightedDijkstra distances,724608paid-progressscores,120768uniformscores,
11520neutralproductiondecisions,12productionfractionalpickupcases,
3840mixedserial/parallelrobotdecisions withactualwarm/coldparticipation.

Screen8898980 complete6/6valid,max935.833ms,RSS5.233GB. Surcharge0one/fourcold
wholeprefixesexactV43,actualmixed1warm/3cold/allwarm4/allcoldconstructorsverified.
FRACTIONAL_MIXED.md andresults/fractional-mixed-screen-v44/equivalence.json.
Full8898981/8898982 COMPLETE:12/12 valid, all60000entries<=1s,max937.83ms,
RSS12.147GB, all22sourcehashesexactf6d0ae4, fourfullcoldcontrols exactV43.
Surcharge1/2 means141773/141931 (−0.446%/−0.335%versusoneworker0).
Allwarm/mixed means142381.5/142770.5 (−0.693%/−0.421%versusfourcold).
Allfourcandidateslosebothmatchedtotals;notpromoted. Surcharge2 reducesloadedturns
9.04%/9.54% butwaitsrise12.86%/10.93%. Actual5000-stepwarm/coldparticipationverified.
FRACTIONAL_MIXED.md;results/fractional-mixed-full-v44/comparison.json.

Fableturn11 complete18:47:43UTC,515.385s,47.123751USDactual,samesession,
noerrors/denials/tools. Archivedvisiblefinal/metadata/status/qualifiedassessment.
No newreproducedmotionbug. It proposedsmallfractionalturnandmixedwarm/cold;
thesearehypotheses,notnewgains. Allwaitvirtualrotationdiscountcanrepresent2turns,
notuniversal1slot. P/Q arithmeticwasnotarealizablegridfixture. Do notadopt
proxy-onlyrejection/permanentfalsifierrules. Noarrival-scorechangejustified.
Newcodewasimplementedafterreview; thereviewdidnotinspectV44implementation.

Fullrotationattribution eightcasesnowverified: sixconfirmed64plusfour-worker0/2,
allseed/planned/protectedturnssumexactlytomotioncounters. Tiedwaitturnsonlysixseed
54–83,planned>99.5%of~2.2Mturns. Full64-rotation-attribution-v1.json;LOADED_MOTION.md.
This is observedprovenance,notaninterventionbound.

## Completed latest experiments

V43 exactsource1661176ca3dd6854a7ba8d1a8a0ec8e33a2cffa9, all22 source/test
hashes verified. Frozen runs/cgar-pickup-cost-key-build-v43-20260919,
binaryf6d2a8ef5963e27a2408b503efb283ceb2d686155688d886fa81301a9e29313f.
Build8898877/all regressions complete; no C++ build active and no production
source changed since1661176. V43 fixes pickup CPU-affinity validation and adds
optionalcost discovery key. The default preserves V42 by full trajectory evidence.

- Cost-key full8898901/analysis8898902: all8 valid, all4default0/32 controls
  exactly reproduce V42. Fields32/key1=135244/137131 vs healthy0-field
  138963/139677: mean−2.248%, empty travel+17.46–17.95%, agep90+2755–2785.
  It rescues collapsed32/seed0=70043 but still loses to healthy reference.
  Fields64/key1=135773/79601 vs original64141829/142988; seed2final5540.
  Reject both cost-key profiles. PICKUP_COST_KEY.md and results/pickup-cost-key-full-v43/.
- Worker full8898924/analysis8898927:1worker141829/142988,2workers141432/142935,
  4workers143340/143409. Bothone-worker fullhashes exact V42/64, all6valid,
  disjoint fourphysicalcores each, noquota, all22source hashes checked.
  Fourworkers +0.67833% mean, finalwindows+60/+83, agep90unchanged,
  emptyrobotsteps+0.647%/−0.758%. Mean316.0–329.1ms,max912.607ms,
  CPU2.619–2.679cores,RSS11.885GB,27.39–28.47min. Two workers losesboth totals.
  Fourworkers was subsequently not promoted after six seeds (see below). PICKUP_FULL_WORKERS.md,
  results/pickup-full-workers-full-v43/comparison.json.
- New loaded action audit8898958 completed4 trajectories. decompose.cpp/.py
  now partition every50Mrobotsteps, assert conservation and agree with all CGAR
  phase/action counters. Completedtask loadedactions=finish−pickup; all old
  reportfields exactly preserved. Both handcountedfixtures pass. No planner change.
  Percompletedtask:64s0 chain241.758+extraF9.125+turns11.191+wait15.311=277.385;
  64s2 241.809+8.195+10.355+14.100=274.459; failed32s0
  235.286+35.573+44.036+77.821=392.715; KK233.836+4.594+5.678+8.411=252.518.
  Different completedcohorts, unfinishedworkcensored, necessaryturnsincluded;
  not causal/additive savings. LOADED_MOTION.md, results/loaded-motion-audit-v1/.

## Live jobs and next comparisons

-8898962/analysis8898963 COMPLETE. Fourworkers sixseed143340/143767/143409/
 143359/142868/139710,mean142742.1667 (+0.187635%). Fivefull/tailwins, butseed5
 loses3207total/1933final1000, agep90+53. NOTPROMOTED. All12valid, all6oneworker
 fullhashesexactconfirmedV42/64,22sourcesmatch1661176. Report
 results/pickup-full-workers-six-seed-v43.json. Newabsolutehigh143767at19:05:34UTC
 source1661176; record40logged, confirmedmilestonesremain7. Results/docs
 committed/pushed0fb86dd; V44productionalreadycommitted/pushedf6d0ae4.
-8898971/analysis8898972 COMPLETE. Strength2/4/6/8, all8valid, bothstrength4
 controlsfullhashesexactV42/64,22sourcesmatch1661176. Seeds0/2:
 2=141776/143213 (+0.0604%mean,mixedtotals,final+35/+128,age−4/−3);
 4=141829/142988;6=141085/142854(−0.308%);8=139351/142470(−1.052%).
 Retainstrength4;no repeatablegain established. Fullcomparison
 results/pickup-full-flow-strength-full-v43/comparison.json. No newabsolutehigh.

Additional V43worker scaling:8898975screencomplete. Full8898978/8898979 COMPLETE,
4/4valid,all20000entries<=1s,max954.12ms,RSS11.976GB,all22hashesexact1661176,
disjoint16physicalcores/process,noquota.8workers143157/142645,mixedagainst1,
losesbothagainst4.16workers143749/143571,mean143660,+0.87881%vs1,+0.19913%vs4,
bothfull/tailwinsagainstboth; agep90−6/−2andempty−0.15%/−1.00%vs1.
16workersmean379.5–392.0ms,CPU7.43–7.68/16reserved. Notyetpromoted.
PICKUP_FULL_WORKER_SCALING.md;results/pickup-full-worker-scaling-full-v43/comparison.json.
No newabsolutehigh:143749<existing143767. Reference/historyrecordsunchanged.

New16workerconfirmation:8898991/analysis8898992 seeds1/3,8898993/analysis8898994
seeds4/5. Bothmatricesrunningfourconcurrentcases(one/sixteenworkersperseed),
64physicalcores,16perprocess,64GiBaggregate,32GBprocesscap. research38/research50,
started19:34:46/19:35:16UTC. FrozenV43,exact1661176. Alloneworkercontrolfullhashes
mustexactV42/64beforeinterpretation. Configpickup-full-workers16-confirm-variants.json.
Rawruns/cgar-pickup-full-workers16-confirm13-v43-20260919 andconfirm45 analog.

## Opposite-turn audit completed

Offline job8898983 complete, seven full replays, four hand-counted fixtures pass.
All prior fields exact forthree overlapping reports; all CGAR phase/action
counters exact forsix trajectories, eachconserves50Mrobotsteps. Source/script
hashes exact. Source changes are offline decompose.cpp/.py only; no planner change.
TURN_REVERSALS.md and results/turn-reversal-audit-v1/ archiveJSON/submission.
Stationary opposite-turn transitions percompletedtask: one/fourworker seed0
0.524/0.376,seed2 0.432/0.371,seed5 0.424/0.697; KK0.110. Reversals correlatewith
firstpair improvement andseed5regression; notcausalproof orunique wastedactions.

Possible next assignment diagnosis (not implemented): task_chain_cost is spatial
whilepickupseesheading/flow. TurnDistanceOracle::peek(goal) alreadyexistsanddoes
not touchLRU. Investigateknown-visible errands andcached complete orientedtable
coverage beforea new policy; no addedtablebuilds orfuturetasks. Needinspecttask
lifecycle/costscaling carefully. No newproductionchange afterf6d0ae4.

## Persistent Fable

Session1ebb1075-3538-49d1-93d1-a00c94fa256a, Fable5.1max viaClaudeCodeCLI;
explicit source-sharing permission persists. Turns01–11 complete; turn12 RUNNING
CLI session76577, prepared19:40:31UTC, same session UUID. Source-spec ranges now
V44, includes whole task-finish function, new mixed/fractional regression slices
and PIBT macros. Payload41455B,5changed sources, hash83c7470948e6151db64130e589177d40d7a4ad1546c30683d4c7b0fecbbbe1fa.
followup-11.md gives completed negative V44 tests,16worker candidate and reversal
audit; requests structural coordination/known-next-errand and cached-chain diagnosis.
Raw runs/fable-cgar-session/turn12; archive only visible final/metadata/status and
qualified local assessment when complete. Source cache updates only at completion.
Latest finished findings/assessment still fable-flow-session/turn11/.
No rawprotocol/hiddenreasoning inGit. The12USD CLI flag has not acted asahardcap.

## Established negative evidence and guardrails

All new benchmark runs use complete prescribed work, no anytime partial outputs.
A timeout is a full-entry/run failure. Match full hashes before interpretingcrossbuild
controls. Full horizon forquality;200step screens onlyfeasibility/equivalence.
Heavy fulltrajectoryreplay onGRID. Reserve physicalcoresperprocess; aggregateRAM
mayexceed32GB but everyplannerRSSmustremainbelow32decimalGB.

Quota32seed0 collapse70043 is real and reproduced; quotaresponse nonmonotone.
Global age-aware discovery intentional; costkeytest does notsupersede confirmed64.
8/16GiB cache exactfulltrajectories137149/139037, saves197/217rebuilds without
changingguided/fallbackcounts. Gradedmargin0=56537/139796 rejected. Globalchurn
40publicationsnotpredictive ofcollapse; nofullfieldhashinproductionlog,countsnotvectorhash.
Batch128sixmean136957.33vs137073.33; batch512failsdespite100%sampledcoverage.
Pool/transactions mixed sixseeds, notpromoted. Earlier4workers+0.204% onbinary50/build32
is distinct fromcurrent64policy. Oldstrictwaitturns mixed two seeds. Weightedturn2
withoutflow lost, turn4laterdeadlinefailed; anyscaledflowinteraction isnewhypothesis.
Larger distancecoefficient256/1024 lost onearlyno-flowseed0; no knownscoringbug.
Guidereconnection/weightedguidevariants previouslycollapsed; do notrepeatsamepolicy.

Next: archivecompletedworker/fractional/reversalaudits; collect16workerconfirmation,
validate and confirm promising policies. Currentreportcheckpointd4ece2eispushed;
V43plannerexact1661176ispublic. V44source/resultscheckpointf6d0ae4isalreadycommittedandpushed. Goal remains active/unmet, not blocked.
