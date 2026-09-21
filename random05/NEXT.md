# Active RANDOM campaign

Updated 2026-09-21 04:21 UTC. Persistent goal ACTIVE: independently validate
>=4,000 on full archived RANDOM-05 plus a fresh-input improvement. User also
requests continued general improvements and explicit tricks on RANDOM-01..04.
Non-random maps are out of scope. No delegation; other agents share this tree.

## Boundaries

Edit/stage/commit only random05/ and RANDOM05_PROGRESS.md. Raw jobs live in the
ignored runs/random05/ workspace. Public fywu85/lorr may be pushed; preserve
visibility. Do not touch other agents' files/jobs. Match every map-specific
choice to --trick RANDOM-0N. Selected maxima, repeated planner seeds, fresh
starts/tasks and unseen-layout transfer are distinct. Throughput is primary;
completed and censored waiting statistics remain secondary.

Full horizons 600/600/800/1000/2000, strict 1s entry / 30s preprocessing,
32 decimal GB process guard. EPYC9354: 16 physical cores / 32 SMT workers,
actual affinity and no-quota guards. Shared hosts allowed; no wall-time-truncated
portfolio. Existing inputs 50001–50010 are excluded from all future tuning.
No50011+ input generated. Freeze new protocol/commit/config before generation.

## Current audited records

General RANDOM01..05: 726 / 1376 / 1582 / 1558 / 2226.
Trick RANDOM01..05: 628 / 1122 / 2439 / 2516 / 4011.
Matched NMS32: 649 / 1228 / 2359 / 2580 / 3172.
All selected records have full independent replay, source/input/resources and
waiting checks. One archived layout at five densities, not unseen geometry.
Exact cases/hashes: random-frontiers.json; all-five log: RANDOM_PROGRESS.md.

R05 **4011**, source a2ff2b2, build80, case32-record3990-mix1,
batchrecord3990-coupling-split-full-v80. Seed0, first7968/K16320/B18/s2/q4/G4/E8/P8,
bias3/fraction.25, field15/flip5/contrast2.4, triage1.25/mix1, turn.6/wait.5.
WINDOW/startup/finalist rescoring off. Mean534.412/max621.751ms, RSS546080KiB.
Finished03:50:40.471914UTC. +26.5% vs NMS3172. Longest completed order1944;
130initial orders unfinished,86unopened; oldest unfinished censored2000.
All85main frontier rows verified; current waiting report rendered. Best manifests
and both ledgers promoted. The archived threshold is crossed; goal NOT COMPLETE.

Four-core record3770 remains separate, source5f81613/build65/seed3, first4608/
K5760/B12/cache512/triage1.5. Mean788/max845ms, versus NMS4=2914 (+29.4%).
FreshV4 frozen a7bad0c:3680/3641 versus stronger NMS2907/2930 (+25.42%).
Those four-core fresh results do not validate the latest32-worker refinement.

R03 now uses windowed LNS: source4b70a80/build81, H20/keep14/I2048/islands32,
blocker+equal repairs, turn2/wait2, seed5, explicit flow2.4 guidance, horizonOFF.
Case trick-random-03-window2343-keep14, batchrandom-window2343-followup-split-full-v81.
2439 beats NMS2359 by3.4%. Previous H20/keep10/I2048 seeds5/0/3/4:
2343/2357/2344/2352, mean2349. H15/keep10 gives2354. More islands64/keep10
only2351. General no-guidance H20/I2048/keep10 gives720/1376/1253 on01/02/03;
no universal benefit. R02's1376 is +12.1% versusNMS1228. R01record726 unchanged.
R04 reactive depth12/K16320 gives2500/2505/2516 on seeds5/0/3.
Best2516(source80) remains64belowNMS; depth14seed5=2487. All audited.

## Frozen V5 validation — highest priority

Protocol commit **e46f57b**, committed03:56:06UTC BEFORE generation03:56:45UTC.
Files: experiments/fresh-validation-v5-{protocol,full}.json; FRESH_VALIDATION_V5.md.
Fresh inputs runs/random05/fresh-inputs-v5/seed50009,seed50010.
Candidate is4011 above; baseline is previous3990, identical except mix.75.
Both sources/binaries a2ff2b2/build80. Two original NMS32 repetitions per seed.
NMS SHA2dfe6f35b3ae532d075aa487de88ec8d436a07ddef97e4dec24e94b550820539.

Predeclared success: exact archived repeat>=4000 and all6fields identical;
each fresh candidate beats stronger NMS repeat; fresh candidate aggregate exceeds
3990 baseline aggregate. All original attempts strict and independently replayed.
Do not change candidate or criteria based on outcomes. No guarantee of4000 on
every distinct fresh stream. Two inputs are limited replication, not significance.

Fresh candidate/baseline four runs are complete and independently replayed:
seed50009=3912/3901; seed50010=4047/4023. Aggregate7959/7924,+0.442%, both
positive. Fresh4047 is a distinct task stream, NOT a replacement archived record.
Fresh NMS jobs8901487/88/91/92 still pending; the final eight-run protocol/source/
resource audit has not run. Exact archived repeat8901484 is COMPLETE4011,
fully audited, all6fields identical (record4011-repeat-split-full-v80/audit.json
and exact-equivalence.json). Its peak entry time and resource evidence are in
that audit. Only four NMS repetitions block finishing the declared V5 comparison.
Check actual GRID handles; do not restart merely due to an observation timeout.

