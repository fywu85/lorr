# Active RANDOM campaign

Updated 2026-09-21 03:04 UTC. Persistent goal ACTIVE: reach4,000 on full2,000-step
RANDOM-05 combined, then independently validate the selected improvement on fresh
inputs. Best3,978, gap22. User also requests general improvements AND explicit
tricks for RANDOM-01..04. Preserve this expanded scope across continuations.
Non-random maps are deferred; no memory-bounded port is requested.

## Boundaries

Only edit/stage/commit `random05/` and `RANDOM05_PROGRESS.md`. Other agents share
main and GRID; leave their files, staging and jobs alone. No delegation. Public
`fywu85/lorr` may be pushed; preserve visibility. Throughput primary, waiting
secondary. All instance-specific choices require the matching `--trick RANDOM-0N`.
Selected maxima, planner-seed means and fresh-input validation are distinct.

Full archived horizons600/600/800/1000/2000. Strict1s entry,30s preprocessing,
32decimalGB address-space guard. Allocation16physical/32SMT EPYC9354; keep actual
affinity/no-quota checks. Shared hosts allowed. Fixed declared work completes or
entry times out; no wall-time truncation. Preserve every failed original attempt.
Fresh task/start seeds50001–50008 are excluded from tuning. No50009+ generated.

## Current records and logs

All-five ledger `RANDOM_PROGRESS.md`, frozen configurations `random-frontiers.json`.
General01..05:726/1370/1582/1558/2226. Trick01..05:628/1122/2171/2462/3978.
Matched NMS32:649/1228/2359/2580/3172. General records are independent algorithmic
variants, not an automatic per-instance dispatcher. Selecting by map name is a
trick. All cases share one819-cell layout, not unseen-geometry validation.

R01 windowH20/512iterations/turn2/wait2/seed4:726. Fixed settings on planner
seeds5/0/3/4 give718/717/714/726 (mean718.75). R02windowH20/2048iterations/seed5:
1370. EarlierH15/512 fixed settings give1314/1323/1319/1325 (mean1320.25).
All strict/replayed, source80 `a2ff2b2`. No guidance or horizon tricks. New
maxima+11.9%/+11.6% versus a single matched NMS run; not fresh-input claims.

R05best3,978 source`acdbfd7`, build77, plannerseed0, movebias3/fraction.25,
B18/first7968/K16320/s2/q4/G4/E8/P8, field15/flip5, contrast2.4,
triage mix.5/scale1.25, turn.6/wait.5; startup and finalist rescoring OFF.
Finished2026-09-21T02:16:03.845916+00:00. Mean616.646/max785.890ms,
RSS546628KiB, +25.4% against3172. Replay and all82progress rows verified.
Maxcompletedwait1959; initialunfinished130/unopened92, oldestcensored2000.
Evidence`move-bias-refinement-split-full-v77/32-move-bias3-b18-seed0`.
`best.json` and `best-32-workers.json` pin it. Mainlog`RANDOM05_PROGRESS.md`.

R05bias3 seeds0/3/4/5:3978/3839/3782/3941 vsbias0 3666/3675/3877/3928,
aggregate+2.601%,3/4positive. Additional6/7/8/24:3901/3948/3850/3864 vszero
3832/3851/3876/3904. All strict/replayed. Eight-seed aggregate not yet written.
Bias2 four-seed aggregate+1.842%. These are one development input, not fresh.

Four-core best3770 source`5f81613`, build65, seed3, first4608/K5760/B12,
G4/E8/P8, triage1.5/cache512. Mean788/max845ms,+29.4%vsNMS4=2914.
Frozen freshV4 protocol`a7bad0c` preceded generation50007/50008:3680/3641
vsstrongerNMS2907/2930,aggregate+25.42%. Six original runs strict/replayed.
Later selected maxima are not independently validated yet.

## Sources/builds

-77 `acdbfd7`, SHAca5840674488735bcd31dbed3416f309e9928ce2954a1c1fcfe70308dbea3e5c:
move-bias proposal. Zero control exactly reproduces3928 in six trajectory fields.
-78 `6b40cd5`: explicit trick labels01..05 only, control3928 exact.
-79 `9e9dbfa`, SHA5338e02c5cfa2b42dbb7bdda4a5b1c6dd52a6dfe483d614d8f3dddbb96accee6:
fraction knob; .25control3941 exactly matches six fields. Other fractions lose.
-80 `a2ff2b2`, SHA2196d6df96c220e40f141caf64830c65a0cb8cf540a25d75f32c36ecb41c9232:
optional fixed-work windowed LNS. Regression35.39s,35compiled/test inputs verified.
Window starts from fresh pipeline or retained6steps+newtail; time-space A* repairs
small groups in deterministic parallel islands. All complete paths certified.
R05_WINDOW=0default preserves reactive mode. No participant source copied.
-81 buildcomplete03:00:15UTC, job8901349, SHA
928ae4bb6c92d92c642dcfa6d827ba13980ce713ef653f3aac482e787598178e.
Optional WINDOW_BLOCKERS/EQUAL, bothoffdefault. Guides find actual route blockers;
equal complete paths permit plateau exploration. Regression34.67s,35input hashes
verified. Needs source commit pin before benchmark submission (commit next).
New tests cover dense safety, worker/cache equivalence, checkpoints and failed
repair restoration. Source81 changes are inengine.hpp/engine.cpp/window.cpp/tests.

