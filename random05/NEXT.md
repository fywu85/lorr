# PILOT continuation

Updated 2026-09-22 05:09 UTC. Goal active; this turn made progress.

## Scope and operating constraints

Prioritize RANDOM-01 and RANDOM-04 per the latest human instruction; all five
remain in scope. Always use matched max(NMS,KK):692/1256/2359/2649/3172.
The corrected R04 target is2914; the stored app goal has stale03/04-only text
and2838. ACTIVE_GOAL.md governs current scope. Never falsely complete/recreate it.

Only edit/stage/commit random05/, PILOT_PROGRESS.md, RANDOM05_PROGRESS.md.
Public fywu85/lorr pushes authorized; preserve visibility and shared CGAR work.
No subagents. Fable provider-credit failure unchanged; do not retry.
Keep random05/ naming until development ends. Heldouts50001–50012 and50015–50022
excluded;04seeds50013/14 reserved/ungenerated. No fresh data used this turn.

Full600/600/800/1000/2000 steps; strict1000ms entry,30000ms prep,32decimalGB.
16bound physical EPYC9354cores/32SMTworkers, actual affinity/topology/no-quota
checks. Shared hosts allowed. Fixed work finishes or fails; no timed partial
solution. Explicit --trick INSTANCE for tuned guidance/admission/horizon presets.
Throughput primary; preserve completed/censored waiting evidence.

## Current records

Selected745/1408/2646/2799/4302; general727/1397/1634/1616/2314.
Leads7.66/12.10/12.17/5.66/35.62% over matched maxima. R01 needs17to762;
R04 needs115to2914. Frontiers pin full recipes and source/timestamps.

R01=745, source167/6074498c3357d3f59f6e5a32e7ad478e81dd1bd3, seed4,
MATCH_FORECAST_HOPS8/MAX32 on previous739 profile. Finished2026-09-22T04:44:25.643796+00:00,
mean70.901/max119.472ms,RSS373.232MB. Five pairs740/739/742/739/745 versus
736/739/726/732/732 total3705vs3665,+1.0914%,4gains/1tie,max119.523ms.
All audited/strict-valid.740,742 and745 repeat all six fields exactly.
Fresh01V1 still qualifies729only; general727 unchanged.

R02=1408 source132/027df4d9; general1397 source141/611aa2a6. Fresh02V1
qualifies1408 at+10.17% over matched max. Preserve new general forecast radius4
step2 timeout1016.576ms; one forecast row does not isolate its cause.

R03=2646 source162/4fb9498e, seed3, progress.25/span32, physical gate1,
heuristic1.2,H20/keep14/I6144/group6. Exact/fivepairs+0.4221%,allpositive.
FreshV3=2660/2622 vs max2461/2380, +9.109688%, still below fresh10% milestone.
All12 fresh runs audited,50021/22 excluded. General1634 unchanged.

R04=2799, source166/0bb92ceaece9a08a48465bfe94295f646659749a, seed0,
SCHED_CHAIN1/LENGTH_WEIGHT.5 on prior2782 profile. Source166 pickup-heading price0.
Cap560/all700movable, B10/depth16/K8064/FIRST4032; flowseed15,oneflip/flipseed5,
contrast2.2/20updates; sharedrankings2048MiB/orders1. Horizon1000,cutoff.875,
guidedmix.75/progress.25/span32; optionalidle32/horizonprice16. Mean467.634,
max984.795ms,RSS685.867MB. Exact repeat passes all six fields, max902.458ms.
Original noncompressedseed1 fails step0/1046.301ms; recipe NOT called robust.
Exact compact_idle1 candidate seeds0/1 give2799/2771,max848.301/859.270ms.
Their total5570 vs controls2776/2795(total5571) is not a paired gain.
The compressed extension has three audited startup timeouts:control2=1039.572ms,
candidate2=1144.865ms,control4=1041.974ms. All six extension attempts are complete.
Control3/candidate3/candidate4 finish2776/2761/2766. The three complete pairs
0/1/3 total8331candidatevs8347control(-0.1917%); five-pair qualification fails. Neither
configuration is robust; compression alone is insufficient. Better-qualified old0.25
recipe2795 exact/fivepairs+0.5714%,4gains/1loss,max907.769ms. No fresh04claim.
General1616 unchanged. Do not erase the earlier oldseed11 startup failure either.

R05=4302 source164/30a56647,seed0,terminal_pending.5 on4254field80recipe.
Exact repeat; three valid pairs+1.0477%, but disabledseed3 fails1045.313ms.
Full four-pair qualification NOT achieved; no historical substitution.
General2314 exact/fourpairs+3.4599%,3gains/1loss. FreshV6 qualifies4175only,
NMS-only. All92 selected05frontier/waiting rows audited. Longest completed1986,
146initialunfinished/110unopened,censoredoldest2000. Keep these caveats.

## Completed batches and next bounded work

No PILOT benchmark jobs remain active. The random04-compact-qualification-split-full-v166
batch, frozen879a1f93, is complete and independently audited: three valid full runs
and three startup failures. Original compressed0/1pairs remain2799/2771vs2776/2795.
Completed pairs0/1/3 give8331vs8347; no five-pair gain or robust runtime claim.
REPORT.md, paired-comparison.json, FAILURES.md and startup-failures.json contain
final accounting. Never replace failed pairs with older scores.

