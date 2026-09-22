# PILOT campaign continuation

Updated 2026-09-22 01:02 UTC. The user explicitly requires substantial effort across all five RANDOM instances.
ACTIVE_GOAL.md is the working objective. Always headline matched max(NMS, Kitty Knight).
The stored app goal has stale03/04-only wording and old04reference2580/threshold2838;
do not falsely complete it to change its text. Current user steering governs.

## Boundaries

Only edit/stage/commit random05/, PILOT_PROGRESS.md and RANDOM05_PROGRESS.md.
Public fywu85/lorr pushes are authorized; preserve visibility. The user deferred
renaming random05/ to pilot/ until the end of development; no rename was made.
Leave other agents' files, commits, GRID jobs and locks alone. No subagents.
Fable's provider-credit failure is unchanged; do not retry without new information.

Full horizons600/600/800/1000/2000; strict1000ms entries,30s initialization,
32decimalGB process guard. Allocate16bound physical EPYC9354cores/32SMTworkers,
verify actual topology/affinity/no quota. Shared hosts allowed. Fixed work completes
or fails. Held-out50001–50012 and50015–50020 are excluded from all tuning;
50013/14remain reserved/ungenerated04inputs. Freeze protocols before fresh generation.
All tuned tricks require --trick RANDOM-0N; mark them as tricks in commits/logs.

## Current selected records and qualification

Selected739/1408/2646/2782/4254. General727/1397/1634/1616/2226.
Matchedmax692/1256/2359/2649/3172; leads+6.79/+12.10/+12.17/+5.02/+34.11%.
R04target2914still132short. Fresh03V2gain8.852%is below10%. Fresh01V1qualifies729,
02V1qualifies1408,03V2qualifies2620. No fresh739/2646/4254qualification;05V6qualifies
4175only and is NMS-only. The dashboard/density ledger use current matched references.

R01=739: source162/4fb9498e, progress.25/span32 on735anneal.25+physicalgate1,
seed1. Batchrandom1234-progress-transfer-split-full-v162,
case trick-random-01-progress-transfer-mixp25-span32. Finished2026-09-22T00:43:24.293402+00:00.
Mean73.552/max114.527ms,RSS388.555MB. Exactrepeat6fieldpasses. Five qualification
seeds0..4=736/739/726/732/732 versus732/735/725/729/730:3665vs3651 (+.3835%,all5positive),
max118.656ms. Report random01-record739-split-full-v162. Earlier physicalgate alone
+.690%all5positive; annealing alone+.027%3gains/2losses. No fresh current-profile claim.

R03=2646: source162/4fb9498e, progress.25/span32 on2621gate1profile,seed3.
Batchrandom1234-progress-transfer-split-full-v162,
case trick-random-03-progress-transfer-mixp25-span32. Finished2026-09-22T00:53:28.091236+00:00.
Mean484.103/max676.507ms,RSS469.467MB. Full replay passes; off2621control6fieldexact.
Intermediate2628chronologicalrecord also logged. Exactrepeat/paired qualification pending below.
Preceding2621fivepairs13029vs12999 (+.231%,4gains/1loss);2620freshV2remainsseparate.

R05=4254: source162/4fb9498e, flow80updates on4242profile,seed0.
Batchrandom345-flow-iterations-split-full-v162/casetrick-random-05-flow-iterations80.
Finished2026-09-22T00:21:04.224019+00:00; mean579.781/max874.301ms,RSS490.107MB.
Longestcompletedorder1973,147initialunfinished/109unopened,censoredoldest2000.
All91chronologicalrecords/waitingmetrics pass. Exactrepeat6fieldpasses. Pairedseeds0..3
4254/4189/4252/4253 vs4242/4028/4183/4128 yield16948vs16581 (+2.2134%,all4positive),
mean4237.0,max919.594ms,RSS494.387MB. Also+1.5641%above historical4197profiletotal16687.
Report random05-record4254-split-full-v162. This is development-input qualification.

