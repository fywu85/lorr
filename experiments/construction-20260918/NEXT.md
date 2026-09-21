Latest 08:16 UTC: R02 replication promotes1215seed2, R05 selectedscan promotes3065seed10.
R01 all3seedspositive +4.29%; R02 +0.64% one loss; R05 horizon +0.19% one loss.
Only R03 scaling matrix8902208/8902209 remains. See canonical selected records.

Updated checkpoint at 2026-09-21T08:13:21.706571+00:00: prefix v5 completes. RANDOM-01=662 (seed2,
4096 attempts/island), RANDOM-02=1202 (seed4,2048). Both source339718b3, full
strict1s/4core. RANDOM-03 prefix variants1678/1849/1737 versus1829 windowcontrol;
selected1902 remains. All3control wholetraces repeat. Exact records promoted in
selected-full-results.json and BEST_HISTORY. Replication against previous frontier
profiles: R01 seeds0/4 jobs8902204/8902205; R02 seeds0/2 jobs8902206/8902207.
R03 eightcore fixedwork scaling jobs8902208/8902209. See ACTIVE_JOBS.json.
Sortation rematching completes:150830/150753/150759/150865; frontier150894unchanged.
Latest runtime alternative-v3=150865/max894.99ms, controlwholetrace repeats.
Proposed next implementation: renew the tail of retained window history with
complete joint seed projection; current history is truncated and compared with
fully projected fresh seeds, which may systematically reject useful warm starts.
No such code edits yet. Prior detail below is historical where superseded.

# Continue CGAR across all ten instances

Updated 2026-09-21 08:03 UTC. Latest user instructions supersede the obsolete formal
Warehouse160k goal: exceed max(published NMS,KK) by10% on RANDOM01-05; aim for5% on
other five, explicitly a stretch for Warehouse. Throughput primary, fairness
secondary. Selected-seed maxima qualify. Continue overnight; user asleep. General
CGAR mechanisms AND explicit instance tricks. Never mark goal complete just
because an old published target is met. No token budget; active goal persists.

## Ownership and execution

Do not edit/stage/build/collect/interrupt random05/, RANDOM05_PROGRESS.md,
PILOT_PROGRESS.md, its jobs or held-out task streams. PILOT read-only reference
and transfers into CGAR are explicitly authorized. Read immutable git snapshots.
Only our runs/cgar-* jobs/artifacts are owned here. No internal subagents.
Fable CLI persistent session1ebb1075-3538-49d1-93d1-a00c94fa256a is authorized;
turn46 lacked credits; no availability change, so do not retry yet.

All shell tools require require_escalated (default bwrap fails). Python3.7,
quoted Python/heredoc edits; apply_patch fails. Heavy build/test/raw analysis on
GRID; read small summaries locally. Research44/57:64physical/128logical; use
scheduler-slots-per-core2. Four physical cores/case=8slots x4GiB; eight cores=
16slots x2GiB. Exact physical binding/noquota guards, strict1000ms complete entry,
32decimalGB process limit. Shared hosts allowed. Fixed work, timeout is failure.
Analyzer currently accepts4/8cores; extend/check if experimenting on16cores.

Shared main/index: explicit owned files and git commit --only, never remove locks,
amend, force, or stage others. Public repo must stay public. Commit/push authorized.
Push: GIT_ASKPASS=/bin/false GIT_TERMINAL_PROMPT=0 git
-c 'credential.helper=!/user/fw2449/.local/bin/gh auth git-credential' push origin main.
No tool/exec sessions should remain pending after current checkpoint preparation.

## Current selected full strict1s records

WAREHOUSE155173; SORTATION150894; CITY01=8427; CITY02=16315; GAME24447;
RANDOM01=647; RANDOM02=1197; RANDOM03=1902; RANDOM04=1999; RANDOM05=3043.
Targets:162535/160350/8868/17847/24438/757/1386/2568/2802/3355.
Only GAME meets the new margin target. All selected rows use --trick INSTANCE.
Warehouse/Sortation8cores, others4. Warehouse has exclusive-host and independent
100M-action replay qualification; others simulator validation plus complete
movement/waiting accounting. Published competitors are historical, not matched
local comparisons. No starvation-free claim, particularly GAME/R05.

