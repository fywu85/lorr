# RANDOM campaign continuation

Updated 2026-09-21 07:23 UTC. The registered 4,000-task RANDOM-05 milestone is
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
Trick RANDOM-01..05: 628 / 1122 / 2580 / 2641 / 4011.
Matched NMS32: 649 / 1228 / 2359 / 2580 / 3172.
Selected bests versus NMS: +11.9% / +12.1% / +9.4% / +2.4% / +26.5%.
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

Goal ACTIVE: archived targets 2595 / 2838 plus strict subsecond qualification,
selected exact repeat, three planner seeds and subsequently frozen fresh inputs.
No fresh R03/04 input generated. Preserve 50001–10; freeze before 50011+.

R03 best **2580**, source db2d9bc5 / build99, batch
random03-window-merge-split-full-v99, case trick-random-03-window-merge-merge6.
H20/keep14, I6144, group6, 32 islands, 2 rounds, storage reuse1, fastgroups1,
window merge1, temperature0; field15/flip5/contrast2.4, turn2/wait2, seed5,
horizon800/triage1/hop-only. Mean535/max736ms. Independently replayed/promoted.
15 tasks short. General mechanism merges cost-improving complete components
across island plans; cross-parent vertex and edge-swap conflicts link agents.
All mixed windows validated; fixture tests enumerate every component combination.
Same batch: control2551, merge/group4=2562, annealed/group4=2544,
mixed-temporal/group4=2541, half-work=2481. All strict-valid/audited.
Earlier2552 annealed best remains in history, not the current frontier.

R04 best **2641**, source fa98fd1d / build96, batch
random04-matching-refine-split-full-v96, case trick-random-04-matching-refine-keep0.
The prior2634 recipe with keep bonus0 instead of0.5 and shared rankings2048MiB.
B10/K8064/first4032, depth16/G4/E8/P8/screen2/keep4, mutation0.1,
noise200/seed4; field15/flip5/contrast2.4, turn0.6/wait0.5/dispersion0.8,
horizon1000/triage1.25/mix0.5. Mean441/max498ms, RSS1551MB.
Completed 07:11:31.028236 UTC, independently replayed/promoted. 197 tasks short.
Other matching/forecast variants2391–2589. No replicated10% claim.

Exact runtime improvements remain separate from throughput selection:
- Shared rankings source92 preserves2634 in all six fields. Private mean489/
  max540ms versus2048MiB mean447/max485, RSS1558MB. Later source96 control
  repeats2634 at427/502ms. Cache default off; selected2641 enables it.
- Four-way heap source97 preserves2551/2552 in all six fields. Mean494->459
  and489->471ms; max917->759 and683->780. Mean benefit, no uniform max claim.
- Source96 A-star state/row reuse preserves2551/2552 exactly. More work and
  larger windows mostly time out; H32/I4096 scores2486 max982ms. Preserve failures.
- Source94 fast blocker groups exact2551; mean gain but varying maxima.
  results/random34-runtime/REPORT.md covers earlier timing evidence.

Recent closed trials: field23 low-work R03=2517 versus2483, but full-work
refinement2504–2545 loses current frontier. Eight new R04 fields all lose.
R04 policy transfers lose1489–2616; intent mode1 is particularly bad, preserve
negative evidence. R03 temporal blocker scans score2523–2535 versus2551;
source98 control exact evidence still to write. These options remain off.

Sources/builds after96, all full regression passed:
97 /6343b03c: optional exact four-way A-star heap. SHA6e4e038b91a1470ee5fa4662ca493f8ea403894f025675f789ee27d32eb90223.
98 /aaab5e04: temporal blocker scans (mode0 unchanged,1 rotated,2 mixed).
99 /db2d9bc5: safe full-window component merging, default off.
100 /03b68098: optional portable IPO/LTO build; this build explicitly uses --ipo.
101 /8ecb72fe: apply explicit task rank weights to window cost/merit/delay;
  default score rank0 remains unchanged. Last code build; SHA
  66160cc6d5b5c668eea46bcba4b94d2028c64bcb0e1dc22a534e0552debf8123.
Build100 SHA8abcc422288a979d0ac89b6129a3e027fbbc4b7199a09d635a5e06f8991bd69b.
Source95 disables shared cache construction in parallel forecast shadows.

ACTIVE at07:23UTC:
- random04-flow-policy-split-full-v96 (8): six done, age64/128/256 gives
  2573/2581/2621; confidence0.5/1 gives2463/2460; construction penalty2.4=2505.
  Two early-fill cases pending. Sourcefa98fd1d. Audit all when complete.
- random04-components-split-full-v96 (6), jobs8902014–19:2641 control,
  component trials32/64/128, rounds4 and min-group4. Sourcefa98fd1d.
- random34-ipo-runtime-split-full-v100 (4), jobs8902022–25: exact2551
  binary/4way,2552/4way,2641 controls. Audit source03b68098; prove six fields.
- random03-window-rank-split-full-v101 (7), jobs8902028 onward: merged-group6
  control2580 withheap4, powers0.125/.25/.5/1, startup .5/200 and .25/400.
  Explicit task-priority tricks. Source8ecb72fe. Full800 steps, strict1s.
- random03-merge-refine-split-full-v99 (6): merged-group6 seeds0/1/3/4,
  group5/I7168 and group8/I4096. Sourcedb2d9bc5. Submission session34563
  may still need closing. Every case is full800, no held-out data.

Last verified public push 4cc6aef0. Later owned source/evidence commits need push.
Commit scope remains random05/ + PILOT_PROGRESS.md + RANDOM05_PROGRESS.md only.
Pending: update campaign table/runtime report; exact source98/99 controls;
collect/audit active batches; qualify promising candidates before fresh generation.

R04 motion diagnostic: loaded forwards/lowerbound2.05for2565 versus1.67NMS;
first200steps428vs638. This is observational on diverging task sets; routing
softening mostly lost. Current2634 waypoint intervals p95=182,p99=234,max322;
priority-cap tests are explicit tricks, and waiting metrics stay secondary.

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