R02=1408source132/027df4d9; general1397source141/611aa2a6 remains.
R04=2782source144/88551e69; mean478.686/max739.553ms. Exactrepeat plus8pairedseeds
+.340%6gains/2losses, with a preserved laterseed11first-step1050.759mstimeout.
General04=1616face8source158 repeats but five-pair mean-.101%; general auction1595
has+4.312%all5pairspositive. All full recipes/evidence are in random-frontiers.json.

## The only active batch

random03-record2646-split-full-v162: five full800step cases, seeds0..4.
Jobs8920753/8920754/8920755/8920756/8920757, frozenbeforelaunch in c511babe.
Seed3repeats2646. Same-source historical2621-profile controls2604/2595/2600/2621/2609
were predeclared in random03-record2646-v162-protocol.json. All strict1s/32GB;
selectingseed3is included, no fresh input. Audit all5withsource162/build162,
run declared exactrepeat, report five paired totals and preserve any failures.
Promote only independently audited higher selected seeds. No source164 exists.

## Completed batches since the last checkpoint

- random1234-progress-transfer-split-full-v162: all12audited, all3declared controls6fieldexact.
  01=735/739/739,02=1408/1405/1403,03=2621/2628/2646;04=.125/span32:2766,
  .25/span16:2768,.25/span128:2777 vs historicalsame-source2782. All04alternativeslose.
  Complete REPORT.md; original partial01/03controlproofs explicitly retained as partial.
- random345-flow-iterations-split-full-v162: all12audited,3controls6fieldexact.
  Iterations20/10/40/80:03=2621/2527/2600/2602;04=2782/2570/2761/2771;
  05=4242/3864/4193/4254. Keep20on03/04; only05flow80promoted and qualified.
- random123-failure-first-split-full-v163: all12audited,5controls6fieldexact.
  General01control727,matched720/full723; trick01control735,matched731/full733;
  general02control1397,matched1389; trick02control1408,matched1383;
  trick03control2621,matched2582. All candidateslose; optional3ordersstaysOFF.
- 739/4254qualification batches: all5/all7audited, exact repeats6fieldpass, pairedreports complete.
- Earlier source162physicalgate10valid+2allocationrefusals allpreserved/audited;
  sparseanneal16allvalid/4exactcontrols;735/733/2621qualification complete.
- Earlier source159startup-capalllose; general04facepairedmeannegative. Source158face,
  159revisit,160prefix-search and153weighted sparse trials retained all negatives/timeouts.

## Source and execution

Source162=4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad/build162,
SHAfda9057b35c275b4e7efeaa6394f321c39902c76c09d9ce16fd60386a8e3e556, regression54.99s.
Source161failed only its new horizon18vs150step fixture;162fixed that fixture without
changing production source/assertions. Never benchmark failed161; retain original evidence.
Source163=d323e605a5bcdc2cc7f5e3a5864f89fb6f6351c6/build163,
SHAeb99ff7049a293ba22b8c901e403fb88e690211a74df60cfa3832407f0e25ddd, regression62.13s.
Adds optional third repair order; default1 unchanged. Source163benchmarks allnegative.

Hosts research35|research36|research37|research39|research46|research47|research48|
research50|research51|research52|research55|research56; exact CPUmodel
AMD EPYC 9354 32-Core Processor. Exclude49/54binding refusals, older34/40/42/45;
53/58have earlier timing concerns. Do not alter other jobs or infer hostcauses.

Shell requires escalation because the sandbox namespace fails; Python3.7default,
env/bin/pythonforNumPy. apply_patch unavailable; safe Pythonheredocs. Only scopedgit
operations; never reset/amend shared work. Latest successful push9092d222;
subsequent scoped commits needpush. Publicpushuses existing gh credentialhelper.

## Next work after qualification

Keep meaningful effort across allfive. The dated current-bottlenecks-v162snapshot
shows little sparse loadedwaiting/detouring and much more on04/05; it is observational,
not causally recoverable work. Candidate-bound rejection in window repair is only a
possible runtimehypothesis: prove lowerbound/FPsafety, exactrollback, and handling of
annealing/component salvage/thirdorder before implementation. No speedup claimed.
The739/2646tricks and4254field now need independent fresh-input qualification after
configuration/protocol freeze, if chosen as the next validation step; no new stream
has been generated. Do not repeatedly draw heldouts just to cross a threshold.