## Active/pending batches

Collect`python3 random05/tools/split_grid.py collect --output runs/random05/BATCH`.

- `move-bias-seed0-split-full-v80`, jobs8901291–97:3978control, K20160/K24480,
  biases2.5/3.5, fractions.125/.5. Allseed0/source80. Control must matchsource77
  inall6fields. At03:00UTC mostat1500–1800; K24480at1100. No results collectedyet.
- `random04-search-split-full-v80`, jobs8901343–48: B4/8/10/14nearK16320 and
  depths6/10; source80, explicitR04tricks. Compare2456 frozen baseline/2462best.
- `random03-reactive-guidance-split-full-v80`, jobs8901350–58:2171control,
  flowseeds0–5 (sameflipseed5), KK400andNMSfiles. All explicitR03tricks/source80.
- Source81 planned: commonH20/512/turn2/wait2 on01–03 with oldcontrol,
  blockers/equal/both; R03flow2.4 samefourvariants withtrick. Not submittedyet.

## Completed expansion batches

All successful cases below have full source/input/resource/action/waiting audits.
`tools/audit_random_cases.py --batch NAME --build build-vNN --source COMMIT`.
`tools/promote_random_records.py AUDIT...` updates01–04index and timestampedledger;
R05requiresmainfrontierpromotion separately. Retain failed cases in audits.

- `random-density-first-split-full-v79`:23cases. Noise50/dispersion0/both generally
  lose; dispersion0R02=1106. Horizon-only01/02/03=628/1063/1667. R04contrast1.6/
  dispersion.4/noise100/bias2/K24480=2267/2421/2358/2436/2462.
- `random-operations-split-full-v79`:K128/K512 on01–03=645/651,989/1038,1084/1186.
- `random-window-split-full-v80`:H15/iters8/24 turn.6/wait.5 gridlocksabove100.
  R01bestunitturn/wait1=685. R02best823,R03best162,dense04/05best6/2.
- `random-window-work-split-full-v80`:H15I128unit1=699/1172/145;
  H15I512unit1=710/1289/149; turn/wait2=714/1314/1237;
  H20I512turn/wait2=718/1314/1270. Unguided forward cost is2.
- `random-window-followup-split-full-v80`:11cases, seed repeats above;
  H15I2048R01/02=717/1351; H20I2048=722/1370/1372. All audited/promoted.
- `random03-window-guidance-split-full-v80`:lanes/KK400/flow1.6/flow2.4=
  1149/1797/1809/1839, belowreactive2171. R03trick/nohorizon/H20I512turn2wait2.
  KK400asset provenance+MITnotice copied; source80binary file input separately hashed.
- `random04-triage-split-full-v80`:scales.75/1.5/hop-only=2428/2452/2447.
  Originalno-horizonfails125(configurationmix.5requireshorizon). Separate
  `random04-no-horizon-configfix-split-full-v80` setsmix0,horizon0=2298. Bothaudited.

## Record promotion and validation

Archive directandone-level JSONmetadata only:summary/completion/spec/allocation/
submission/batch. Never rawtraces/NMScwd. Independentaction replay, source/binary/
input/resource checks precede any promotion. Fieldsforbitwisecontrols:
actualPaths,plannerPaths,actualSchedule,plannerSchedule,events,tasks.
R05newmaxupdatebestmanifests/mainlog/RESULTS/waitingmanifest/all5index+ledger/NEXT;
run audit_progress.py,audit_task_waits.py,render_waiting_report.py. Waitingmanifest
atresults/task-waiting-frontiers-20260920T1612/manifest.json.

NoV5or50009+yet. Freezeexactcandidate/source/plannerseed/protocolbeforegeneration.
Freshauditor supports16physical/32SMT and a priorbaseline. Comparecandidate+prior
+twoNMSrepeats perinput with predeclared criteria. Do not retune onfreshinputs.
`env/bin/python`hasNumPy; generatorreplicatesorganizerrecipe. PriorV4audits pass.

Fablepersistentsession27a4316e-b79d-46cf-86b4-41b0f558938a failedcredits21:55UTC;
no new review. Do not retry unchanged quota or send unapproved newpayload. Existing
approved79KBpayload remainsruns/random05/fable-review-01/payload.txt.

Last push251cf5d includes source80+685record. Later records/source81 needcommit/push.
Use `git commit --quiet --only -m '...' -- random05 RANDOM05_PROGRESS.md` after
staging onlythosepaths. Push:
`env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main`.
Hostlist32/33/35/36/37/39/40/41/45/46/47/48/49/50/51/52/54/55/56; avoid34/42/53/58.
SGEbindingadvisory; actualguardsmandatory. Refusal/spike causes remain unproven.
