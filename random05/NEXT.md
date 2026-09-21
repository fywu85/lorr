# RANDOM campaign continuation

Updated 2026-09-21 19:25 UTC. The user explicitly resumed the campaign and expanded
the active objective to maximum throughput on all five RANDOM instances.
The RANDOM-05 4,000-task milestone is achieved and independently audited.
The unfinished RANDOM-03/04 goal retains thresholds 2,595 and 2,838 against
matched local NMS, with robust subsecond timing. RANDOM-03 is qualified;
RANDOM-04 remains 56 tasks short. Non-random maps are deferred.
Latest user correction: RANDOM05_PROGRESS.md remains the authoritative detailed
RANDOM-05 history; the earlier request naming random05/RANDOM_PROGRESS.md was a
typo. New root PILOT_PROGRESS.md mirrors CGAR_PROGRESS.md across all ten instances
with non-RANDOM placeholders. The accepted family name is PILOT (Pipelined
Look-ahead with Task matching). Directory and executable names are unchanged.
The internal random05/RANDOM_PROGRESS.md remains supporting density history.

## Boundaries

Edit/stage/commit only random05/, RANDOM05_PROGRESS.md, and the newly authorized
root PILOT_PROGRESS.md. Raw jobs remain in the
ignored runs/random05/ workspace. Public fywu85/lorr pushes are authorized;
preserve visibility. Other agents share main and GRID; leave their files/jobs
alone. No subagent delegation. Every map-specific choice needs the matching
--trick RANDOM-0N. Throughput is primary; track completed and censored waits.

Full horizons: 600 / 600 / 800 / 1000 / 2000. Strict 1s entry / 30s preprocessing,
32 decimal GB process guard. EPYC9354 allocation: 16 physical cores / 32 SMT
workers, actual binding and no-quota checks. Shared hosts are allowed. Fixed
work completes or the run fails; do not truncate work on a wall-clock deadline.
All inputs50001–50012,50015/50016 and50017/50018 remain excluded from tuning. R03 validation inputs50011/50012 were generated only after protocol0fb8a953 was committed.
Any further fresh validation requires freezing protocol/config before generation.

## Current resumed execution

Updated 2026-09-21 19:25 UTC. The all-five optimization campaign is active.
Continue across compactions. No subagents. Do not retry Fable without a change
in its usage-credit failure. Latest successful push: e3215efe4326c4415e28185c3488dacf3c555f91.

Selected audited records: **729 / 1408 / 2614 / 2782 / 4242**.
General records: **726 / 1397 / 1582 / 1558 / 2226**.
RANDOM-04 remains56 tasks below its retained matched-NMS target2838.

Recent qualification:
- R05/source144/88551e69:4242 exact repetition; seeds0/1/2/3 give
  4242/4028/4183/4128, +0.157% against the previous4236 profile (3positive,
  1negative), but -0.635% against the more consistent4197 profile. Largest
  entry782.280ms. All90 chronological frontier rows audited. No fresh
  qualification of4242; frozenV6 still applies only to4175.
- R04/source144:2782 exact repetition. Paired seeds0..7 give
  2749/2773/2781/2741/2782/2781/2750/2763 against
  2726/2757/2766/2742/2777/2771/2753/2753: +0.3402%,6positive/2negative.
  Extended seeds8/9/10 give2718/2777/2742; seed11 fails first entry1050.759ms.
  No fresh R04 input has been generated.
- R03/source132:2606 exact repetition; seeds5/0/3 give2606/2572/2590,
  +0.674% versus2602/2548/2566. FreshV1 qualifies2602, not2606.
- GeneralR02/source141:1397 two-order repair with1536 iterations and progress
  ties. Seven paired planner seeds beat one-order3072 by0.361% aggregate.

Fresh RANDOM-01/02 V1 is complete, audited and pushed. Protocolf74286e6 was
committed before generating50017/50018. All16 full runs pass replay and strict
resource/time checks. R01 selected730/702, general727/698 versus strongest
NMS654/630: +11.526%/+10.981% aggregate. R02 selected1382/1348,
 general1376/1338 versus1213/1198: +13.231%/+12.567%. Every input is positive.
These are new task/start streams on the same layout, not tuning data or unseen
geometry. Do not promote fresh730 over the archived-input729 record.

Active GRID work (2026-09-21 19:25 UTC):
- random03-record2614-split-full-v153, source1a3076420bc245d5f56839936d7814eb9087b050,
  ten full runs, jobs8903878–8903887, started19:21. Exact repeat, seeds0/3,
  weights1.1/1.15/1.3/1.5/2, and I7168/8192 (fixed startup6144).
- random45-smooth-field-split-full-v154, sourcea1f66070ee2d42eba95408564e2d3ebec48eded6,
 22 full runs, jobs8903889 onward, started19:22. R04 strength.025/.05/.1 and
  R05 strength.05/.1, seeds0..3 each, plus controls. R04 compact_idle=1 is
  exact in six-field checks from151; other selected settings retained.
  Build154 regression49.56s, binary71cfd49dcab7dad56c620e83274cb66865841b900f811072a338235ce80d8251.
  Submission65594 was drained. No audit sessions active. No source155 started.

All earlier batches are complete and independently audited, including150–153.
New R03 selected record2614 is promoted to random-frontiers and PILOT_PROGRESS.
Source153 heuristic weight1.2, full800steps, mean438.994/max640.097ms,RSS432MB;
record repetition/seed qualification pending above. Original weight1 gives2606;
1.01/1.05 give2583/2603. Sparse profiles preserve selected scores at1.01/1.05;
weight1.2 loses. Default controls and every fully equal trace are archived via
new reusable audit_exact_controls.py; trajectory-comparison.json also keeps
all changed trajectories, including equal-score cases if any.

Source151 exact idle-column compression: all six full runs audited. R02off/on
1408;R04seed4off/on2782,seed0off/on2749. Matching startup522->477ms. Full
maxima838->777ms and814->767ms, but means mixed. Keep unmeasured universal
runtime claims out of the logs. Source150ties has12 successful exact full traces
and R03/R04on failures1080.753/1043.654ms. Default free-tie flag remains off.
Source152query cache preserves all10 successful traces but slowsR03mean555->
580/592ms at512/2048queries. GeneralR02cache fails t2 at1003.501ms. Keep it off.

The source144 dense progress-coupling batch is fully audited, all16 attempts:
no improvement. R04contrast2.3/cutoff.8125 fail first entries1028.486/1025.277ms.
R05best variants4235/4236 remain below4242. All source145–149 failures/negative
results remain archived. The current-dense-blocked-fronts diagnostic reports
only observational adjacency; it does not imply intended movement or recoverable
throughput. Idle robots are not overrepresented in the counted front obstacles.

Held-out50001–50012,50015/50016,50017/50018 are excluded from tuning at every
density;50013/50014 remain reserved and ungenerated for R04. Existing structured
goal is stale-paused and cannot be resumed through the API. Continue the user's
authorized work without falsely completing it or creating a workaround goal.

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
