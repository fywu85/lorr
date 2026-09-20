# RANDOM-05 continuation

Updated: 2026-09-20 15:44 UTC.

Active goal: approach the colleague's roughly 27–28% matched gain over NMS on
four cores in the combined track. The goal is NOT complete. Private colleague
code/inputs are unavailable; their pasted log is the guide. Never compare their
raw scores directly to ours on different inputs.

## Verified records

- Overall: **3,657**, 32 workers /16 physical EPYC9354 cores, source **d933023**,
  build-v46, K16384/B8/start2/local0/generation1/planner seed3. Mean607ms,
  max726ms, RSS364MB. **+15.3% vs NMS32=3172**. Evidence:
  results/continuation-larger-split-full-v46/k16384-b8/summary.json.
- Four cores: **3,562**, source **f9b1143**, build-v47, K2048/B8/start2/local0,
  four generations, planner seeds4 and0. First completed seed4 mean455ms,
  max574ms, RSS285MB; both full trajectories exactly match32-worker runs.
  **+22.2% vs strongest NMS4=2914** (other repeats2902/2903).
  Evidence: results/generation-confirm-four-split-full-v47/; best-four-cores.json
  selects seed4, which completed first. Source version is NOT the latest kernel.
- No known-horizon cutoff: four cores3285 (+12.7% vsNMS4), sourcee896201/build-v45,
  K2048/B8/gen1/seed3; 32workers3408 (+7.4% vsNMS32), sourced933023/build-v46,
  K8192/B8/gen1/seed3. Both still use explicit guidance trick. Same-settings
  known-horizon scores3501/3596; cutoff effects+6.6%/+5.5% over cutoff-free scores.
- All frontier rows are full2000, strict1s, no errors/timeouts, <=32decimalGB.
  The runner imposes a32decimalGB process address-space guard inside the job;
  outer GRID allocation may show32GiB. No silent partial search; timeout exits124.
- RANDOM05_PROGRESS.md has **56 audited frontier rows**. Keep producing source
  commit, UTC timestamp, seeds, tricks, allocation, exact evidence for every best.
  best.json/best-32-workers.json=3657, best-four-cores.json=3562.

Common settings: generated field15, one direction pair flipped (flipseed5),
contrast2.4, turn.6, wait.5, depth8, noise200, dispersion.8, exact guided matching,
keep.5/length.25, horizon2000/triage1.5, per-real-step independent RNG. Field and
horizon are declared with --trick RANDOM-05. Continuation/caching/sorting changes
are general mechanisms but recorded frontiers retain the declared tricks.

Repeated development-input evidence (planner seeds0–4, same task/start input):
K2048/B8/gen1 =3478/3443/3522/3501/3472, mean3483.2;
K2048/B8/gen4 =3562/3499/3514/3520/3562, mean3531.4 (+1.4%,4/5 pairs positive).
K8192/B8/gen1 =3528/3579/3457/3596/3580, mean3548.
K3584/B8/gen2 also reproduces3552 on4cores, full trajectory identical.
Gen4 is not uniformly better: K8192=3582, K16384=3495, both below gen1.

## Fresh validation (complete and frozen)

V1: sourceb824f5d/protocol22e7cd1, seeds50001/50002, ours3386/3178 against
NMS2920/2957 and2898/2915: +14.5%/+9.0%, aggregate+11.8%.
V2: source**e896201**, protocol**e6b2dbe**, seeds50003/50004 generated AFTER
protocol freeze. Exact older3501 four-core config, seed3/K2048/B8/gen1.
Results3494 vsNMS2946/2984, and3387 vs2892/2875: **+17.1% each and aggregate**.
All6 valid; audit checks source/binary/input hashes, CPU topology/model/quota,
full horizon, strict1s and RAM. See FRESH_VALIDATION_V2.md and
results/fresh-validation-v2/audit.json. Keep all50001–50004 inputs OUT of tuning.
Do not replace the frozen candidate with3562 or3657. V1 andV2 use different
inputs, so their gain difference is not a paired causal estimate.

