# RANDOM-05 continuation

Updated: 2026-09-20 18:14 UTC.

The goal is ACTIVE. The user asked us to keep pushing throughput until evening;
waiting time is secondary. Do not impose a fairness constraint that sacrifices
throughput. This is an independent Random05 combined solver guided by the
colleague's pasted log; their code and private inputs are unavailable. The target
is roughly their27–28% matched gain over NMS on four cores, then improve further.
This turn made progress: new verified records, exact CPU optimization, audits,
paired seeds and new full benchmarks. It is not blocked.

## Verified records

- Overall /32 workers: **3743**, source **d6a3e0f**, build-v59,
  K8192/B8/start2/local0, generations4/elites8/persist8, planner seed0,
  future elite blend0. Mean255ms, max401ms, RSS591MB. **+18.0% vs NMS32=3172**.
  Evidence: results/elite-futures-seeds-split-full-v59/k8192-elite-future0-seed0/.
- Four physical cores: **3655**, now faster source **60c5f9b**, build-v60,
  K5120/B8/start2/local0, generations4/elites8/persist8, seed3, blend0,
  cache512 and kinematic mask ON. Mean702.258ms, max825.729ms, RSS449488KiB
  (460MB). **+25.4% vs strongest NMS4=2914**. Every action/schedule/event/task
  equals the older source05559b7/cache64 record (mean790.738ms,max931.132ms).
  Exact implementations save11.2% mean; in the v60 cache512 mask-off/on pair,
  means748.789/702.258ms, a6.2% reduction. best-four-cores.json selects v60.
  Evidence: results/kinematic-mask-split-full-v60/k5120-persist8-kinematic-mask1/.
- Intermediate overall records, all logged:3705 (05559b7,K8192/B8,E8/P8,seed3),
  3724 (d6a3e0f,same plus future elite blend0.5),3741 (05559b7,K16384/B16,E8/P8,
  seed3; mean452ms,max563ms,RSS593MB). Do not discard intermediate frontiers.
- NMS4 strongest repeat2914 (others2902/2903), NMS32=3172. Both retain the
  documented constructor fix; four-worker build only changes THREADS32->4.
  All claimed local gains match input hashes, allocation, CPU model and deadline.

All canonical frontiers are full2000 steps, strict1s,30s preprocessing,
32 decimal GB address-space guard, no planner/scheduler errors or timeouts.
4 workers=4 physical cores;32 workers=16 physical cores using SMT2. EPYC9354,
GRID affinity checked, shared hosts allowed. Published server comparisons are
indirect. Work is fixed; timeout exits124 rather than returning partial search.
New relaxed5s development runs are separate and MUST NOT enter the strict
frontier or receive direct percentage comparisons to the1s NMS reference.

Common best settings: generated field15, average/normalize, power3,
betweenness.75, penalty2.4, one flipped pair/flipseed5; turn.6/wait.5;
depth8/noise200/dispersion.8; Hungarian1000, guided matching, keep.5/length.25;
horizon2000/triage1.5; per-step RNG. Exact flags: shareprefix,packedorder,
fastdispersion,scratchreuse,goalcache,radixorder,candidatecache. Mutation and
future mutation.3, decay1, risk0. --trick RANDOM-05 gates field/horizon/agecap.
Search and cache mechanisms are general; records still use those declared tricks.
Use best*.json for exact measured flags, including profiling, not code defaults.

## Replication and fresh inputs

K5120/E1/P1 seeds0–4:3611/3526/3648/3637/3608, mean3606.0.
K5120/E8/P8:3583/3495/3614/3655/3602, mean3589.8, only1/5 positive pairs.
Thus3655 is a selected best, not a replicated mean improvement.
K8192/E8/P8 blend0 seeds0–4:3743/3492/3730/3705/3513, mean3636.6.
Blend0.5:3577/3610/3693/3724/3660, mean3652.8 (+0.45%,3/5 positive).
Small mean difference relative to seed variation. These are planner seeds on
ONE development input, not independent start/task instances. Paired summary:
results/elite-futures-seeds-split-full-v59/paired-summary.json.