After completion:
1. collect split batches; archive small direct/one-level metadata JSON.
2. audit_random_cases.py for archived repeat, sourcea2ff2b2/build80; compare
   actualPaths,plannerPaths,actualSchedule,plannerSchedule,events,tasks against4011.
3. audit_fresh.py --batch runs/random05/fresh-validation-v5-split-full
   --output random05/results/fresh-validation-v5/audit.json
   --protocol-commit e46f57b
   --protocol-json random05/experiments/fresh-validation-v5-protocol.json
4. audit_fresh_replay.py with same arguments/output replay-audit.json. This new
   helper was verified against all6V4 runs; includes per-run waiting statistics.
5. Explicitly assess frozen criteria (audit_fresh prints ratios, does not enforce
   throughput thresholds). Update V5 report, main results, source/timestamp logs.
6. Full completion audit before any complete status. Current goal remains ACTIVE.

## Other active batches

- random04-field-depth-split-full-v80, jobs8901540–44:four guidanceflowseeds0–3
  (flowflips0), plus depth16/K14400. Sourcea2ff2b2/build80, plannerseed5. Explicit
  RANDOM-04 tricks. Submitted~04:14; collect and independently audit. Compare
  depth12/seed5=2500 for the fields; selecteddepth12/seed3record2516.

All preceding batches are COMPLETE and fully audited: random04-depth12-followup
(4), random03-window2439-followup(5), crowded-window-transfer(6),
random-window2343-followup(9), exact4011repeat(1). No more audit exec sessions
are pending at this checkpoint. Source/report helpers have completed.

The three paired keep14/keep10 R03 seeds5/0/3 are2439/2415/2420 against
2343/2357/2344:7274/7044,+3.265%,3/3positive. Matching hashes/configs except
retained length are proven in paired-keep-lengths.json. Includes selectionseed5.
Keep18H20/H24, islands64keep14=2436/2439/2435, no new record. Longer window
ties2439 but does not displace the earlier frontier. No fresh R03 inputs tested.
Window transfer to04/05 guided512/2048 gives1383/1425 and2034/2110; unguided
2048 gives995/1527. All strict/full/replayed, yet weaker than reactive records.

## Rejected source experiments and completed comparisons

Source83 f217eee remains current planner source (no new source change this turn).
Source84 cost-ranked retained-prefix and85 progress-ranked retained-prefix starts
failed the unchanged dense mobility floor:39/38moves, required>150. Original
controls passed. Both patches/logs/specs are archived under results/build-v84/v85;
allfour source/test files restored exactly to f217eee. No performance benchmark
uses either failed build. Do not weaken mobility tests or silently resurrect it.
Source83 WINDOW_STARTS defaults1; morestarts previously lost badly onR03.

R05 record3990 coupling complete3983(K20160/mix.75),4011(K16320/mix1),
3987(K20160/mix1), all audited. K20160/mix.5 additionalseeds5/7=3880/3944,
below K16320's3941/3948. Eight paired movebias3/bias0 seeds total31103/30609
(+1.61%,5/8positive); preserve this over initialselectedfour-seed+2.60%.

R03 windowwork source81: baselineH20/keep6/I512=1910; I2048=2194;
keep10/I512=2126; keep10/I2048=2343; horizon version1922. All4audited.
R04 depth10/K20160=2482; depth12=2500; depth10mix.75=2486; bias3=2468.
All4audited; depth12 promoted. Earlier reactive R03best2182 supersededbywindow.

## Tool and commit notes

Audit/promotion: audit_random_cases.py --batch NAME --build build-vNN --sourceHASH
[--allow-pending], then promote_random_records.py AUDIT forR01–04.
R05 uses main frontier, best manifests, waiting manifest and85-row audit as well.
The waiting manifest is results/task-waiting-frontiers-20260920T1612/manifest.json.
Never archive raw traces or whole NMScwd trees. Raw split paths are BATCH/CASE/CASE.

Fable persistent session27a4316e-b79d-46cf-86b4-41b0f558938a failed provider credits;
no new review obtained. The approved79KB payload remains unchanged. Do not retry
unchanged quota or send unapproved additional source payloads.

Use require_escalated for shell commands when namespace sandbox fails. Python3.7;
env/bin/python hasNumPy. apply_patch unavailable: usePython/heredocs carefully.
Only scoped commits: git add -- random05 RANDOM05_PROGRESS.md;
git commit --quiet --only -m '...' -- random05 RANDOM05_PROGRESS.md.
Public push authorized:
env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main
Do not amend/reset unrelated shared commits. Last protocol commit e46f57b;
new records/evidence this turn are being committed/pushed. Verify git log and
remote before assuming a push failed or repeating work.

Host list:research32|research33|research35|research36|research37|research39|
research40|research41|research45|research46|research47|research48|research49|
research50|research51|research52|research54|research55|research56.
Avoid34/42bindingrefusals and53/58spikes. Causes unproven; actualguardsmandatory.
