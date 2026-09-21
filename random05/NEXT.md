# RANDOM campaign continuation

Updated 2026-09-21 06:55 UTC. The registered 4,000-task RANDOM-05 milestone is
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
Trick RANDOM-01..05: 628 / 1122 / 2552 / 2634 / 4011.
Matched NMS32: 649 / 1228 / 2359 / 2580 / 3172.
Selected bests versus NMS: +11.9% / +12.1% / +8.2% / +2.1% / +26.5%.
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

Goal ACTIVE: archived targets2595/2838 plus strict subsecond qualification,
selected exact repeat, three planner seeds and subsequently frozen fresh inputs.
No fresh R03/04 input generated; preserve50001-10 and freeze before50011+.
R03 best2552 source de0b763/build91, anneal temperature4, H20/keep14/I8192,
group4/32islands/2sharingrounds, horizon800/triage1/hop-only, seed5.
Batch random03-anneal-split-full-v91, case trick-random-03-anneal-temperature4.
Mean523/max761ms. Only+1 over nonannealed2551: no broad gain established.
2551 profile seeds0/3 give2522/2516, all strict. See results/random34-runtime.
R04 best2634 source ac5cbc9/build90, rootmutation0.1, B10/K8064/first4032,
depth16/G4/E8/P8/screen2/keep4/noise200/seed4; field15flip5contrast2.4,
turn0.6/wait0.5/dispersion0.8/horizon1000/triage1.25/mix0.5.
Batch random04-forecast-split-full-v90, case trick-random-04-forecast-mutation01.
Mean495/max532ms. Same config seeds0/3=2584/2530; throughput spread real.

Exact shared immutable rankings source b4b4abe/build92 preserve all six fields
and2634 tasks. Four full controls audited in random04-shared-rankings-split-full-v92.
Private control mean489/max540ms; shared2048MiB mean447/max485ms/RSS1558MB.
1024MiB449/480;2048/no-private441/593. Runtime evidence is a measured comparison,
not a worst-case guarantee. Default remains off. No new throughput record.

All closed batches through searchshapev92 and fastgroupsv94 audited.
R04 rootmutation.02/.05/.15/.2=2448/2535/2529/2482;G8/G16=2622/2632.
K11520 B10 timed out;B18=2603. Searchshape E1/4/16=2474/2547/2556;
noise100/400=2564/2593;discount.95/.98=2534/2582;depth12/20=2599/2545;
rank1/2=2487/2416. None surpass2634. Fine guidance and scheduling/cost changes
mostly lost. See individual results/*/audit.json; preserve all timeout attempts.
R03 longer windows initially fail strict1s; smaller initial fixed work alone
moves failure to step1 or150. Fast blocker groups skip84-87% unused spatial
sorts; full control and fast both2551, means505/475ms but max719/918, so no
proven maximum-latency gain. Six-field equivalence proof still to write.

Current source fa98fd1d/build96, regression25.06s, SHA
 ae3feb041eb03db6e7a0bb059741cbd09374d9691cf98f3969bc2261fcf5aa1f.
Changes since91:92 shared immutable rankings bounded by RAM;93 fixed FIRST_ITERS;
94 fast groups and explicit max16384;95 disable cache building in parallel
forecast shadows;96 carry A-star state/stage and cached rows, declare fixed
WINDOW_INITIAL_STEPS schedule. All regression suites passed unchanged.
Nested forecast fix95 does not affect92 full cache records (replan off).

ACTIVE:
- random03-astar-warmup-split-full-v96, jobs8901937–45: exact2551/2552 controls,
  first8steps4096 then8192/12288/16384, H24/H28/H32 variants. As of06:54,
  warmup8=2502;16384/H24keep16/H24keep18/H28 already failed strict deadlines.
  Await remaining controls/12288/H32; audit with fa98fd1d.
JUST CLOSED (need audit):
- random03-layout-screen-split-full-v94, source0ca9413c,9full800-step runs.
  I4096 control2483;field4/7/11/19/23/29/37/43 =
  2405/2445/2445/2318/2517/2359/2416/2481. Field23 merits higher-work followup.
- random04-layout-screen-split-full-v92, sourceb4b4abe,9full1000-step runs.
  K4096 control2492;fields4/7/11/19/23/29/37/43 =
  2369/2370/2375/2114/2398/2225/2313/2439. No win in this screen.
These are explicit trick field searches, archived tuning only.

Pending: audit closed fields; six-field94/96 controls; commit scoped changes
and push (last verified remote8714169; later commits not pushed).

R04 motion diagnostic: completed loaded forwards/lowerbound=2.05for2565
versus1.67inNMS2580. First200steps=428vs638completions, initial assignments
have271two-stop tasks each and similar hopwork. Different selected task sets
make this observational; most routing softening tests lost. Evidence in
results/random34-motion-diagnosis. Further progress needs better collective
routes/search, not merely a shorter-distance proxy.

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
