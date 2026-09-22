# PILOT bounded statistical wrap-up

The latest user request adds throughput standard deviations and authorizes more
seeds after asking to wrap up and stop. The only remaining work is the frozen
random12345-wrapup-seeds10 protocol:23existing distinct seed observations and27
new runs, covering planner seeds0..9 for each selected profile. No algorithm
or parameter changes. Finish source/input/resource/trajectory/waiting audits,
report sample SD(n-1),mean,best,n and failures,log/commit/push,then pause.
Do not treat the earlier pause text as preventing this explicitly requested
measurement; do not treat this measurement as restarting the development goal.

Source170 observational diagnostics are complete: full01/04controls745/2799
match all six fields;600/1000native suppression masks match. CTest64.05s passes.
Read results/random14-triage-diagnostic-split-full-v170/REPORT.md. No source171
or new policy exists. Best selected profiles currently745/1408/2646/2799/4302.
The ten-seed protocol freezes exact binaries and parameters per instance.

## Scope and constraints

Maximize all five RANDOM instances; prioritize RANDOM-01 and RANDOM-04 per the
latest human steering. Always compare with matched max(NMS, KK):
692 / 1256 / 2359 / 2649 / 3172. RANDOM-04's ten-percent reference is **2914**.
The app goal's NMS-only 2838 wording is stale; ACTIVE_GOAL.md records current
scope. Never falsely complete/recreate the goal to edit its text.

Only edit/stage/commit `random05/`, `PILOT_PROGRESS.md`, `RANDOM05_PROGRESS.md`.
Public `fywu85/lorr` pushes are authorized; preserve visibility and shared CGAR
work. No subagents. Fable provider-credit failure is unchanged; do not retry
without new information. Naming cleanup from random05/ to pilot/ is deferred.
Heldouts50001–50012 and50015–50022 stay excluded;04 seeds50013/14 remain
reserved and ungenerated. No fresh input was used this turn.

Full horizons600/600/800/1000/2000; strict1000ms entries,30000ms preparation,
32 decimal GB. Reserve16 physical EPYC9354 cores /32 SMT workers; verify actual
binding, CPU model and no quota. Shared hosts allowed. Complete declared fixed
work or fail; no wall-time truncation or partial plan. Tuned guidance, admission
and horizon presets require explicit `--trick INSTANCE`. Throughput is primary;
completed and censored order waiting remains a secondary metric.

## Selected records and qualification

Selected **745 / 1408 / 2646 / 2799 / 4302**; general727/1397/1634/1616/2314.
Leads over matched maxima:7.66/12.10/12.17/5.66/35.62%. R01 needs17to762;
R04 needs115to2914. The all-instance ten-percent values are yardsticks; only
R03/04 were explicitly assigned that numeric qualification. Do not shrink scope.

R01=745, source167/6074498c3357d3f59f6e5a32e7ad478e81dd1bd3, seed4,
forecast radius8/max32/weight1, H20/keep6/I512/group8/islands32, no guidance,
turn/wait2, horizon600/cutoff.75, physical matching gate1, anneal.25,
progress.25/span32. Mean70.901/max119.472ms. Five paired seeds740/739/742/739/745
vs736/739/726/732/732: +1.0914%, four gains/one tie. All strict-valid/replayed;
740/742/745 repeat exactly. Fresh01V1 qualifies the older729 profile only.

R02=1408 source132/027df4d9; general1397 source141/611aa2a6. Fresh02V1
qualifies1408 at+10.17% over matched max. Preserve the source167 general
forecast radius4 step2 timeout1016.576ms; one forecast row does not isolate cause.

R03=2646 source162/4fb9498e, seed3; exact/five pairs+0.4221%, all positive.
FreshV3=2660/2622 vs matched2461/2380: +9.109688%, still below fresh10%.
All12 fresh runs audited;50021/22 remain excluded. General1634 unchanged.

R04=2799 now selects the **exact faster source168 implementation**:
0f95c7878ac7bf4b5393a18b6feb4b16f1c400b6, seed0, MATCH_SKIP_ZERO1,
COMPACT_IDLE1, whole-chain1/length.5/pickup-heading0, cap560/all700 movable,
K8064/FIRST4032/depth16/B10, field15/oneflip/flipseed5/contrast2.2/20updates,
shared rankings2048MiB/orders1, horizon1000/cutoff.875/guidedmix.75,
progress.25/span32, idleprice32/horizonprice16. Mean474.550/max664.721ms,
finished2026-09-22T05:33:23.330167+00:00. See manifest for all settings.
Five contemporaneous off/on seeds0..4 score2799/2771/2761/2761/2766, all six
trace fields exact and independently replayed. Optimized maxima662–669ms vs
805–825ms controls. Another exact2799 repeat in layout batch peaks657.644ms.

This is **not a throughput increase**. `first_attained` in random-frontiers.json
retains original source166/0bb92cea, timestamp2026-09-22T04:26:21.345951+00:00,
max984.795ms. Its noncompressed seed1 failure1046.301ms and compressed
control2/candidate2/control4 failures1039.572/1144.865/1041.974ms remain failed
attempts. No historical substitution in those comparisons. The source166
length.25 recipe2795 has a separate five-pair+0.5714% qualification; length.5
has not shown a paired mean gain over it. No fresh04 qualification yet.

R05=4302 source164/30a56647, seed0, terminal-pending.5 on4254 field80 recipe.
Exact repeat, three valid pairs+1.0477%; disabled-credit seed3 fails1045.313ms.
The full four-pair qualification is NOT achieved. Source168 runtime off/on both
repeat4302 exactly, but do not replace that different terminal-credit failure.
General2314 exact/four pairs+3.4599%, three gains/one loss. FreshV6 qualifies
4175 only and is NMS-only. All92 selected05 history/waiting rows remain audited;
longest completed1986,146 initial unfinished/110 unopened, censored oldest2000.

