# RANDOM campaign continuation

Updated 2026-09-21 06:13 UTC. The registered 4,000-task RANDOM-05 milestone is
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
All inputs 50001–50010 remain excluded from tuning. No 50011+ input was generated.
Any further fresh validation requires freezing protocol/config before generation.

## Current records and evidence

General RANDOM-01..05: 726 / 1376 / 1582 / 1558 / 2226.
Trick RANDOM-01..05: 628 / 1122 / 2551 / 2565 / 4011.
Matched NMS32: 649 / 1228 / 2359 / 2580 / 3172.
Selected bests versus NMS: +11.9% / +12.1% / +8.1% / -0.6% / +26.5%.
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

The goal remains ACTIVE. Targets2595/2838 plus robust subsecond qualification.
R03 now2551 (8.14% overmatchedNMS2359), source dabfcc7/build89, case
trick-random-03-cooperative-triage-triage100 in
random03-cooperative-triage-split-full-v89. Full800steps,mean520/max698ms.
Samecooperative2514profile + horizon800/scale1/hop-only estimate. This known
horizon is an explicit trick. All6triage cases full/audited; scales0.45/0.75/1.25
=2529/2538/2514,scale1/mix1=2545. Newrecord independently replayed/promoted.
No freshR03/04 input generated yet; keep50001–10 untouched.

R04 record2565 still sourcea2ff2b2/build80 depth16/K14400/B18/seed4,
mean832/max949ms. Oldseeds0/3 andsource87control/fused have realtimeouts, so
this profile is NOT robustly qualified. No claim that hostcontention caused them.
B10/K8064 cheaperprofile=2501,mean498/max546ms in source89exactcontrol.
Increasing B10 toK11520 gives2484;dispersion0.4=2507; othertriage/mix/turn
variants2461–2496. Routing changes (weaker/strongercontrast,plain-scoremix,
turn-awarefield,betweenness) alllose2241–2483. Startupscorepower0.5/1/2 at
200steps gives2513/2509/2431;power1for400steps2479. No new R04frontier.

Closed/audited batches before05:56UTC: first12mixed,windowbudgetv86(5),
fusionv87(3,2timeouts),cooperativev88(6),cheaperR04v80(8).
Currentclosed: R03triagev89(6,audited),R04compactv89(9,audited),
R04routingv89(8,audited),R04startupv89(4,auditexec73508 aftersubmissions).
Fullsix-field exactproofs: old2439 source86; old2479 source88control/reuse;
old2565 cache128/512/cyclemask; old2500 source87fusedK11520.
Source89controls (2514,2501,2500) need finalsix-fieldcomparisonproof written.

Sources:
- dabfcc7/build89, regression23.04s, CachedRanking104->96bytes.
- ac5cbc9/build90, regression23.26s, compact candidates and24-byteheader fit
  one64-bytecacheline. Hash/score/order preserved; oversizedstage/epoch bypass.
  binarySHA f288fa5449b5cf474672e5cb97f6470f628bb552fd1b32c5d4561459f3e8830c.
- de0b763/build91, regression24.46s, optional fixed-work annealed windowrepairs,
  preservebestcompleteincumbent; temperature0defaultexactcontrol.
  binarySHA90e318c2e3c951f9945a6472e21cc8cd964c3ef8d1681d83ef6f637a9111840b.

ACTIVE fullbatches:
- random04-cacheline-runtime-split-full-v90, jobs8901762–67: exactB10K8064/11520,
  B18K11520fusedoff/on, B10K11520OpenMPbinding andpassivewaiting.
- random03-anneal-split-full-v91, jobs8901768–75: exact2551control,temperature
  0.25/1/4,group3,group6/I6144,plannerseeds0/3.
- random04-forecast-split-full-v90, jobs8901777–84: futuremutation0.1/0.5,
  risk-1/+0.5,elitefutureblend0.5,radius3,rootmutation0.1,dispersion0.
- random04-localfield-split-full-v90, jobs8901785–92: removeoldflip,1flip
  seeds0/1/2/3/4/6,4flipsseed5. Explicitguidancetricks.
- random03-cost-split-full-v90, jobs8901794–1800: contrast2/2.2/2.6,
  turn1.5/2.5,wait1.5/3,otherwise2551profile.
Audit source90withac5cbc9; source91withde0b763.

R04 motion diagnostic: completed loaded forwards/lowerbound=2.05for2565
versus1.67inNMS2580. First200steps=428vs638completions, althoughinitial
assignments have271two-stoptasksboth andsimilarhopwork. Differentselected
tasksets/trajectories make this observational. Most routingsoftening testslose,
so lowerdetours alone do not predict higher throughput. Evidence in
results/random34-motion-diagnosis.

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
research41|research45|research46|research47|research48|research49|research50|
research51|research52|research54|research55|research56. Avoid34/40/42 binding refusal
and53/58 timing spikes; causes unproven. Actual allocation guards are mandatory.
