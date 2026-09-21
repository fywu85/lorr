# Continue CGAR across all ten competition instances

Updated 2026-09-21T04:30:40.141812+00:00. Actual user scope supersedes obsolete formal Warehouse160k
goal: all10instances, NMS target, throughput primary, fairness secondary. Pursue
general improvements and explicit instance tricks together. Individual seeds
may set bests. Default-off tricks require --trick INSTANCE and labelled commits.

Ownership: never edit/stage/build/collect/interrupt random05/, RANDOM05_PROGRESS.md,
that agent's jobs or held-out streams50001..50008. Our CGAR runs use runs/cgar-*.
No internal subagents. Persistent Fable CLI session1ebb1075-3538-49d1-93d1-a00c94fa256a
is authorized, but turn46 lacked credits; do not retry without availability change.

All shell calls need require_escalated. Python3.7; quoted Python edits instead of
apply_patch. Heavy builds/tests/raw analysis on GRID. Shared main/index: explicit
owned paths and git commit --only. Never remove locks, amend, force, or stage
others. Commit/push authorized; public repository remains public.

Canonical CGAR_PROGRESS.md now concise, selected-full-results.json has all10rows
with exact environment/source/binary/evidence, BEST_HISTORY.md preserves gains.
Warehouse155173; Sortation150333; City01=8427; City02=16315; GAME21742;
R01=621; R02=1188; R03=1902; R04=1645; R05=2806. All full strict1000ms,
32decimalGB. Warehouse/Sortation8cores, others4. OnlyWarehouse has exclusive-host
qualification and independent action replay; others simulator validation plus
complete movement/waiting audits. Published NMS thresholds are not local matches.

## Current source and resource tools

General finite retarget source aad422ff1160e1daae02eccf575209da1e4edd3a,
complete regression passed; runs/cgar-rematch-budget-build-v1-20260921,
binarydfec054298b7eff48c1634e70df377ae95542f08f5ddd07ec2990afcb4478997.
Default budget1 whole controls match. Budget1..8, matching required, horizon
matching guard incompatible. Started/primary/recovery/fair/cooldown protected.

GAME fleet sourcef644accc7e00ceda7a455b7a45ed6eee4f970f8d, complete regression,
binary0a83dc762511ea924c6433b634007bfd3e5227cb5a2ef9ea491f3794c6a0a30f,
runs/cgar-game-fleet-build-v1-20260921 uses build-attested.json, notbuild.json.
Original request raced another agent's index lock; all36frozen source/test hashes
matchf644acc. Preserve original request and attestation, never rewrite history.

RANDOM reference frozenbuild runs/cgar-random-reference-build-v2-20260921,
sourceacaf634b537ff655715aae28ed43056e0c62a1de,
binary17b4f7054cec52f665abad5abb04291369983281ece30a479910e10f0f3835f2.
CITY dispatch build runs/cgar-promise-guard-build-v1-20260921 source85c3f0f,
binary726eedf767129d409fad5d869e124368b6048fa74adbb5b5ae37fc4939d600f7.

Research44/57 physical64/logical128 queues count logical slots. All new matrices
use --scheduler-slots-per-core2 --memory-gib-per-slot4: physical16 reserves32slots,
binding still16. Verifiers2slots/1physical/6G per slot. Exact binding and no-quota
checks stay. Factor2 is now exercised successfully. Keep failed oldallocations.

Analyzer oldconstants(one retarget/eightregionalbatches) caused failed reports.
Fixed assignment checker reads frozen declaration plus solver receipt, checks
actual per-taskchanges<=budget and repeatgap>=20; independentwaiting checker
takes same declaredbound(default1forlegacy), started-task reassignment still0.
Regionalbatchbound derivesparts*rounds. Originalr1/r2attempts retained; r3passed
all4rematchmatrices. R05regionalextensionr2passed. Proofrematch-budget/.

## Completed since previous checkpoint

GAME fleet: control15574, uniform2750=21742, tabu2750=21648, tabu4000=17771;
maxbest835.91ms,RSS10.562GB. Independentmask+excludedassignmentaudit passes,
controlwholetraceidentical.1000stepprefixwasworse, final+39.6%. This is deliberate
robot task-admission exclusion, not starvation-free. All6500remain movable.

R05regional8Mcontrol2684;16M2704;32M2805;8M*4rounds2806(max343.55ms).
R05eightmillionreplicationseed0/2/4:2684/2647/2684vs2574/2608/2528,+3.96%mean.
Rematchbudget1/2/4/4cadence1: R02 1160/1160/1160/1188;
R04 1622/1587/1562/1615; R05 2684/2745/2723/2746; CITY02 16169/16315/16103/16029.
Allvalidstrict1s, defaultwholecontrolsmatch. R02cadencefollowup1/2/4/8budgets
atcadence1=1183/1188/1188/1188; mostgaincadence, inspectactualequaltraces.
CITY01pickup12seeds0/2/4/6=8423/8424/8414/8422;
pickup16=8423/8427/8425/8405. Best8427seed2, only7abovehistoricNMS8420.

## Active full matrices (check before repeating)

All suffixes runs/cgar-SUFFIX-20260921; reports allmaps/results/SUFFIX.

| Suffix | Matrix | Verifier | Details |
|---|---|---|---|
|game-fleet-tuning-full-v1|8901556|8901557|seed0 uniform2750control/2000/2250/2500, sourcef644acc attested |
|game-fleet-seeds-full-v1|8901558|8901559|seeds2/4 nofleetcontrolvsuniform2750, sourcef644acc attested |
|random05-region-combined-full-v1|8901561|8901562|fiveprofiles: round4control2806,16Mround4,8Mround8,budget2,budget4cad1; sourceaad422f |
|random04-region-rounds-full-v1|8901563|8901564|round2control1622,round4,round8,parts2round4; sourceaad422f |
|random02-rematch-cadence-full-v1|8901565|8901566|completed1188/1183/1188/1188, collect/record; sourceaad422f |

GAME started04:23UTC onresearch44, others04:26onresearch57. No pending toolcalls
at handoff unless stated in conversation. Preserve sharedindex and rawfailures.

Next: collect these, promote onlyfullverifiedgainswithcontrolhashchecks, replicate
usefulprofiles; inspect real coordinated-prefix/exactchain designs forR04 gap.
Knownhorizonlowerbound restriction mustnotbe bypassed. Generalpriority,fields,
schedulingandfixedwork alreadymeasured; read TRICK_ROADMAP andnegativeevidence.