## Recent implementations and evidence

- v48 **1c9795a**: optional phase profiling, no policy change.
- v49 **bd01f97**: R05_GOAL_CACHE=1 shares immutable minimum-goal-heading rows
  and final-errand cost rows; map-only preprocessing, no hidden-task access.
  Full on/off32-worker and on4-core controls all3520 with exact trajectories.
  First-step matching matrix28.6->11.6ms; Hungarian solve remains~63ms.
  This does not make K4096 fit on4cores by itself. Defaults off.
- v50 **5fa9ff8**: R05_POLICY_PROFILE samples one in64 advance calls on first5
  steps/every100th. Sampled worker elapsed times, not isolated CPU utilization.
  Candidate ranking~24%, priority order~17% of sampled policy work.
- v51 **9ad642e**: R05_RADIX_ORDER=1 exact stable four-byte sorting of priority
  floats, ties by agentID; signed-zero normalization and finite fallback retained.
  Regression tests pass. Identical prefix actions/events; sorting sampled time
  -70%, look-ahead roughly970->850ms first /952->830ms at100. Full32-worker3555
  exact; strict4-core confirmation is pending. Defaults off.
- v52 **6ce9312**: R05_CANDIDATE_CACHE=1, bounded64 slots per agent per worker,
  about157MB extra at800agents/32workers. Key: epoch, robot, chain pointer,
  stage, pose and moving flag. Clear by epoch each real step. Push-price>0
  bypasses cache because it depends on another robot. Priorities and PIBT
  collision resolution are recomputed. Dense turnover/virtual tasks, alternative
  wait/rotation policies, push fallback and worker counts tested. With radix,
  prefix look-ahead889->712ms initially /878->677ms at100, exact trajectory.
  Full32-worker K4096/B4 control3555 exact; full4-core K4096 andK5120 underway.
  Defaults off until full evidence. All builds v48–v52 pass regressions.
- v53 **94a8519**: signed continuation coefficient: positive prior variance
  penalty, zero exact mean, negative tests optimism about future re-optimization.
  Every configured branch still evaluated. Tests pass5.26s. Eight full4-core
  cases0/-.25/-.5/-1 atgen1/4, K2048/B8, **seed3** with all exact optimizations.
  No benefit claimed yet. Positive penalties at earlier budgets all lost.
  BinarySHA9b4e8287a192dc94891203a19b0ecc28b1e0f2b0858c5c391cc2464bf7afc8c8.

Earlier versions: v42a6ad284 continuation mean; v44da00823 exact prefix reuse;
v45e896201 packed priority/sparse dispersion; v46d933023 scratch reuse;
v47f9b1143 continuation variance penalty. Their completed evidence remains
under results/ and all build snapshots/completions/source hashes under runs/.

Recent rejected guidance: fields33–48 at production K2048/B8/gen4 all lose to
field15=3520; bestnewfield47=3444. All17 valid, field15 full trajectory identical
through v52 caches/sort. See flow-expansion-33-48-split-full-v52. Fields1–32
were previously tried; no blind repeat. Mark new map tuning as trick in commits.

## Pending jobs

- `radix-split-full-v51` / `radix-k4096-b4-workers4`: job8900126.
- `ranking-cache-split-full-v52` / `ranking-k4096-b4-workers4`: job8900137.
- `ranking-cache-split-full-v52` / `ranking-k5120-b8-generations1`: job8900138.
- `ranking-cache-split-full-v52` / `ranking-k5120-b8-generations2`: job8900139.
- `ranking-cache-split-full-v52` / `ranking-k5120-b8-generations4`: job8900140.
- `optimistic-continuations-four-split-full-v53` / `generations1-coefficient0`: job8900194.
- `optimistic-continuations-four-split-full-v53` / `generations1-coefficientminus0.25`: job8900195.
- `optimistic-continuations-four-split-full-v53` / `generations1-coefficientminus0.5`: job8900196.
- `optimistic-continuations-four-split-full-v53` / `generations1-coefficientminus1`: job8900197.
- `optimistic-continuations-four-split-full-v53` / `generations4-coefficient0`: job8900198.
- `optimistic-continuations-four-split-full-v53` / `generations4-coefficientminus0.25`: job8900199.
- `optimistic-continuations-four-split-full-v53` / `generations4-coefficientminus0.5`: job8900200.
- `optimistic-continuations-four-split-full-v53` / `generations4-coefficientminus1`: job8900201.