## Previous stopping checkpoint (2026-09-21 13:25 UTC; historical)

No PILOT GRID jobs are running or queued. All declared cases through build132
have completed and their results are audited. Other agents' CGAR jobs are
outside this session's scope and remain untouched. No source133 implementation,
new experiment, or fresh RANDOM-04 input was started.

Latest compiled source: 027df4d9 / build132, full regression passed in38.30s.
Binary SHA175466cf2a3ee9bcb2e62905f1fffa8173dc66a2bab9c4d71f11914f22fe4801.
Default-off joint proposals and rematching did not improve the selected profile:
- build131: unchanged control2776; one/two proposals2775/2773; lowerK6144=2721.
  The unchanged control matches the archived2776 run in all six trace fields.
- build132: zero/one/two/four repair rounds2773/2776/2776/2776. Every run is
  strict-valid and independently replayed; largest observed entry699.938ms.
  Zero repairs matches build131's2773 in all six fields. The three2776 repair
  runs differ from the original2776 trajectories despite matching its score.
- build130: exact2776/2764 controls; first-decision arrival alternatives
  2727/2699/2728/2740. Keep all arrival and joint-move options disabled.

The source124 record2777 profile now has eight planner seeds0..7:
2726/2757/2766/2742/2777/2771/2753/2753. All pass replay and strict timing,
with the largest entry790.204ms; exact seed4 repetition is retained.

Source119 depth12/20=2756/2753; noise100/150/300/400=2707/2738/2724/2723;
turn0.5/0.75=2739/2734; wait0.25/0.75=2754/2685; K4096/5120/6144=2661/2703/2757.
Source129 arrival bonuses all lose2764/2734/2745/2732; control2776 is exact.
All completed results, lower-work alternatives and failures remain recorded.

The approved Fable summary was submitted through the existing CLI session,
but the provider returned a usage-credit error and no review. Approval is
resolved. Do not retry without a change in the credit condition.

If resumed, a possible next hypothesis is to combine independently legal local
components from a joint proposal with the best PIBT plan. This is only an idea:
no source, benchmark or performance claim exists for it. Reassess against the
negative whole-proposal evidence before implementing. Fresh RANDOM-04 inputs
50013/50014 stay reserved until a qualifying configuration is frozen.

## Current records and evidence

General RANDOM-01..05: 726 / 1390 / 1582 / 1558 / 2226.
Trick RANDOM-01..05: 729 / 1408 / 2602 / 2777 / 4197.
Matched NMS32: 649 / 1228 / 2359 / 2580 / 3172.
Published max(NMS,KK):688/1260/2334/2547/3050. Selected gains versus that
published orientation reference are5.96%/11.75%/11.48%/9.03%/37.61%; these are
not matched-machine comparisons. All records have independent full replay and
resource/waiting checks. One layout at five densities is not unseen-layout transfer.
Exact cases:random-frontiers.json. Consolidated ledger:RANDOM_PROGRESS.md.

RANDOM-05: source027df4d9/build132, case trick-random-05-record4175-coupling-triage1,
batchrandom05-record4175-coupling-split-full-v132, finished2026-09-21T16:24:44.415657Z.
Seed0, activecap680/fastadmission1, first7968/K16320/B18/s2/q4/G4/E8/P8,
movebias3/fraction0.25, field15/flipseed5/oneflip/contrast2.4,
triage1/mix1,turn0.6/wait0.5. Mean569.038/max802.896ms; peakRSS490.828MB.
Longest completed order1977steps;150initialordersunfinished,113unopened;
oldest unfinished censored2000. Exact repeat and seeds1/2/3 are running.
Previous4175has exact repeats and frozenV6qualification;4011sourcea2ff2b2/build80
retains its exact repeats and freshV5qualification. Neither qualifies4197.