Canonical allmaps/selected-full-results.json includes exact source, binary, seed,
UTC, configuration and evidence. BEST_HISTORY.md is append-only. Latest all10proof
snapshot selected-results-checks-20260921-0746.json. Active targets in
active-targets.json; CGAR_PROGRESS.md links evidence. New records must update all.

R05 new3043: source7783736fec8dd929316fdaa2bd6d0b4fdd92c94c, frozen
runs/cgar-chain-potential-build-v1-20260921, binary
37c9ae5d6e55c6698037872626ce2991b6629e3f23904764cf1eca60d038e8fa.
Profile chain-potential/best-random05-horizon.json. Explicit known2000step horizon,
Manhattan-plus-service bound, prospective mean completion margin; new assignments
only. Seed0, max742.14ms, four cores, full2000steps. Control3027wholetrace repeats;
bare horizon3027; mean3043; p90=3037. Agep90=2000. Margin replication/scan pending.

Underlying chain score-only (original priority retained) replicates across seeds
0/2/4:3027/3010/3022 vs2956/2900/2889, +3.59%aggregate, all positive. Controls
wholetraces repeat. chain-potential/random05-three-seed-summary.json. Exact chain
ordering loses on R05; full chain score/order loses R02/03/04. R01 selected647has
unchanged three-seed mean629, not a general average-gain claim.

## Major new implementation: fixed-work rolling window

All defaultOFF; none of its completed throughput variants has surpassed selected
CGAR records. Do not replace the canonical profiles with an experimental window.

15640f8d52e8db60f32e3062b98f6b0a2e4dc49c: new rolling_window.hpp and adapter.
Complete5actionCGARseed (actual wait-seed rotations reconciled), then waits toH20.
Time-space A* carries task stage and charges actions+exact remaining chain.
Small groups removed/repaired atomically; bounded A*failure rolls back wholegroup.
All fixed attempts/islands complete; deadline errors propagate even after gains.
History checks executedcell/heading, taskID, one-action service suffix, domains,
new protections; conflicts reset monotonically to fresh valid seeds. Primary,
recovery/support/witness/pocket restrictions maintained. Default-off trajectories
identical. Independent oracle memory budget rejects big quadratic allocations.

72418383: optional CGAR_WINDOW_SEED_ROLLOUT=1 extends first5actions with complete
joint TemporalPibt chunks; same protected tails fixed. Allchunks complete; choose
wait-extended seed if its whole-window objective is lower. Independent window RNG,
so a node-capped no-op layer with historyoff preserves whole CGAR trajectories.
Build-v2 captured shared headc332609a90f1fdd8ff493897e230b5a3125e42fa;39CGAR/test
hashes are the72418383code. Binary5b3f99b0face42932d57c93a364c859c995294b74580da4f7afb65d4a936b991.

441e8a1560f71e68a37ae7db070a6a83bec70be3: optional
CGAR_WINDOW_PROGRESS_TIES=1. A*equalf prefers lowerh before greaterdepth;
complete group/seed/island equalcost prefers lower remainingcost. Totalobjective
never worsens. Remainingcost diagnostics recorded. Small independent optima agree
in bothmodes, but those small fixtures have0changedtiepaths; production/full maps
exercise behavioral differences. Build-v3 binary
04e52d52018e0722f4549c67ac45ed8febb393e292f150f79e3640a72f7eb756.

