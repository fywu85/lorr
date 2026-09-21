# Active RANDOM campaign

Updated2026-09-21 03:36UTC. Persistent goal ACTIVE: >=4,000 completed tasks on
full2,000-step RANDOM-05 combined, then independent fresh-input validation.
Currentbest3,990, gap10. Do not mark complete. User also requests general
improvements AND explicit tricks on RANDOM-01..04; preserve that scope through
continuations. Non-random work is deferred. No new large-map port/evaluation.

## Boundaries

Only edit/stage/commit `random05/` and `RANDOM05_PROGRESS.md`. Other agents share
main/staging/GRID; leave their files and jobs alone. No delegation. Public
`fywu85/lorr` may be pushed; preserve visibility. Throughput primary; waiting
secondary. Instance-specific choices require the matching `--trick RANDOM-0N`.
Selected maxima, planner-seed averages and fresh-input validation are distinct.

Full archived horizons600/600/800/1000/2000. Strict1s entry,30s preprocessing,
32decimalGB address-space guard.16physical/32SMT EPYC9354, actualaffinity/no-quota
checks. Sharedhosts allowed. Fixedwork completes or entry fails; no time-truncated
portfolio. Preserve every original failure. No delegation. No50009+generated;
50001–50008 are excluded from tuning. Freeze any fresh protocol before generation.

## Current verified records

All-five ledger`RANDOM_PROGRESS.md`, configurations`random-frontiers.json`.
General01..05=726/1370/1582/1558/2226. Trick01..05=628/1122/2182/2486/3990.
Matched NMS32=649/1228/2359/2580/3172. These are selected development records;
all successful records independently replayed. One819-cell layout at5densities,
not unseen-geometry validation. Per-case index is not an automatic dispatcher;
selecting settings by map name counts as a trick.

R01:windowH20/I512/turn2/wait2/seed4=726. Fixed512settings on seeds5/0/3/4:
718/717/714/726(mean718.75). R02:H20/I2048/seed5=1370; priorH15/I512 on
5/0/3/4=1314/1323/1319/1325(mean1320.25). Source80a2ff2b2; no guidance/horizon.
CommonH20/I2048/seed5 gives722/1370 on01/02; unguided03only1372.
R03:reactiveB18/K16320/seed0=2182, samefield15/flip5/mix.5 as old2171seed5.
R04:reactiveB18/K16320/depth10/seed5=2486. EarlierK24480/depth8=2462.
All matched NMS gaps: +11.9%/+11.6%/-7.5%/-3.6%/+25.8%.

R05NEW3,990, source`a2ff2b2`, build80, plannerseed0, movebias3/fraction.25,
B18/first7968/K16320/s2/q4/G4/E8/P8, field15/flip5, contrast2.4,
triagescale1.25/directional mix.75, turn.6/wait.5. Windowedmode OFF;
startup/finalist rescoring OFF. `best.json` and `best-32-workers.json` pin it.
Finished2026-09-21T03:26:06.798792+00:00. Mean512.857/max564.625ms,
RSS546312KiB(+25.8%vs3172). Evidencebatch`record3978-triage-split-full-v80`,
case`32-record3978-triage-mix0.75`. Replay/source/resource checks pass.
All84mainfrontierrows audited; waitingreport rendered. Completedmaxwait1945,
initialunfinished130/unopened91, oldestunfinished censored2000.

Its matchedK16320/mix.5parent=3978. PreviousselectedK20160/mix.5=3985.
K24480/mix.5=3872, notmonotonic. Latestmix.75has no paired/fresh validation.
Eightpaired bias3 vsbias0 plannerseeds0/3/4/5/6/7/8/24 total31103/30609,
+1.6139%,5/8positive. Supersedes initialfour-seed+2.60%; includes selectedseed5
andoldstrongseed24, not independent input sampling. Evidencepaired-eight-seeds.json.

Four-core3770source5f81613/build65/seed3/first4608/K5760/B12/G4/E8/P8,
triage1.5/cache512;mean788/max845ms,+29.4%vsNMS4=2914. FrozenfreshV4protocol
`a7bad0c` predates50007/50008generation:3680/3641vsstrongerNMS2907/2930,
aggregate+25.42%. All6originalrunsstrict/replayed. Latermaxima notfresh-validated.