Four-core record3770 stays separate: source5f81613/build65, seed3,
first4608/K5760/B12/cache512/triage1.5, mean788/max845ms, RSS485MB.
Matched NMS4=2914 (+29.4%). Earlier fresh V4 gives +25.42%; it is not evidence
for the latest 32-worker configuration.

## Completed frozen V5 validation and 4,000 milestone

Protocol e46f57b committed 03:56:06 UTC, before generation 03:56:45 UTC.
Candidate4011 and previous3990 use the same source/binary; only cutoff mix
changes0.75 -> 1. All eight original runs finish without errors/timeouts.

Seed50009: candidate3912 / previous3901 / NMS3095,3090.
Seed50010: candidate4047 / previous4023 / NMS3177,3190.
Aggregate candidate7959 vs stronger NMS6285 (+26.6348%) and previous7924
(+0.4417%, positive on both inputs). Fresh4047 is a different input; the
archived frontier stays4011. Candidate mean573/560ms, max624/644ms, RSS<560MB.
Two fresh task/start streams provide limited replication on the same layout.

FRESH_VALIDATION_V5.md and results/fresh-validation-v5/{audit,replay-audit}.json
contain the full frozen comparison. results/completion-audit-4000/audit.json
passes ten requirements. Supporting proofs include all starts/task chains on
10 traces, 35 compiled/test source files, actual CPU/AS guards, frozen config
difference, unchanged held-out hashes, 52 unsuccessful attempts and 6 failed
builds. results/trick-gates-v80/audit.json tests the actual frozen executable.
All 85 main frontier rows and waiting metrics are checked; separate manifests
and all user-facing logs are current. The registered goal was marked COMPLETE
at05:07 UTC after the evidence and PILOT dashboard were committed/pushed in
a87f097. Reproduce the historical milestone auditor at its archived commit; its checks pin the then-current best manifests. The broader RANDOM
campaign remains authorized; the goal completion does not claim all maps solved.

## Retained RANDOM-03/04 goal and qualification

R03 archived threshold and frozen fresh qualification are COMPLETE. Current
record2602, source02bccaa7/build102, batchrandom03-merge-coupling-split-full-v102,
case trick-random-03-merge-coupling-triage0875. H20/keep14/I6144/group6,
32islands/2rounds/window_merge1/heap4=1/fastgroups1/storage1/temperature0;
field15/flip5/contrast2.4,turn2/wait2,seed5,horizon800/triage0.875/hop-onlymix0.
Originalmean507/max710ms; exactrepeat2602 max661ms; seeds0/3=2548/2566,
max655/674ms. All full800steps/replayed. No claim all seeds exceed10%.

Protocol0fb8a953 committed07:55:00UTC before generation07:55:03.289264.
Fresh50011/50012: candidate2599/2557, previous2580 recipe2590/2535,
NMS2318/2327 and2343/2322. Individualgains11.69%/9.13%,aggregate10.41%.
All8valid/replayed; candidatefreshmax700/676ms. Never tune on50001–50012.
RANDOM03_FRESH_VALIDATION_V1.md contains full source/timing/wait evidence.
The auditor checks actualNMSplannerTimes versusPILOTentryComputeTimes, rather
than requiring PILOT-onlyentryComputeSamples from the unmodified reference.

R04 CURRENT **2777**, sourcecaedcce7/build124, batchrandom04-triaged-credit-split-full-v124,
case trick-random-04-triaged-credit-horizon16-price32. This adds matchhorizonweight16
and optionalidleprice32 to the2776 profile; admissioncredit remainsOFF. Full1000steps
independently replayed; mean469.540/max779.602ms, RAM697MB. Onlyone extra task, not
a substantial win; +7.64% versusNMS2580,61shortof2838. Exact repetition passes all6fields,max770.041ms.
Longestcompleted998,initialunfinished175/unopened157,oldestunfinishedcensored1000.

