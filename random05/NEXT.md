# RANDOM campaign continuation

Updated 2026-09-21 10:33 UTC. The registered 4,000-task RANDOM-05 milestone is
achieved and independently audited. A NEW ACTIVE goal now targets RANDOM-03
>=2595 and RANDOM-04>=2838, at least10% above matched NMS, with robust
subsecond timing. See RANDOM34_CAMPAIGN.md for qualification rules. Broader authorized work develops general
improvements and explicit tricks for RANDOM-01..05. Non-random maps are deferred.
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
All inputs 50001–50012 remain excluded from tuning. R03 validation inputs50011/50012 were generated only after protocol0fb8a953 was committed.
Any further fresh validation requires freezing protocol/config before generation.

## Current records and evidence

General RANDOM-01..05: 726 / 1376 / 1582 / 1558 / 2226.
Trick RANDOM-01..05: 628 / 1122 / 2602 / 2741 / 4011.
Matched NMS32: 649 / 1228 / 2359 / 2580 / 3172.
Selected bests versus NMS: +11.9% / +12.1% / +10.3% / +6.24% / +26.5%.
All records have full source/input/resource, independent replay and waiting
checks. One archived layout at five densities is not unseen-layout transfer.
Exact cases: random-frontiers.json. Consolidated ledger: RANDOM_PROGRESS.md.

RANDOM-05: source a2ff2b2 / build-v80, case 32-record3990-mix1,
batch record3990-coupling-split-full-v80, finished 03:50:40.471914 UTC.
Seed0, first7968/K16320/B18/s2/q4/G4/E8/P8, move bias3/fraction0.25,
field15/flip5/contrast2.4, triage1.25/mix1, turn0.6/wait0.5.
Windowed search, startup weighting, and finalist rescoring are off.
Mean534/max622ms; RSS559MB. Archived record4011 repeated exactly in all six
trajectory/schedule/event/task fields. Longest completed order1944 steps;
130 initial orders unfinished, 86 unopened; oldest unfinished censored2000.

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
a87f097. Reproduce with tools/audit_4000_milestone.py. The broader RANDOM
campaign remains authorized; the goal completion does not claim all maps solved.

## Active RANDOM-03/04 ten-percent work

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

R04 CURRENT **2741**, sourceaa171530/build116,
batchrandom04-active-cutoff-split-full-v116,
case trick-random-04-active-cutoff-scalep875.
Finished2026-09-21T10:14:35.787464UTC, mean450.058/max649.058ms.
Fully replayed/promoted; +6.24% vsNMS2580, still97shortof2838. Seed4;
cap560/triage0.875/mix0.75, otherwise prior2661profile. Longestcompleted997;
178initialordersunfinished,134unopened; oldestunfinishedcensored1000.
The current case needs seed qualification. The coupling batch repeats it on119.

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

CURRENT SOURCE1b3f1798/build120, SHAca7f13de8c8e062a722bd27417c43f61953f53d159b3e4f10a5d82eeb3387e3f.
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

ACTIVE:random04-admission-coupling-split-full-v119, jobs8902605–12:
2741exactcontrolwith8Bcache/fastmatching, dispersion0/.4/1.6,turn.4/1,wait1,
chainmatching. Allfull1000steps/strict1s. Thenimplementcost-sensitiveadmission
asaseparateexplicittrick; fixedcapcurrentlymustfillallavailablecapacity.

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

FABLE QUESTION PENDING: automatic approval review rejected sending a new6022byte
results-only summary (no newsource) to the existing externalFable session.
NOT SENT. Exactpreparedfile runs/random05/fable-random04-diagnosis-02/payload.txt;
SHA inprepared.json. User wasasked throughrequest_user_input_async toapprove
this payload/destination. Do notsendorchangeit unlessapprovalarrives. Continue
localwork; this doesnotblockthegoal. LastactualFablecall21:55UTCpreviousday
failedusagecredits. Prior79KB approvaldoesnot cover thisnewpayload according
toautomaticreview. Preserve rejection explanation in any final response while
unresolved. Readstatus.json inthatdirectoryforcontext.

Lastverifiedpublicpushc302547d. Laterownedcommits/evidence needpush. Only
random05/,PILOT_PROGRESS.md,RANDOM05_PROGRESS.md may becommitted. Fablepayload
stays inignoredruns; do not publish or reroute it to bypass the rejection.

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
no new feedback. Approved79KB payload is unchanged. Do not retry unchanged quota
or send new unapproved payloads. Shell requires escalation due namespace failure.
Python3.7; env/bin/python has NumPy. apply_patch unavailable; use safe heredocs.

Only scoped git operations; never reset/amend others' work. Public push:
env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main
Check git log before assuming the current HEAD; other agents commit independently.

Hosts: research32|research33|research35|research36|research37|research39|
research41|research46|research47|research48|research49|research50|
research51|research52|research54|research55|research56. Avoid34/40/42/45 binding refusal
and53/58 timing spikes; causes unproven. Actual allocation guards are mandatory.
