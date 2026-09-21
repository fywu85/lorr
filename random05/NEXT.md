# RANDOM-05: active 4,000-task goal

Updated 2026-09-21 02:41 UTC. Goal ACTIVE: reach4,000 in a strict full2,000-step
combined run, then independently validate on fresh inputs. Best3,978, gap22.
Only edit/stage/commit `random05/` and `RANDOM05_PROGRESS.md`; other agents share
main and GRID. No delegation. Public `fywu85/lorr` may be pushed. Preserve visibility.
Throughput primary; waiting secondary. Tricks require explicit instance flags.
Keep32GB/1s/30s strict frontier checks. Preserve failed attempts. Fresh task/start
seeds50001–50008 are excluded from tuning; none later have been generated.

## User expansion: all five RANDOM instances

Latest user asks for general improvements AND declared tricks onRandom01–04
while continuingRandom05 toward4k. Large-map work is deferred. All edits remain
inside random05/ and RANDOM05_PROGRESS.md. New ledger:RANDOM_PROGRESS.md;
configuration/evidence index:random-frontiers.json. Keep this expanded task
through automatic goal continuations; do not let the original4k goal hide it.

Initial matchedNMS01..05=649/1228/2359/2580/3172. General starts647/1079/1582/
1558/2226; trick starts592/1122/2171/2456/3941. All successful starting traces
have independent replay. No density-specific winning configuration chosen yet.

`random-density-first-split-full-v79` has23 full cases: general noise50,
dispersion0, both, applied across all five densities; horizon-only tricks on
01–03; R04 one-change tests contrast1.6/dispersion.4/noise100/movebias2/K24480.
All use source79=9e9dbfa and explicit instance flags when appropriate. Use full
horizons and compare each case with its matching starting profile and NMS.

## Verified records

- **3,978**: source`acdbfd7`, build77, B18/first7968/K16320/s2/q4/G4/E8/P8,
  seed0, move bias3; field15/flip5, triage mix.5/scale1.25. Full configuration
  in`best-32-workers.json`; startup/rescoring OFF. Finished02:16:03UTC Sep21.
  Mean616.646/max785.890ms, RSS546628KiB, +25.4% versus NMS32=3172.
  Replay passed, all82 frontier rows audited; max completed wait1959,
  initial unfinished130/unopened92, eventual maximum censored>=2000.
  Bias3 paired seeds0/3/4/5:3978/3839/3782/3941 versus3666/3675/3877/3928,
  +2.601%,3/4positive. Includes selectedseed5; not fresh input validation.
- Previous3928 source`233f5bf`, build69, same settings with bias0. Source77
  zero-bias control reproduces all six trajectory fields exactly.
  Seven B18/B14 planner pairs aggregate26705/26778 (-.273%), 4/7 positive.
- Four-core3770 source`5f81613`, build65, seed3, first4608/K5760/B12,
  G4/E8/P8, triage1.5/cache512; mean788/max845ms; +29.4% versus NMS4=2914.
  `worker-affinity-four-full-v65` is UNSPLIT. Frozen freshV4 aggregate+25.42%
  on50007/50008 (3680/3641), six original runs pass strict/source/replay checks.
  Protocol`a7bad0c` preceded generation. Later maxima are development records.

## Frozen generalization evaluation

User chose frozen solver first; report large-map limits, NO memory-bounded port.
See`GENERALIZATION.md`. Full horizons, same16physical/32logical EPYC9354,
1s/30s,32GB. Two frozen profiles on Random01–05: generic without map tricks
(build69), and3928recipe transferred without tuning (build78). Only trick label
and declared horizon vary. Random05control remains3928, not new3933recipe.
All five Random cases share819-cell layout: density transfer, not unseen maps.
Large pipeline all-pairs table alone95–189GB: analytically unsupported32GB.
Five large NMS attempts exit124 at30s preprocessing, RSS12.1–16.1GB beforekill.
Do not call these NMS RAM failures. All original evidence retained.

Build78 changes only explicit trick-label support and runner forwarding;
Random01..05 validate32x32 and100/200/400/700/800 robots. No search change.
Regression22.19s, wrong-label negative check passed. SHA
83f08d663ef9363c8579c5d83035b50e0c2006d848e740c19b713c797ab42f6e.
Source78 is`6b40cd5`; source hashes and all fourteen new trajectories verified.
Tool`audit_generalization.py --trick-source COMMIT` checks source/binary/input,
allocation/deadline/RAM and independently replays every successful run.