R04 preceding **2776**, source487a35da/build119,
batchrandom04-admission-guidance-split-full-v119,
case trick-random-04-admission-guidance-contrast2p2.
Full independently replayed1000steps, mean434.778/max626.736ms.
+7.60% vs NMS2580, still62shortof2838. Seed4; cap560, triage0.875/mix0.75,
dispersion0.4, fieldoutputcontrast2.2, shared8Borders1, fastadmission1.
All other B10/K8064/first4032/depth16/G4/E8/P8/field15flip5 settings retained.
Four-seed timing at contrast2.2/2.6 is complete; no fresh R04 inputs.

Previous2762 profile (contrast2.4) improves allseeds0/3/4/5 over dispersion0.8:
2725/2751/2762/2772 vs2719/2710/2741/2759. All initialrunsmax<636ms;
exact seed4 repetition is slower, mean521/max729.537ms, preserved in evidence.
The2772 seed5 record is retained in the historical ledger. Original2741profile
repeats exactly on119and121 (all6fields); newcache/matching traceproofs pass.

Preceding2738 usescap560/triage1; fourseeds0/3/4/5=2718/2689/2738/2735,
all originalsourcequalificationmax<671ms. Full controlrepeat116and119/120
runtimecontrols matchall6tracefields (largest119controlmax684.333ms).
Prior2698cap560/triage1.25 scores2701/2654/2698/2705; admissionimprovesall4seeds
againstuncapped2661profile2542/2576/2661/2605. Allsourcecontrols andfailuresstay.
Prior2661hasaslowerexact113controlmax755.061ms, no hostcauseinferred.

Config: B10/K8064/first4032,depth16/G4/E8/P8,screen2/keep4,rootmutation0.1,
noise200,sharedrankings2048MiB,keepbonus0,field15/flip5/contrast2.4,
turn0.6/wait0.5/dispersion0.8,horizon1000,guidedmatch/length0.25.
Openedtasksstaylocked; all700robotsremainmovable. Explicitadmissiontrick.
No freshR04inputexists. Reserve50013+onlyafter freezingqualifiedcandidate.

HORIZON CHECK FIX: makespan is maxassigned-workcost, NOT executedsteps.
Cap480's oldrunnerrejectedmakespan976 althoughall700pathsand1000entrysamples
cover1000stepswithnotimeouts. Newindependentreplayconfirms2631tasks; original
invalidsummaryremainsunchanged, supplementalcap480-horizon-correction.json
recordsfullproof. Correctedrunner/auditors use actualpathcounts andentrysamples,
retainreportedmakespan,andrejectuneven/truncatedpaths. Sixhelpertests pass;
all10existinggeneral/trickfrontierhorizonsremainunchanged. Thehelperisfrozen
andhashedwithnewGRIDrunners. No benchmarkdata,binaryorinputwasrewritten.

CURRENT SELECTED RECORD SOURCEcaedcce7/build124, regression33.72s, SHAd43d3708b526521317df8159dcaf64131be38e872e1a039d4d846489cb1c8237.
Active-onlytravelratecalibrationdoesnotbeat2738control; leaveoff.
Source118/ca22bb7c exact8byteordercache, fallback32Bwhenroutingbiasneedscores.
Source119/487a35da exactdummy-prefixHungarianoptimization. All6cacheand3matching
fullcontrolsproved6tracefieldsidentical. 2698RAM1.73GB->0.57GB; fastmatching
controlmean427/max619ms. Bothenabledinnewexperiments, noscorebenefitclaimed.

Allbatches throughactive-cutoff116,2738seeds116,active-rate120,
admission-match119andcorrectedpredict119closed/audited. Active-ratevariants
2733/2737/2714/2659 lose2738; matchinglength0/.5/1/2=2297/2691/2712/2693;
correctpredict2715. Original-predictcasewasmisconfiguredR05_PREDICT_MATCHING,
ignoredbybinaryandexact2738control; preservedwithconfiguration-note.json.
CorrectkeyR05_SCHED_PREDICT. NewmanifestlintrejectsunknownR05optionsagainst
frozenbinarysource;3tests pass. Nooriginalartifactchanged.