Latest339718b3bc0364afb1f780033304b5f88d6d3ce9: optional
CGAR_WINDOW_PROTECTED_PREFIX=1. Exact physical first primary/support actions remain
immutable. Compatible later forecasts may move within current pocket permissions.
Active transactions, witness cells, parked/capacity bootstrap paths stay frozen.
Adapter preserves real pinned commitments/next_ proposals even on turningsteps.
Future paths are forecasts, not simulated future CGAR primary/recovery decisions.
Full tests PASSED,39source/test hashes match, build-v4 binary
88e3e965e2da7d7102c9579803ed6612fc8e7bb4870985be072c146eac3758f7.
Independent960layeredoptima;130partialgroup rollbacks;8400serial/parallelactions;
1213serviceevents; default/no-opidentity, actualprimary/pocket/capacityfixtures,
changedforecastwithunchangedfirstaction, deadlineafterimprovement all pass.
Frozen runs/cgar-rolling-window-build-v4-20260921; proof rolling-window/build-v4/.
No production edits pending; v5fullprefix matrices are running.

Other options: CGAR_WINDOW(H6-32;0off), KEEP(default6,<H), ITERS(default128perisland),
NODES(default2048persinglerobotsearch), GROUP4, WORKERS4, THREADS4, WAIT_COST0
(resolves forwardbase; positive override supported). Preprocess uses
CGAR_TEMPORAL_CHAIN_MB/THREADS (64MiB/4onrandomprofiles). Five-step CHAIN_MODE must
beoff withwindow; staticguidance required, legacywarm/promise/guide/next-errand/
neutral-tail incompatible. Optionalflags parse strictly; nondefaultworkwhenoff
rejects. Cgar README describes semantics/diagnostics. Analyzer validates receipts,
complete iterations, costs, movement,source/binary/resources; accepts old receipts.

## Completed window evidence: all negative vs current frontier

Full600/600/800horizons, seedsR01=2,R02=4,R03=0,4physicalcores,strict1s.
Firstcurrentfield128attempts/island:629/955/1256 versus635/1197/1902controls;
allcontrol wholetraces match earlier source. Uniform firstarms used reference0and
were rejectedbeforeexecution. Preserve six flag_mismatch failures. Corrected
uniform profiles retain reference1withuniform1; actual field is fingerprinted
constant20. Do not disable the sparseRANDOM explicitreferenceguard.

Uniform correctedv2: seedcontrols625/1026/1472;window128=604/946/1377;
window512=623/1038/1493. Morework helps but nofrontiergain.

V3 independentRNG/seedrollout:
R01 currentcontrol635,uniformwindow512wait623,512roll621,2048roll621.
R02 current1197,wait1032,roll1049,2048roll1060.
R03 current1902,wait1390,roll1386,2048roll1604.

V4 progress ties:
R01 uniform2048control621,ties635,currentfield2048ties642,current512ties642.
R02 uniformcontrol1060,ties1073,current2048=1144,current512=844.
R03 uniformcontrol1604,ties1418,current2048=1829,current512=1505.
Alluniform2048wholecontrol traces matchv3. Max500.4ms acrosscompletedv4runs.
Windowbest642/1144/1829 remainsbelowselected647/1197/1902.
Evidence rolling-window/first-results.json and rollout-progress-results.json.

V5 next: samecurrent2048tiescontrol, protected-prefixcurrent2048,
protected-prefixcurrent4096, protected-prefixuniform2048. Exactmatchedablation.
If morework helpsbut4coreslimits, use8physicalcores with8islands/threads and fixed
4096attempts each. Firstrepeatcontrol on8cores should preservewholetrajectory.
Do not interpret lowest surrogate score as throughput; that already failed.

PILOT read-only sources de0b763:window.cpp/engine.hpp;ac5cbc9:engine.cpp/engine.hpp.
Reference facts/hashes in pilot-transfer/. Sparse/medium uses20window+LNS; crowded
usestrue2phasepipeline andcommonfuturecontinuations. No copied alternative solver.
Next crowded structural candidate: common-future evaluation of completeCGARplans,
not another collection of unrelated5stepstarts. A read-only next-move deferral
counter could distinguish replanningprocrastination from realtraffic blocks.
Long fixed protected tails were an explicit conservative approximation, nowv5.

## Other recent comparisons and diagnosis