Do not restart jobs because they disappear from qstat. NFS summaries can lag;
inspect the original job/completion/qacct. No silent restarts or hiding failures.

## Next useful steps

1. Collect the pending full strict4-core tests. Any K5120 deadlines failures must
   remain excluded. Their first step passed, which is NOT full validation.
2. For valid new bests, archive compact evidence, compare full trajectories across
   worker allocations when applicable, add source/timestamp/seed/compute/tricks,
   update best*.json and run audit_progress.py. Never promote prefix scores.
3. Finish the optimistic-score A/B. The zero controls should retain3501(gen1)
   and3520(gen4), seed3. Judge full throughput, not prefix ranking.
4. If an optimized high-K configuration wins, validate candidate seeds and timing
   before a new held-out protocol. Existing fresh seeds stay untouched for tuning.
5. Goal not complete: development+22.2%, fresh+17.1%, versus colleague~27–28%.

## Fable review status and authorization

The user explicitly approved the exact prepared79KB payload in the latest turn.
We executed it at15:01UTC through claude-fable-5-1, max, tools disabled, persistent
session27a4316e-b79d-46cf-86b4-41b0f558938a. The provider returned
model_requires_usage_credits / HTTP429 / out of usage credits. Zero usage/cost,
NO REVIEW. This is a provider quota failure, not a remaining approval rejection.
Original payload: runs/random05/fable-review-01/payload.txt, SHA256
 ded0889fcaa2cb82a3f4fd9d5dc46f737bdf50cf7a9e466bb66f9114f9661d0f.
It is an older source snapshot7506a from08:16UTC, not the latest kernel.
Keep the session and exact approved payload for a retry after credits return;
do not re-ask approval for the same payload. Preserve the failed attempt.
The runner disallows re-executing an output dir with status.json, so a retry
needs a new output directory with copied exact prepared payload/snapshots/meta,
then --execute in the same session (initialized=true, uses --resume). Do not
silently prepare a different source payload under this exact-payload approval.
Latest metadata/error is in runs/random05/fable-review-01/; optional review does
not block local progress. No subagent delegation is authorized.

## Workspace and tools

A Warehouse session shares main/workspace. Only edit/stage/commit random05/ and
RANDOM05_PROGRESS.md. Leave cgar/, WAREHOUSE_PROGRESS.md, experiments/construction-
20260918/ and their jobs alone. git commit --only -- random05 RANDOM05_PROGRESS.md
keeps unrelated staged work out. Publicfywu85/lorr push authorized; no visibility
change. Most exec calls require_escalated because bwrap namespace creation fails.
Python3.7; generator uses env/bin/python. apply_patch is broken; use Python edits.

GRID: split_grid.py submit/collect, one case/job, checked physical affinity/model.
For4cores use research33*|research39*, EPYC9354. Wide32worker hosts are documented
in existing batch manifests. Shared hosts allowed; avoid causal isolation claims.
Archive only batch/spec/allocation/summary/completion/equivalence JSON; raw
results/native logs/binaries stay ignored. audit_progress.py verifies all frontier
source/input/binary hashes, full validity, matching budgets, latency and RAM.

Push: GIT_TERMINAL_PROMPT=0 GIT_ASKPASS= git -c credential.helper= \
  -c 'credential.helper=!gh auth git-credential' push origin main