CLOSED/AUDITED through admission-coupling119,2741seeds119,dispersion119,
priced-admission121,guidance119,objective119. Guidance2.2=2776,2.6=2770;
1.2/1.6/2.0=2675/2706/2715. New task rewards all lose2721/2716/2729/2714/2725/2738.
Price12/16/20/24/32=874/1587/2627/2740/2741; uncapped24=2658. Priceoffcontrol
andprice32 preserve2741 exactly. Price mechanism remains off. Semantic tests
passed34s; newly cheaper idle slots can starve the pool at low prices.

CLOSED/AUDITED: nonlinear122 loses2617/2692/2718/2758, exactdefault2762.
Fieldseeds119 at2.2 give2730/2711/2776 on0/3/5, maxima<630ms; contrast2.2
best2776 improvesselectedbest butnotaggregate versus2.4. Cutoffvariantsalllose.
Immediateperiods1/2/4/8=2732/2720/2724/2726. LargerworkK11520/13824/B12K11520
=2771/2762/2771. OriginalB8K8192 initializationrejected(error125), nottimedout;
K8192notdivisibleby4*14screeningwork. Originalpreserved, noteinexperiments.
CorrectedB8K8064=2710, fullyaudited. Newlintchecksfullscreeninggroups;4tests.

STARTUP123 CLOSED/AUDITED: control2776, oldprice12until100/150/200/250 gives
2690/2661/2630/2547, newprice12until150/200 gives2710/2666, new16until300 gives2703.
Early gains do not survive the full run. All4oldcases match theconstant-price12
reference exactly through expiry in all6fields; firstactiondifference isexpiry+1.
The2776control123 is6fieldexact. Keepstartuppriceoff; no positive claim.

CLOSED/AUDITED: random04-triaged-credit-split-full-v124. Control2776 matchesall6fields.
Credit0.25/0.5/1 loses2766/2757/2743; fullcredit+horizon4/16+price32 gives2767/2762;
fullcredit+horizon16withoutprice2693. Credit staysOFF. Horizon16+price32only2777.

CLOSED/AUDITED: pairing124, original2776repeat119 and unifiedR03/R05controls124.
All6pairingvariantslose2705–2760. Exact2777repeat max770.041ms; original2776repeat
max622.264ms; R03/R05currentengine max671.719/572.148ms. All4controls exact6fields.

CLOSED/AUDITED: all7goal-local125cases and3additional2777seeds124.
Localradius2mix.25/.5/1=2753/2725/2692; radius4=2719/2713/2665. Control2776
is6fieldexact. Keepgoal-localoff: no reductioninnear-goalawaymoves oncommonorders.
2777profile seeds0/3/4/5=2726/2742/2777/2771; everyoriginal/repeat/seed max<789ms.
Still61shortof2838. No freshR04input has been generated.

CLOSED DIAGNOSIS: build126/source1c9c8b95 read-only goal_probe replayspasses.
First700steps near-goalaway:2781total,102staticmin,203kinematic-onlyminimum,
2476(89.03%)nonminimumeligible. Interaction/cycles/suppressedgoalspossible, not
causalwasteproof. IndependentPythonloaded-forward/awaycounts agreeexactly.
Artifactsrandom04-goal-score-diagnosis-v126. Frozen126tools hashesverifiedagainst
latercommit afterconcurrentgitindexlock; originalspecpreserved.

CLOSED: source127 displacement trials and source119 route trials all lose.
Source127 low-work slow/fast2559 controls match all six fields. Source128
optimized2559 also matches; its fullK8064 push0.25 run failed at step1,
1027.906ms/exit124. Preserve this real deadline failure. LowerK6144 pricing
scores2736/2695/2723/2654, all valid but below the record. No pending audit.

RECENT NEGATIVE EVIDENCE:
- Horizon matching108/d879feda weights.25/1/4/16:2602/2573/2575/2585 vs2661.
- Startup priority109/7714e8ca (shorter remainingwork) all variants2522–2630
  vs2661. Includes200/100step bounds andallrun preference; leave off.
- Loop/dispersion/noise refinements96 all2496–2601 vs2661.
- 32 new fields101–132 atK1024 alllose inherited2361 (best109=2324).
  Declaredtop4 full-work verification109/111/105/115 onplannerseeds4/0 also
  loses:2526/2490,2481/2490,2494/2567,2433/2489 versus2661/2542. No new field
  promoted. Allfull1000/replayed; archive selectionprotocol andoriginalattempts.