Fresh validationV3 COMPLETE: frozen protocol81bdfbd, source6ce9312/build-v52,
K5120/B8/gen4/E1/P1/plannerseed3 (3637 development run). Input50005:3698 vs
NMS2926/2900;50006:3619 vs2906/2831. +26.38%/+24.54%, aggregate **+25.46%**
against the stronger repeat. All6 full four-core runs pass. Never substitute
later records. All inputs50001–50006 MUST stay outside tuning. New validation
requires a new frozen protocol and new inputs. See FRESH_VALIDATION_V3.md and
results/fresh-validation-v3/audit.json. V1/V2 gains11.8%/17.1% used other inputs;
changes in gain across protocols are not isolated paired effects.

## Latest implementations and controls

v55 05559b7: persistent priority vectors, scores/actions/reservations re-evaluated.
v56 0bd0a41: mutation decay; tested values lose at main budgets. Keep1.
Stronger constant root mutation.5/1 also loses. Keep.3.
v57 0295ae9: FIRST_K, an explicit smaller initial work count, never clock-based.
E1/P1 first5120/regular5632 and5888 score3650/3591 on four cores;6144/cache64
fails step2=1000.533ms. All preserve fixed work and prior failures.
v58 bdc051f: cache capacity64/128/256/512 preserves3637 full trajectories.
Four means791/778/776/772ms. First5120/regular6144 with128/256/512 allvalid3545,
max989/997/982ms, exact fast32 counterpart. More K is not monotonic improvement.
The separate E8/P8/cache512/profile full run is valid3655, mean768.778ms,
max895.654ms; full trajectory equals original3655. Evidence current-policy-profile-
four-split-full-v58; this diagnostic is complete.

v59 d6a3e0f: R05_FUTURE_ELITE_BLEND, default0, range[0,1], requires persist>=2
when positive. At the original future-mutation mask positions blend random
values with a previous-step elite vector, one fixed vector per continuation.
Keep the same RNG draws/masks; empty history falls back to random futures.
Only priorities persist, not stale actions/scores. Tests8.63s pass.
K5120 blend0/.5/1=3655/3528/3506;K8192=3705/3724/3594.
Full default controls exactly reproduce old3655/3705; paired seeds above.
SHA7ab2ed25fa8795ae367c58a9104e95b59a3648b03f21cdbbb023a8721b5d40c4.

v60 60c5f9b: R05_KINEMATIC_MASK, default0. Cache executable candidate indices
under the existing pose/stage/task/moving-state key. Visit allowed indices in
original order, keeping all reservation, swap, fallback and expansion checks.
Compile-time optimistic/kinematic modes remove dynamic branching. Tests9.44s
pass, including push costs, intent rotation, pinned components, cache eviction,
expansion limits, task turnover and worker counts. Full mask off/on controls
EXACTLY preserve3655four and3724on32 workers. On32, mean231.516->221.944ms.
SHA cbc99ea0d2326a3dc2298ef18580e4872aa3ddd22be06fe1e68b68d306ab662a.

## Failures: preserve and classify correctly

kinematic-larger-four-split-full-v60, source60c5f9b, cache512/mask1/E8/P8:
- job8900396 K6144/blend0: step0=1036.945ms, exit124.
- job8900397 K6144/blend.5: step0=1839.045ms, exit124.
- job8900398 K6400/blend0: step1=1088.306ms, exit124.
All use FIRST_K5120; initial work is the same as the valid825ms mask control.
These failures do not prove the larger later-step work caused the timeout.
- job8900399 K6400/blend.5: solver NEVER STARTED. GRID exposed64 physical cores
  instead of4; the guard refused. Qacct exit1, wall0, CPU.074. No native/usage file.
  results/kinematic-larger-four-split-full-v60/...blend0.5/preflight-rejection.json
  hashes spec/allocation/scheduler log. attempt-audit.json records all4 terminal.
  Do not leave this classified as pending just because frozen runner wrote no summary.
The original batch summary has3 solver rows; the fourth is a separate preflight
rejection. Do not edit frozen runner/spec or fabricate a throughput score.

continuation-budget-next-split-full-v60:
-8900411 K24576/B24/blend0: step0=1101.406ms, exit124.
-8900413 K32768/B32/blend0/first16384: step2=1148.664ms, exit124.
Preserve these strict failures; no silent retry or promotion of partial runs.

## Still running / newly submitted

Check qstat and actual outputs before acting; summaries can lag NFS.
- persistent-budget-seeds-split-full-v55: K16384/B16/E8/P8 seeds0/1/2/4,
  jobs8900401/402/403/404 are COMPLETE. Allseeds0–4=3705/3617/3715/3741/3567,
  mean3669.0 (+0.89%,3/5positive vs K8192/B8 mean3636.6). Best3741<3743.
  Compact evidence and paired-summary.json are archived; no new frontier.