## Active jobs (only five at this checkpoint)

Collect`python3 random05/tools/split_grid.py collect --output runs/random05/BATCH`.

- `record3985-seeds-split-full-v80`, jobs8901421/22:K20160/mix.5/seeds5/7.
  Source80a2ff2b2. Both pendingatlastcollect. Comparepriorseed0record3985.
- `record3990-coupling-split-full-v80`, jobs8901451–53:source80/seed0,
  mix.75/K20160;mix1/K16320;mix1/K20160. All explicitR05tricks.
  Compare3990mix.75/K16320 and3985mix.5/K20160. No fresh inputs.

All other batches submitted in this turn are terminal and archived/audited.
No live exec_command/functions cells should remain after final push.

## Sources and architecture

- Source77acdbfd7, SHAca5840674488735bcd31dbed3416f309e9928ce2954a1c1fcfe70308dbea3e5c:
  move-proposal bias, old3978record. Zero-bias control3928exact in6fields.
- Source79`9e9dbfa`, SHA5338e02c5cfa2b42dbb7bdda4a5b1c6dd52a6dfe483d614d8f3dddbb96accee6:
  configurablefraction. Quartercontrol3941exact; otherfractionslose.
- Source80`a2ff2b2`, SHA2196d6df96c220e40f141caf64830c65a0cb8cf540a25d75f32c36ecb41c9232:
  optionalwindowLNS, defaultoff. Regression35.39s;35compiled/testinputsverified.
  Freshpipeline vsretained6steps+newtail, independentLNSislands,time-spaceA*,
  complete reservations and certifiedpaths. WINDOW counts replace reactiveK.
  No participant source copied. NewR05records usewindowOFF.
- Source81`4b70a80`, SHA928ae4bb6c92d92c642dcfa6d827ba13980ce713ef653f3aac482e787598178e:
  WINDOW_BLOCKERS/EQUAL bothdefaultoff. Regression34.67s. Four controls exact.
- Source82 FAILED mobility regression before benchmarking. Failed-source.patch
  against4b70a80 plusregression.log/buildmetadata inresults/build-v82. Retained
  originalmobilitythreshold. Do not usefailedbinary or claimperformance.
- Source83`f217eee`, SHA346e291a6ae48a94eca5f2753aeffa32c633d850e1aa29120595b60b46cff82e:
  WINDOW_STARTS default1, fixedportfolio extra starts ranked by summedremaining
  taskcost. Regression22.76s passes unchangedmobility/worker/cache/checkpoints;
  all35compiled/testinput hashesverified. Fourfullone-startcontrols exact.
  FULLBENCHMARKSNEGATIVE: keepWINDOW_STARTS=1; no promotion fromthisfeature.

## Important completed experiments

Full evidence is inRANDOM_PROGRESS.md, mainRANDOM05_PROGRESS.md, RESEARCH.md.
All successes below have source/input/resource/action/waiting audits.

R05:
- `move-bias-seed0-split-full-v80` all7: control3978(exact6fields),K201603985,
  K244803872,bias2.53872,bias3.53888,fraction.1253798,fraction.53917.
- `record3978-triage-split-full-v80` all4:scale1.13969,scale1.43976,
  mix.253979,mix.753990. Newmaxpromoted; main84audits/waitingcomplete.
- Source77additionalbias3seeds6/7/8/24=3901/3948/3850/3864.
  All8paired outcomes+1.61%nowreflectedinleadingreports; do not citeonly+2.60%.

OtherRandom:
- First23configtrials:noise50/dispersion0 generallylose;R02dispersion0=1106.
  Horizon-only01/02/03=628/1063/1667. R04contrast1.6/disp.4/noise100/bias2/
  K24480=2267/2421/2358/2436/2462.
- Three-actionpolicyK128/K512 on01–03=645/651,989/1038,1084/1186.
- InitialwindowH15/24 withturn.6/wait.5 gridlocksabove100robots. Turn/wait2
  andmoreLNS work give current01/02wins. Plainforwardcost is2. R03no-guidance
  H20I512/I2048=1270/1372, belowreactive1582. Guidance crucial butinsufficient.