The exact745 job8921735 is complete, independently replayed and six-fieldexact.
Its report is random01-record745-split-full-v167/REPORT.md.

Next priority: R04 startup headroom. Observed failures spend495–593ms in
assignment and464–497ms in look-ahead; task-cost preparation adds46–54ms.
Inspect existing exact matching/cost-preparation opportunities before repeating
parameter sweeps. Parallel independent matrix/chain preparation is a hypothesis,
not implemented or sufficient by itself; no source168 yet. A declared smaller
FIRST_K is a separate deterministic configuration and must keep its own scores.
Task-dependent matching cannot move into initialization because tasks are revealed
afterward. Continue R01 effort without treating its remaining gap as a ceiling.

## Newly completed and audited

- random12345-match-forecast-split-full-v167:20attempts,19valid/onegeneral02
  timeout; all8offcontrols six-fieldexact. General off/on radius4:
  01=727/721,02=1397/FAIL,03=1634/1628,04=1616/1510,05=2314/2255;
  selected03=2646/2625. Selected01 radius8 helps; no broad transfer claim.
- random01-record740-split-full-v167:7valid; exact740repeat+3disabledcontrols.
  Complete five-pair+1.0914% and record745, as above.
- random01-forecast-radius-split-full-v167:12valid; exact742/740/727controls.
  Selected seeds2/0, radii8/12/16/24:742/739/740/739 and740/740/744/739.
  Totals1482/1479/1484/1478. Radius16 gains2 in total but does not beat745;
  do not silently change radius. General0/8/16/24=727/726/714/719, all negative.
- random04-chain-runtime-split-full-v166:9valid; four exact refs cover original
  2799repeat and optional-column compression. Compressed pairs length/idleprice
  (.25,32),(.5,32),(.5,64),(1,128) total5571/5570/5524/5529.
  Candidate.5/32 bothseeds below860ms. No paired gain. Original timeout preserved.
- random04-chain-price-split-full-v166:8attempts,7valid/one1046.301mstimeout.
  Weights.25/.125/.5/1 at idleprice32: seed1=2795/2777/FAIL/709;
  seed0=2776/2785/2799/709. Twoexactcontrols. Higher coupled idle prices later lose.
- random04-record2783-split-full-v166:9valid; exact2783; fivepairs+0.5714%,
  new2795 then exactly repeated. Later2799 is separately less-qualified.
- source166pickup-heading24valid,tenexactcontrols: feature loses all profiles;
  existing04whole-chain alternative is the useful separate finding.
- R01cooperation8valid/twoexactcontrols: totals1475/1471/1465/1467,allchangeslose.
  Prior12repair-granularity trials also allnegative.
- R04futurememory6valid: blend0/.5/1 totals5531/5459/5345;keep0.
- R05firstwork4valid: FIRST5952/4032 totals8336/8292 vs separate valid candidates
  8526; keep4302. Never substitute these for the failed disabled-credit control.

Reports under random05/results/<batch>/REPORT.md. Old record files/history remain.
Sync root summary/completion after collect; audit rows can finish after an earlier
summary snapshot. Do not re-run a complete unchanged independent audit needlessly.

## Diagnosis and implementation

R01 task-transitions:739 vs KK692 on685 common completed orders, approach5922vs7955,
loaded44842vs45855. No broad matching deficit. General source167 adds bounded
final-leg forecast rows to joint assignment; real opened tasks remain locked,
only visible orders used, no future-task reservation. Counts bounded by hop radius,
spare columns and row limit. Admission/idle/deadline matrices bypass forecasts;
horizon-price mode is rejected. Selected02/04/05 are excluded from this feature
trial because their constraints disable it; all-five GENERAL profiles were tested.

R04 task-transition report freezes2795/off2773/NMS2649. On2568 common completed
orders, loaded420016vs404002, physical bounds133115vs133584. Transition intervals
include admission/idling and order-release delay, now split from physical travel
in the diagnostic; do NOT interpret a larger release bound as long travel.
At900steps2795/NMS=2431/2417, at1000=2795/2649:132of146lead appears inlast100.
This locates the gain, not its cause. Target sustained traffic execution next.

Source166 regression67.56s; binary9c2916c87a5637ee4b9f02fbc3075b869d1effb905325ae50542b8bad3fe677d.
Source167 regression63.70s; binary0f27448cec1074f934909461f4b54592822d5c7a627b3908da0770d83c75104e.
Frozen executables runs/random05/build-vNNN/build/lifelong_random05; source/test/binary
proofs inresults/build-vNNN. No source168 yet. Default-off166heading price loses.

## Tools and git

collect: python3 random05/tools/split_grid.py collect --output runs/random05/BATCH
independent: audit_random_cases.py --batch BATCH --build build-vNNN --source HASH
exact: audit_exact_controls.py --batch BATCH --references random05/experiments/REF.json
Use --allow-pending only for partial independent audits; mark partial control
proofs explicitly. Promote only valid full replayed records; helper excludes05.
Render PILOT with tools/render_pilot_progress.py after updates.

Sandbox namespace broken: shell calls need escalation. DefaultPython3.7;
env/bin/python has NumPy. No reset/amend/lock removal. Scoped git operations only.
Check HEAD/origin at resume; commit/push checkpoints are authorized:
env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main
Hosts research35|research36|research37|research39|research46|research47|research48|
research50|research51|research52|research55|research56; exactCPU AMD EPYC 9354 32-Core Processor.