## Source79 general search extension

`R05_MOVE_BIAS_FRACTION` defaults0.25, preserving source77's selected robots
and preferred directions exactly. New values vary the fraction receiving a
proposal; activation hash bits are independent of direction bits. Fraction0
is neutral; actual rollout scoring and legal-movement checks are unchanged.
Regression34.04s passes zero-fraction identity and cache/worker/checkpoint
checks at1/8,1/2,1. All34compiled/test inputs match the frozen build.
Build79 SHA5338e02c5cfa2b42dbb7bdda4a5b1c6dd52a6dfe483d614d8f3dddbb96accee6.
Source79 is`9e9dbfa`; link records to that verified source, not the shared-tree
HEAD saved when the build was submitted.

## Active batches

Collect with`python3 random05/tools/split_grid.py collect --output runs/random05/BATCH`.

- `generalization-random-generic-split-full-v69`: Random01–04=647/1079/1582/1558;
  Random05control2226. COMPLETE, all replay checks pass. Jobs8901129–33.
- `generalization-random-frozen-trick-split-full-v78`:592/1122/2171/2456;
  Random05control3928, all six fields exact vsoriginal. COMPLETE. Jobs8901152–56.
- `generalization-nms-split-full`: all9done. Random01–04=649/1228/2359/2580;
  five large prep-timeout failures as above. All19attempts archived/audited;
  historicalNMS05=3172 inputs/allocation and independent replay also pass.
- `move-bias-fraction-split-full-v79`: six strict cases, jobs8901206–11.
  Bias3 atfractions.25(control)/.125/.5/1; bias2 atfractions.5/1.
  AllB18/seed5/first7968/K16320, source79. Quarter control must reproduce3941
  exactly in all six trajectory fields. Original source77 experiments continue.
- `move-bias-refinement-split-full-v77`: six strict cases, jobs8901185–90.
  Bias3 seeds0/3/4, amplitudes2.5/3.5 seed5, and bias3/seed5 K24480 with
  unchanged first7968. Source77/build77; frozen full cases in experiments/.
- All previous followup jobs8901160–64 COMPLETE. Bias2 seeds0/3/4/5 versus
  zero:3798/3909/3785/3933 vs3666/3675/3877/3928 (+1.842% aggregate,3/4positive).
  Includes selectionseed5; same development input, no fresh validation.
  Bias3/4seed5:3941/3894. All strict,3941 independently replayed.

## Latest completed comparisons

- Move biases0/.125/.25/1/2:3928/3885/3757/3905/3933. Original.5 failsstep0,
 1066ms. Declared strict repeat and5s diagnostic both3745, six fields exact;
 preserve original failure, no proven cause. Only strict counts.
- Static rescoring weights B64 legacy/0/.05/.1/.25:3906/3769/3863/3808/3834;
 B1281/64/.05/.1:3797/3918/3888. All strict; legacy3906 exact vsbuild75.
- Future mutation B18seed5 rates.1/.2/.5/.8:3651/3730/3793/3863 vs3928.
 B14R16B64seed4 rates.5/.8:3753/3897 vs3906. Keep default.3.
- IndependentR16B64 atB14 paired0/3/4 gains27/48/34 (+.948% aggregate).
 B18seed5 identical allocation-repeat3704 vs3928; originalrefusalretained.
- Source75 controls3928/3917 exact. Startup+rescoring3867/3769 vs3917;
 B18seed5startup3747,R4B32resc3898,startup+R16B643845.
- ExpandedB18 seeds9–24:15valid,best3904seed24; seed11timesoutstep1455.
 Guidance fields49–56 allvalid,best3899. No further blind expansion planned.
- Late assignment maximum-cardinality proxy shows only0/0/3/4/2/0 extra
 feasible assignments at1250/1500/1750/1850/1900/1950. Not a causal bound.
 Decision-diversity diagnostic exists, no diversity-selection feature written.

## Important completed results

- Independent rescoring: off/R1 controls3872, exact in all six trajectory fields.
  R4B32=3885, R16B64=3906, R16B128=3659, R16B64/blend0.5=3819, R32B128=3867.
  All valid;3906 is below the overall frontier. The three paired checks are summarized above.
- Startup initial batch: controls3872/3845 exactly reproduced. Power0.25/250
  gives3917; power0.5 for100/250/500 gives3879/3756/3801; power1/250 gives3755.
  The3879 run finished after3917, so it is not a separate frontier.