- WindowR03H20I512:nohorizon,lanes1149,KK4001797,flow1.61809,flow2.41839.
- Source81control/blockers/equal/both atH20I512:
  R01=718/723/718/718;R02=1314/1318/1331/1318;
  R03plain=1270/1090/1291/1105;R03flow2.4=1839/1865/1886/1910.
  Noall-densitygain; bothflagsoptional.
- Source83starts16noise30/64noise30/16noise100:
  R01=716/717/716vs718;R02=1312/1309/1316vs1314;
  R03plain=1148/1124/1144vs1270;R03guided=916/877/1063vs1910.
  All16audited, fourcontrols exact. Rejectdirectionascurrentlyimplemented.
  Plausiblecause: freshpipeline seeds firstactionneverFW; repeatedly replacing
  retainedplan mayprocrastinate. This isnotyetacausalmeasurement. Nextwindow
  hypothesis: keepcommittedprefix inallportfolio candidates, varyonlytail, and
  compareunderoneobjective. No suchcodeadded; source83 remainsoptional/off.
- ReactiveR03flowseeds0–5=2039/2055/2035/2078/2040/2024;
  KK400andNMSfileboth2001withdifferenttrajectories.2171control exact6fields.
- R04cutoffscales.75/1.5/hop-only=2428/2452/2447vs2456baseline.
  Originalno-horizonfails125(configmix.5requireshorizon);separateconfigfixmix0
  gives2298. Failurepreserved, bothaudited.
- R04B4/8/10/14=2441/2384/2417/2426;depth6/10=2258/2486.
- Crowdedseedbatchsource80: R03seeds0/3/4/6=2182/2158/2157/2182;
  R04K24480/depth8=2439/2441/2431/2470. All8audited,2182promoted.

## Audits, next work and reproduction

Use audit_random_cases.py --batchNAME --buildbuild-vNN --sourceCOMMIT for complete
source/input/resource/action/waitingaudit. --allow-pendingretains pendinglabels.
`promote_random_records.py AUDIT...` promotes01–04; R05usesmainfrontieraudittoo.
Archiveonlydirectandone-levelsummary/completion/spec/allocation/submission/batch
JSON. Neverrawtraces orNMScwd. Default-control equivalencefields:
actualPaths,plannerPaths,actualSchedule,plannerSchedule,events,tasks.

FornewR05max:replay,source/binary/input/resourcechecks; bestmanifests;
mainlogtimestamp/source/evidence;RESULTS;waitingmanifest;all5index/ledger;NEXT.
Then audit_progress.py,audit_task_waits.py,render_waiting_report.py.
Waitingmanifest:results/task-waiting-frontiers-20260920T1612/manifest.json.

Priorities:collectfiveactiveR05runs; followpromisingcutoff/workinteraction,
continue general/declaredtricks on03/04 (currentgaps177/94tasks). A retained-prefix
windowinitialization isahypothesis, notpermission toskipfullcontrols/regressions.
Further sparseimprovements canuse currentlowlatencyheadroom; preserve common
config comparisons, don'tclaimselectedper-casefrontiers are onefrozen solver.

NoV5or50009+yet. Whenreadyfreezeexactsource/config/plannerseed/protocolbefore
`generate_fresh.py`. Fresh auditt supports16physical/32SMT pluspriorbaseline.
Comparecandidate+strongprior+twoNMSrepeats perinput withpredeclaredcriteria;
neverretuneonfreshinputs. env/bin/pythonhasNumPy. PriorV4allpass.

Fablepersistentsession27a4316e-b79d-46cf-86b4-41b0f558938a failedcredits21:55UTC.
No new review; do not retry unchanged quota or send unapprovednewpayload.
Approved79KBpayload staysruns/random05/fable-review-01/payload.txt.

Commit/pushonlyownedpaths. Source83f217eeecommitted; latestresults needfinalcommit.
Lastknownremote251cf5d beforefinalpush. Usegitcommit --only -- random05 RANDOM05_PROGRESS.md.
Push:`env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main`.
Hostlist32/33/35/36/37/39/40/41/45/46/47/48/49/50/51/52/54/55/56;avoid34/42/53/58.
SGEbindingadvisory;actualguardsmandatory. Refusal/spikecausesunproven.
