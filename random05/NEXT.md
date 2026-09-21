# RANDOM campaign continuation

Updated 2026-09-21 08:15 UTC. The registered 4,000-task RANDOM-05 milestone is
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
Trick RANDOM-01..05: 628 / 1122 / 2602 / 2661 / 4011.
Matched NMS32: 649 / 1228 / 2359 / 2580 / 3172.
Selected bests versus NMS: +11.9% / +12.1% / +10.3% / +3.1% / +26.5%.
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

LATEST OVERRIDE (08:38UTC): R04 **2661**, sourcefa98fd1d/build96, batch
random04-triage-refine-split-full-v96, case trick-random-04-triage-refine-mix075.
Same2641 configuration below exceptR05_TRIAGE_GUIDED_MIX=0.75. Mean430.374/
max465.929ms, full1000/replayed/promoted. 177short of2838; qualificationpending.
R03 frozenfresh qualification remainscompleteat+10.41%aggregate.

Source109 startupremaining-workpriority build pending; source108/d879feda
horizonmatching build passed. Source107/ae18871e feasiblemoveproposal modes
passedtestsbutall6fullvariantslost2531-2615; exactdefault2641. Source106 all
rootdiversityvariantslost2447-2605, exactdefault2641. Sources104/105 fully
closed/audited/exactcontrols. No new runtime optimization selected from104.

ACTIVE at08:38UTC:
- random04-horizon-match-split-full-v108 (5), sourced879feda,2661control and
  weights0/.25/1/4/16. Jobs8902265-69.
- random04-layout-finalists-split-full-v96 (10), sourcefa98fd1d. Fields109/111/
  105/115 andinherited control, eachplannerseeds4/0. Jobs8902307onward.
- build-v109 startuppriority preference; recovercommitwithgitlogsrc.
Full33-case layoutscreen allvalid, bestnew2324vscontrol2361;audit session2130
pending. Selection follows predeclaredprotocol andisnotchangedbasedonnewdata.
Sharedwork3casesallvalid2615/2592/2615, audit session4748pending. Operations6
closed/audited: onegenuine18562.600ms timeout atstep74, fivevalid1733-1936.
Feasibleproposals7closed/audited. Exactproofsfor106/107written. The priorstatus
below describes earlierqualificationandprofiles; use thisoverrideforcurrentjobs.
Lastverifiedpublicpush43572e0f; laterownedcommitsandevidence needpush.

R03 **2602**, source02bccaa7/build102, batchrandom03-merge-coupling-split-full-v102,
case trick-random-03-merge-coupling-triage0875. H20/keep14/I6144/group6,
32islands/2rounds/window_merge1/heap4=1/fastgroups1/storage1/temperature0;
field15/flip5/contrast2.4,turn2/wait2,seed5,horizon800/triage0.875/hop-onlymix0.
Mean507/max710ms/RSS434MB. Finished07:48:41.498854UTC, independently replayed.
Archived repeat2602 exactlymatches all six fields. Seeds0/3=2548/2566;
qualificationrepeat/seeds max661/655/674ms. R03 archived threshold crossed.

FROZEN R03 validation COMPLETE: protocol0fb8a953 committed07:55:00UTC before
generation07:55:03.289264. Inputs50011/50012; candidate2599/2557, previous2580
recipe2590/2535, NMS2318/2327 and2343/2322. Gains11.69%/9.13%, aggregate10.41%.
All8valid and independentlyreplayed; candidatefreshmax700/676ms. Do nottune
these inputs. RANDOM03_FRESH_VALIDATION_V1.md contains source/timing/wait evidence.
The audit now checks actualtimingseries: NMSplannerTimes vsPILOTentryComputeTimes;
it no longer requiresPILOT-specificentryComputeSamples fromunmodifiedNMS.
Protocol/source/configurations unchanged. OverallgoalACTIVE: R04notattarget.

R04 **2641**, sourcefa98fd1d/build96, batchrandom04-matching-refine-split-full-v96,
case trick-random-04-matching-refine-keep0. B10/K8064/first4032,depth16/G4/E8/P8,
screen2/keep4,rootmutation0.1/noise200/seed4,sharedrankings2048MiB,keepbonus0;
field15/flip5/contrast2.4,turn0.6/wait0.5/dispersion0.8,
horizon1000/triage1.25/mix0.5,guidedmatching/length0.25.
Mean441/max498ms/RSS1551MB. Completed07:11:31.028236UTC,fullyaudited.
Seeds0/1/3/4/5=2526/2601/2596/2641/2628; everymax<498ms. Exactprofile/control/
IPO/32and16workerscompareall6fields. Reduced16workersmean680/max861ms.
197tasks short of2838. No freshR04inputgenerated; reserve50013+onlyafterfreeze.

Current source106/75723ed8, buildpassed08:12:32UTC,
SHA5a743c32b2fce8a4193176d83d7996fc4321cde2e3414376e7ed9928c7cacab3.
Configurable restartperiod(default4;0disables), elitenext-decisiondifference
(default0). Source105/aa370473 optionalR05_DESTINATION_LOAD (openedvisible
orders only); source104/1436f231 sharedranking64bytealignment and optional
R05_LAZY_COST_ROWS. Defaults preservebehavior; full exactcontrols pending.

ACTIVE at08:16UTC:
- random04-cache-layout-cpufix-split-full-v104:4cases,source1436f231.
  shared2048/private0 x eager/lazy; expectexact2641. Jobs8902193-96.
- random04-destination-load-cpufix-split-full-v105:5cases,sourceaa370473.
  control andcoefficients1/4/16/64. Jobs8902197-2201.
- random04-root-diversity-split-full-v106:8cases,source75723ed8.
  control/restart0/2/8/16/elitefraction.01/.03/.1; jobs8902214onward.
The corresponding non-cpufix104/105batches were refusedbeforelaunch solely
because I supplied the abbreviatedCPUmodel. Archived/audited; no solver score.
Useexact --cpu-model 'AMD EPYC 9354 32-Core Processor' goingforward.

All older batches throughsource103 plusR03merge-budgetv100 arefullyclosedand
audited. Lastsevenmissingfull6-fieldexactproofs nowwritten. Runtime report
contains28rows includingR03qualification/fresh andR04five-seedtiming.
Morewindowrepairs2579/2558/2556 do notbeat2580. Guidanceexpansion14variants
alllost; weightedwindow2466-2571; merge-round/horizon2532-2579. R04visible-task
forecast2511; otherclosedloopforecastsloseortimeout. SeeRANDOM34_CAMPAIGN.md.

Last verifiedpushc332609a; laterownedcommitsneedpush. Freeze/ref/protocoland
sourcecommits areonmain. Onlyownedscopesmaybestaged/committed. KeepR05unchanged.

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