- Startup follow-up: powers0.125/0.375 at250 give3800/3926; power0.25 at375/500
  gives3724/3881; B18 coupling onseed4 gives3754. Seeds0/3/5/6 give
  3762/3862/3780/3834. Original125-step case never launched: research42 exposed
  64 cores instead of16. Its separately named identical repeat scores3,718.
- Branch allocation: B18/B22/B26 near fixed work give3877/3732/3705;
  fixed3264 roots give3837/3871/3772. All six valid.
- Branch seeds: B18 seeds0/3 give3666/3675. B14/B18 pairs: seed5=3819/3928,
  seed6=3868/3832, seed7=3832/3851, seed8=3764/3876. All valid.
- All closed-loop forecast batches are terminal and lose. Legacy source73
  control3752 is exact; smaller K64/depth8 gives3785. K128 strict variants and
  K96/depth6 time out. Heavy5s diagnostics give3723 and3682. Keep disabled;
  original failures and earlier source70 negatives remain archived.
- Whole-run rank weighting, waypoint-age retention, completion bonuses,
  component recombination and the eight local guidance retunes all lose.
  Keep these failed presets off. Full details remain in progress/research logs.

## Evidence and consultation

For a new maximum: independent replay, source/binary/input checks, best manifests,
timestamped row, waiting manifest, audit_progress.py, audit_task_waits.py,
render_waiting_report.py. Archive only direct and one-level metadata JSON;
never raw trajectories or NMS cwd. Source77`acdbfd7` and source76`1ac8940`.

No V5 protocol or fresh50009+ yet. Freeze source/config/seed before generation.
Fresh auditor supports32workers and a prior baseline. All prior checks pass.

Fable CLI session27a4316e-b79d-46cf-86b4-41b0f558938a failed provider credits
at21:55UTC; no new review. Do not retry unchanged quota. Approved79KBpayload
remains unchanged inruns/random05/fable-review-01/.

Use actual affinity/quota guards. SGE binding is advisory; refusals/spikes have
unproved causes. Never alter other jobs. Newhostlist excludes34/42/53/58.

Push helper: `env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main`.
Commit only owned paths with`git commit --only ... -- random05 RANDOM05_PROGRESS.md`.

## Current expansion status

All29 source79 trials complete and independently audited by audit_random_cases.py.
Best general01=651(action K512),02=1106(dispersion0),03/04/05 unchanged.
Best trick01=628(horizon-only),02=1122,03=2171,04=2462(K24480),05=3978.
New source80 optional rolling-window LNS in window.cpp is building (job8901261,
outputbuild-v80). It has deterministic independent islands, time-space A*, full
reservations, short-prefix/pipeline initialization, and checkpoint support.
New tests cover immediate motion, repeated stops, dense safety, worker/cache
identity and restoration after failed repairs. Do not benchmark/promote until
build/regression passes; source80 not committed yet.

Source79 fraction batch all6done:3941control(exact sixfields),3904/3909/3900,
3898/3897. No gain. Source77 refinement all6done:3978/3839/3782,3890/3835/3945.
Source77 new bias3 seeds6/7/8/24 running jobs8901242–45, batch
move-bias-seeds-split-full-v77. Keep originals, collect and audit any record.

Build80 completed successfully02:40:32UTC; regression passed and all35 compiled/test inputs match the frozen snapshot. BinarySHA2196d6df96c220e40f141caf64830c65a0cb8cf540a25d75f32c36ecb41c9232. Full windowed benchmarks are next.

Source80 verified commit`a2ff2b2`. Declared full batches:random-window-full-v80(13general cases),random04-triage-full-v80(4explicit tricks),move-bias-seed0-full-v80(7cases including exact3978control). No fresh50009+ inputs generated.

Window80 first13cases complete, all strict/replayed. NewR01records664 then685,
unit-cost685mean9.87/max25.66ms; generalindex/ledger updated. R02bestwindow823,
R03best162, denseR04/R05gridlock6/2. New12casebatch
random-window-work-split-full-v80 jobs8901302–13 tests128/512iterations,
turn/wait2 andwindow20 identically across01–03. All still general/no tricks.
R04triage jobs8901287–90 running; originalno-horizon configfails125 because
mix.5requires horizon. Corrected separatejob8901314 in
random04-no-horizon-configfix-split-full-v80 sets mix0 with horizon0.
R05seed0seven-casefollowup jobs8901291–97 running;source80 a2ff2b2.
Previousbias3seeds6/7/8/24 allstrict:3901/3948/3850/3864; no newrecord.