- kinematic-larger-four-relaxed-split-full-v60: jobs8900405–408, first5120 then
 6144/6400, blend0/.5, four cores, declared **5s** budget. All four configurations
 are explicit diagnostic copies under the user's prior relaxed-budget permission.
 Record full throughput/timings separately; require strict confirmation or more
 optimization before calling a strict1s frontier. Work stays fixed.
- continuation-budget-next-split-full-v60:8900409 K16384/B16/blend0 (control3741),
 8900410 same blend.5,8900412 K24576/B24/blend.5. All32 workers,cache512,mask1,
 source60c5f9b,strict1s. Other two jobs in this batch failed as listed above.

Collect with split_grid.py; archive only compact batch/spec/allocation/summary/
completion JSON using direct one-/two-level globs. Never recursively copy NMS
working directories. Raw results/native logs/binaries stay ignored. Do not
restart a job merely because qstat disappears; inspect original evidence first.

## Audits and next ideas

Main frontier audit65 rows PASSED; checks source,
input, allocation, binary and trajectory hashes, strict limits, all2,000 steps.
audit_progress.py generates WAITING_PROGRESS.md. The matched waiting manifest/
report is results/task-waiting-frontiers-20260920T1612/. Update it for every best.
Current3743: completed maximum1939, mean422.77,p95=1032; initialunfinished142,
unopened95. Current3655: max1916, initialunfinished141/unopened98.
NMS4 max1997, unfinished219;NMS32 max1976,unfinished206. All leave step0 orders
unfinished at2000. Eventual maximum wait is unknown and at least2000; no bound.

Current action/suppression audit: results/action-audit-current-v55/.
All four trajectories replay safely. On common completed tasks, our four-core
policy saves turns/forwards but adds16.99 loaded waits per task; net13.36 fewer
loaded steps. The32-worker pair adds24.65 waits, net8.54 moreloaded steps.
These are observational, not causal/recoverable-throughput estimates.
Reconstructing triage reproduces all20 native samples: loaded waits active/
suppressed520097/79060 four,520986/81395 on32. About13% are suppressed-goal waits.

Two next ideas are documented in RESEARCH.md, NOT IMPLEMENTED:
1. Exact sparse bit masks for geometric cycle eligibility; cycle scans take
 roughly13% of sampled policy time. Preserve ring order and clear eligibility
 after accepted cycles. Need tests spanning64-bit boundaries, holes and overlap.
2. Fixed-budget staged continuation allocation, inspired by Sequential Halving
 (Karnin/Koren/Somekh2013, primary PMLR paper read). First measure whether cheap
 2/4-branch scores retain finalists under8/16 branches. No guarantee transfers
 to our correlated/control branches. A possible K5120 design per generation is
 256 roots x2 branches, then64 survivors x12 more =1280 evaluations, fourgen.
 This needs empirical ranking diagnostics before implementation; not anytime.
A small harness follow-up could record preflight exceptions explicitly so failed
allocation jobs do not look pending forever. Keep the allocation guard intact.

## Fable and workspace

Approved exact79KB payload attempted15:01UTC viaClaudeCode,max, persistent session
27a4316e-b79d-46cf-86b4-41b0f558938a. Provider429/model_requires_usage_credits;
NO REVIEW,zero usage. User informed; optional review does not block local work.
Payload runs/random05/fable-review-01/payload.txt SHA
 ded0889fcaa2cb82a3f4fd9d5dc46f737bdf50cf7a9e466bb66f9114f9661d0f.
Older7506a snapshot; do not silently replace authorized bytes. Retry exact payload
with --resume in a new outputdirectory if credits return, preserving failed attempt.

Shared main with Warehouse agent. Only edit/stage/commit random05/ and
RANDOM05_PROGRESS.md. Leave all other files/staged changes/jobs alone. No new
subagent delegation. Publicfywu85/lorr; push authorized. Current source60c5f9b;
latest evidence/checkpoint is being committed and pushed now; check git log
and origin/main for the final result.
Use git commit --only ... -- random05 RANDOM05_PROGRESS.md. Shell needs
require_escalated because of bwrap namespace failure. Python3.7; env/bin/python
for NumPy. apply_patch broken; use Python. No local AGENTS or RTF found.
Push: GIT_TERMINAL_PROMPT=0 GIT_ASKPASS= git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main.