SORTATIONparallelstarts:150894control,2x1M150830,4x500k150685,4x1M150798.
Control wholetrace repeats. Runtimealternativev2=150830/max888.48ms, only64tasks
belowfrontier; control max998.94ms. Frozen JSON sortation/runtime-alternative-v2.
This is betterheadroom thanold150780/max900.70ms butnotfrontierpromotion.
Rematchingbudget2/4/cadence1 isrunningonthis150830alternative.

R05 eightcorecomposition with32starts:2956control,8parts12rounds2926,
8parts16rounds2893,4parts12rounds2946; alllose. Controlswholetrace repeat.
OldregionalpeakretentionlosesR05mean, staysOFF. Baseline32startdiversity itself
replicated +1.95% beforechain transfer. GAMEpickup12replicates+1.88% over3seeds;
selected24447alreadyabove+5target24438. Halfits6500robotsgetnonewtasks.

Cities andSortation have under2%waiting (CITY01~0.7%,CITY02~1.7%,Sortation~1.1%).
Mostrobot timeisforwardtravel. FleetreductionislessmotivatedthaninGAME; investigate
triplength/assignment. Thisisnotathroughputupperbound. Reproducible summary
motion-bottleneck-20260921.json. Earlier CITY02horizon/objective/order alllost;
R03extra finite rematching/constructiondiversity alllost. Preserve negatives.

## Owned jobs and build/eval recipes

Authoritative allmaps/ACTIVE_JOBS.json. Pendingat08:03UTC:
SORTATIONruntime-rematch matrix8902097/verifier8902098,8cores,source15640f8d.
R05horizonrep seeds2/4 matrix8902150/verifier8902151,4cores,source7783736.
R05horizonselectedscan seeds6/8/10/12 matrix8902152/verifier8902153,4cores,source7783736.
R01prefixv5 matrix8902176/verifier8902177,4cores,source339718b3.
R02prefixv5 matrix8902178/verifier8902179,4cores,source339718b3.
R03prefixv5 matrix8902180/verifier8902181,4cores,source339718b3.

Raw runs/cgar-SUFFIX-20260921; reports allmaps/results/SUFFIX. Allanalysis tags
factor-analysis. Readfinishedverification.json beforepromotion. NFSnegativecache
canbrieflyhidefiles; listparent/retry, neverrerun a finishedsolver for that.

Buildcommitfirst, then experiments/assignment-20260918/build.py --output runs/NAME
--shared-host --scheduler-slots-per-core2 --memory-gib-per-slot4
--hosts research44.grid.gsb research57.grid.gsb. No sourceeditsduringbuild.
Attest39source/test hashes againstsourcecommit,binarySHA,fullregression,4physical,
noquota. Sharedheadmayincludeotheragentcommits; fingerprintsidentifyexactCGARcode.

Matrix tools/benchmark_matrix.py --output RAW --binary FROZEN/lifelong
--source-manifest FROZEN/build.json --variants PROFILE --seeds SEEDS
--instances INSTANCE --parallel-suites4 --jobs-per-suite1 --cpus-per-instance4
--scheduler-slots-per-core2 --memory-gib-per-slot4 --time-limit-ms1000
--runtime01:45:00 --hosts research44.grid.gsb research57.grid.gsb
--expected-cpu-model 'AMD EPYC 9354 32-Core Processor' --shared-host --trick INSTANCE.
Eightcores:memory2. Space flags and values normally; above shorthand is prose.
Analyzer allmaps/analyze_matrix.py --raw RAW --output REPORT --commit COMMIT
--variants PROFILE --inputs allmaps/chain-potential/inputs/<instance-lower>.json
--control NAME --trick INSTANCE --hold-job ACTUAL_JOB --seeds SEEDS.
R05requires --allow-random05;8coresrequires --cpus-per-instance8. Parse actualqsubID,
neverguesshold. Preflightguardsinstances/profiles/seeds/trick/cores beforecreating
an attempt. Actualmatrixinputs werecorrectinall earlier analyzer mistakes.

Checkpoint completed ownedreports/profiles/logs, never in-progressfragments.
Push authorized checkpoint and continue. No formal goalstatus change.
