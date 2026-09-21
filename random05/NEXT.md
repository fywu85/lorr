# RANDOM campaign continuation

Updated 2026-09-21 05:09 UTC. The registered 4,000-task RANDOM-05 milestone is
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
Trick RANDOM-01..05: 628 / 1122 / 2439 / 2565 / 4011.
Matched NMS32: 649 / 1228 / 2359 / 2580 / 3172.
Selected bests versus NMS: +11.9% / +12.1% / +3.4% / -0.6% / +26.5%.
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

## Latest RANDOM-04 experiments and next research

New batch random34-tenpercent-first-split-full has12 active cases,
jobs8901631–8901642: eight R03 routing/scheduling variants (source81) and
four R04 runtime controls (source80). Audit using --source-map
random05/experiments/random34-tenpercent-source-map.json, because the
batch deliberately uses two frozen binaries. The older batch below is closed. All six original attempts in
random04-depth16-validation-split-full-v80 are terminal and audited.
Source a2ff2b2/build80, explicit --trick RANDOM-04:
- depth16/K14400 seed4 =2565, mean832/max949ms, RSS440MB (new selected record);
- the same configuration seed6 =2536, mean826/max892ms;
- depth18/K13056 seed5 =2525, mean904/max987ms;
- depth16 seeds0/3 fail at step1/217, 1197/1007ms respectively;
- depth20/K12240 is rejected at initialization: K must be divisible by96.
All failures are retained. No throughput claim for invalid runs and no unproven
assertion that host contention caused the timing spikes. Corrected depth20
would require K12288 or another multiple of96, declared as a distinct trial.

The 2565 record is only15 tasks below local NMS2580; it is a selected planner
seed gain from previous2551, not a new algorithm or fresh-input result. Priority
is throughput plus runtime headroom. Candidate directions: reduce per-future
cost without changing trajectories, controlled lower-work/depth combinations,
and small scheduling/guidance ablations after profiling. Do not consume held-out
inputs for tuning. Additional fresh validation needs a newly frozen protocol.

Earlier random04-throughput-followup-split-full-v80 is fully audited:
loop threshold2/3 =2523/2526, length0.5=2546, rank0.25=2531; none beat2551.
Field/depth batch: depth16/K14400 seed5=2551, mean831/max902ms, RSS436MB;
depth12 field seeds0/1/2/3=2304/2302/2395/2361. Prior depth12 seeds5/0/3
=2500/2505/2516; depth14/seed5=2487; depth12/mix1=2470. No new source change.

## RANDOM-03 and general improvements

R03 record2439 uses source4b70a80/build81, guided windowH20/keep14/I2048,
32 islands, blocker/equal repair, turn2/wait2, seed5, horizonOFF.
Case trick-random-03-window2343-keep14, random-window2343-followup-split-full-v81.
Keep14 vs keep10 paired seeds5/0/3:2439/2415/2420 vs2343/2357/2344,
aggregate+3.27%, all positive, including the selection seed. Evidence in
random03-window2439-followup-split-full-v81/paired-keep-lengths.json.
Longer window/keep or more islands ties at best. No fresh R03 inputs tested.

General H20/I2048/keep10 gives720/1376/1253 on R01/02/03: improves R02,
not universal. R01 record726 remains. The same guided window profile loses
at high density: R04 I512/I2048=1383/1425; R05=2034/2110. Unguided=995/1527.
All six strict/full/replayed, so retain reactive planning for crowded cases.

## Rejected source experiments / tool notes

Source86 raises only the explicit WINDOW_ITERS ceiling2048->8192, default
unchanged. Regression/build and full2048 controls are required before crediting
the larger work budget. All other current source matches f217eee (build83),
default WINDOW_STARTS=1. Extra starts
lost on R03. Source84 cost-ranked retained-prefix and source85 progress-ranked
retained-prefix attempts failed the unchanged dense mobility floor (39/38 moves,
required>150). Both patches/logs are in results/build-v84/v85. All four modified
source/test files were restored. No full benchmark used either failed build.
Do not weaken mobility tests or silently resurrect the failed approach.

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

Hosts: research32|research33|research35|research36|research37|research39|research40|
research41|research45|research46|research47|research48|research49|research50|
research51|research52|research54|research55|research56. Avoid34/42 binding refusal
and53/58 timing spikes; causes unproven. Actual allocation guards are mandatory.