## Previous checkpoint: 55 completed full runs

1. `random12345-zero-update-split-full-v168`:18/18 valid and independently
   replayed.16 declared historical comparisons and9 contemporary off/on pairs
   match all six fields. All five selected scores unchanged. R04 startup solves
   take338–339ms vs478–501ms;160752/163770 increments arezero (98.157%).
   Only add/subtract-zero bookkeeping passes are skipped, with all relaxations,
   tie choices and search effort intact. Full-run mean latency is mixed; the
   clear benefit is startup headroom. REPORT.md, paired-equivalence.json,
   RANDOM04_RUNTIME.md and random04-exact-runtime.json contain complete evidence.
2. `random01-forecast-cap-split-full-v167`:8 valid; exact745/740 controls.
   Caps32/8/16/24 total1485/1474/1473/1485 across seeds4/0. Cap24 also matches
   both complete cap32 traces in a post-result comparison. Keep32.
3. `random01-forecast-weight-split-full-v169`:12 valid; four exact controls.
   Selected weights0/.25/.5/1 total1468/1462/1472/1485 across seeds4/0.
   General off727; radius8 weights.25/.5/1 give718/723/726. All discounts lose.
4. `random01-forecast-emphasis-split-full-v169`:9 valid; three exact controls.
   Selected weights1/2/4 give745/727/730 onseed4 and740/730/730 onseed0;
   general726/708/702. All higher weights lose. Keepweight1.
5. `random04-chain-layout-split-full-v168`:8 valid; exact2799 control.
   Field15/oneflip=2799;15/noflip2772,1=2625,3=2692,7=2686,23=2670,
   31=2683,43=2677. All six additional field seeds lose. This also provides the
   extra source168 exact2799 repetition, mean474.599/max657.644ms,RSS689.877MB.

Every batch has a full independent audit, exact declared controls and REPORT.md.
Current selected throughput remains unchanged. Keep all negative results. No
fresh task/start or geometry claim. Root summaries/completions were collected.

## Source and tests

Source168=0f95c7878ac7bf4b5393a18b6feb4b16f1c400b6,
binary96875dae2d61b629df4d31ab012f4908a654669640bffffa7142fc4945bd1ad6.
FullCTest63.31s,3840 exact assignment fixtures. MATCH_SKIP_ZERO is defaultoff;
selected04 now explicitly enables it. Source/test/binary proofs inresults/build-v168.
Source169=b429a0285b729e3a42d090b1c56cbc9abbdd4bd6,
binary24850412496ae7fa647b2044030b16e5db5a09afe1e62d4481a91adfea25da38.
FullCTest61.04s. MATCH_FORECAST_WEIGHT0..4 scales only virtual rows' final prices;
default1 exact. Manual witness, workers/caches/checkpoints/constraints pass.
No nondefault weight has shown a gain. Source/test/binary proofs inresults/build-v169.
Frozen executables are runs/random05/build-vNNN/build/lifelong_random05.
Source170 adds only observational diagnostics; see the paused checkpoint above.
Do not repeat passing full tests without a new reason.

## Notes for an explicit future resume

The end-of-run diagnosis is now complete; do not repeat it. No further algorithm
was implemented after the diagnostic because the user asked to stop. All archived
runtime and negative-setting evidence below remains relevant. The old
`audit_goal_suppression.py` still handles only the old hop-only rule; use the
new triage probe and audited native fingerprints for current guided/progress
mixtures. Neither case has a demonstrated inherent ceiling.

The prior04 common-order comparison (2795 vs NMS2649,2568 common completions)
finds loaded420016vs404002 steps, physical bounds133115vs133584. At900steps
PILOT/NMS=2431/2417, final2795/2649:132 of146 lead emerges inlast100.
Transition waiting includes order release/admission; do not call it all travel.
See results/random04-task-transitions-v166/REPORT.md. R01's739 trace already
improved both approach and loaded work over KK; no broad assignment deficit.

Avoid blindly repeating negatives:01 repair groups/cooperation/extra work,
heading prices and next-pickup hints lost;04 future-memory blends/local search,
flow-model changes and the six new layouts lost. Earlier04 K10240/12288 on2777
profile scored2751/2754 with883/954ms maxima. Capped04 had only five layout seeds
before this turn, now eleven; extra-flip coverage remains limited, but no new
layout benefit has been observed. Fresh04 seeds50013/14 remain reserved.

## Tools and git

collect: python3 random05/tools/split_grid.py collect --output runs/random05/BATCH
independent: audit_random_cases.py --batch BATCH --build build-vNNN --source HASH
exact: audit_exact_controls.py --batch BATCH --references random05/experiments/REF.json
promote_random_records.py only promotes higher throughput and excludes05; the
explicit equal-score04 runtime replacement is documented and keeps first_attained.
render: python3 random05/tools/render_pilot_progress.py

Sandbox namespace broken; shell calls need escalation. DefaultPython3.7;
env/bin/python has NumPy. No reset/amend/lock removal. Scoped git operations only.
The prior public checkpoint wasa00599ab. This wrap-up includes sourcee30c20b1,
protocol455e642e, runner4b1b2dd0and its final audit/pause commit. Verify HEAD and
origin/main rather than rely on a commit hash embedded in the same checkpoint.
All edits are scoped to random05/,PILOT_PROGRESS.md,RANDOM05_PROGRESS.md.
Public push remains authorized; preserve unrelated shared CGAR work and visibility.