- Morecompletework+sharedcache+IPO100/03b68098:K11520/13824/16128 gives
  2615/2592/2615,allstrict,so extra compute didnothelp.
- Three-stepoperationtransfer96:fivevalid1733–1936;K256 timesout atstep74,
  18562.600ms. Preservefailure; do not attribute a host cause without evidence.
- Feasiblemoveproposal107/ae18871e:all6lose2531–2615,exactdefault2641.
- Rootdiversity106/75723ed8:all7lose2447–2605,exactdefault2641.
- Destinationprices105/aa370473:all4lose2540/2515/2543/2566,exactdefault2641.
- Cachelayout104/1436f231:four full2641exactcontrols, no compelling measured
  alignment/lazy-row win. Original104/105jobnameswithoutcpufix were refused
  beforelaunch because I supplied abbreviatedCPUmodel; preserved/audited.
- Older guidance reversal/field flips, replan forecasts, component crossover,
  score weighting, window scaling also lose; detailsinRANDOM34_CAMPAIGN.md.

FABLE REVIEW APPROVAL RESOLVED at13:11UTC: user explicitly approved the unchanged
6022-byte results summary at runs/random05/fable-random04-diagnosis-02/payload.txt.
SHA493ff3fb8a05766e037127300fcf4489196c8bea2c506a2c00b0ed16d9a70bd5 was verified,
then CLI resumed session27a4316e-b79d-46cf-86b4-41b0f558938a with Fable5.1/max,
no tools and12USD cap. Provider returned OUT OF USAGE CREDITS; no review arrived.
Status/response/completion are preserved in that ignored directory. This is now
a provider-credit issue, not a pending approval or automatic-review rejection.
Do not retry unchanged credit failure or send new unapproved payloads. The
optional review does not block a future local continuation, once requested.

This stopping checkpoint is to be committed and pushed to public fywu85/lorr
main, with the resulting hash reported to the user. Only random05/,
PILOT_PROGRESS.md and RANDOM05_PROGRESS.md are in scope. Exact Fable payload
and response remain in ignored runs; neither is published or rerouted.

## Rejected source experiments / tool notes

Source86 raises only the explicit WINDOW_ITERS ceiling2048->8192, default
unchanged. Full regression and exact six-field2439 control passed. The older
source84/85 retained-prefix proposals failed the unchanged dense mobility
floor (39/38moves,required>150); patches/logs preserved and source restored.
Do not weaken the test or silently resurrect the approach.

Audit: tools/audit_random_cases.py --batch NAME --build build-vNN --source HASH.
Promote R01–04 with tools/promote_random_records.py AUDIT; R05 also requires its
main frontier, best manifests, waiting manifest, and full history audit.
R01–04 promotion now refreshes PILOT_PROGRESS.md automatically. After a new R05
record, also run tools/render_pilot_progress.py. Published and matched-local NMS
comparisons must stay separate; missing maps keep their placeholders. Archive
small direct/one-level spec/allocation/submission/summary/completion/batch JSON,
never raw traces or NMScwd. Split raw traces live at BATCH/CASE/CASE/result.json.

Fable persistent session27a4316e-b79d-46cf-86b4-41b0f558938a failed provider credits;
no new feedback. The new6022-byte payload was explicitly approved and submitted but also hit
usage credits. Do not retry unchanged quota or send new unapproved payloads. Shell requires escalation due namespace failure.
Python3.7; env/bin/python has NumPy. apply_patch unavailable; use safe heredocs.

Only scoped git operations; never reset/amend others' work. Public push:
env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main
Check git log before assuming the current HEAD; other agents commit independently.

Hosts: research32|research33|research35|research36|research37|research39|
research41|research46|research47|research48|research49|research50|
research51|research52|research54|research55|research56. Avoid34/40/42/45 binding refusal
and53/58 timing spikes; causes unproven. Actual allocation guards are mandatory.
