# RANDOM-05 continuation

Updated: 2026-09-20 14:42 UTC.

Active goal: approach the colleague's reported roughly 27–28% matched gain over
NMS in the combined track. Their code and private instances are unavailable.
The goal is not complete. Raw task counts on different inputs are not matched gains.

## Verified development-input records

- Best: **3,596** on 32 workers / 16 physical EPYC 9354 cores. Source `d933023`,
  build-v46, K8192/B8/start2/local0, planner seed 3. +13.4% versus matched
  NMS32 = 3,172. Mean 308 ms, max 421 ms, RSS 447 MB. Not verified on four cores.
- Four cores: **3,501**, source `e896201`, build-v45, K2048/B8/start2/local0.
  +20.1% versus strongest matched NMS4 = 2,914 (other repeats 2,902 and 2,903).
  Mean 459 ms, max 579 ms, RSS 285 MB. Every action, schedule and task event
  matches its 32-worker trajectory. `best-four-cores.json` freezes the settings.
- Earlier four-core 3,492 also reproduces exactly through three implementations:
  mean 627 -> 578 -> 534 ms. These are shared-host observations, not isolated
  causal timing estimates. The final implementation uses prefix reuse, packed
  priority order and sparse dispersion counting.
- All frontier rows: full 2,000 steps, strict 1s, <=32 decimal GB, no errors or
  timeouts. Timeout exits 124; fixed work never silently returns a partial search.
- `RANDOM05_PROGRESS.md` has 49 audited history rows, with producing commits,
  timestamps, seeds, tricks, CPU allocation, matching budgets and evidence.
  `best.json` / `best-32-workers.json` point to 3,596; four-core best stays separate.

Common configuration: generated flow field 15 with one direction pair reversed
(flip seed 5), contrast 2.4; depth 8, noise 200, dispersion 0.8, wait 0.5, turn
0.6; exact guided matching, keep 0.5, length 0.25; horizon 2,000 / triage scale
1.5; independent RNG per real step. Guidance and known horizon require explicit
`--trick RANDOM-05`. Continuation search and exact code optimizations are generic,
but the frontier still uses those declared tricks.

K2048/B8/start2 across planner seeds 0–4 gives 3478/3443/3522/3501/3472,
mean **3483.2**, versus the preceding mean 3338.4 (+4.3%; all five positive).
Same development input, not independent task/start instances. K8192/B8 extra
planner seeds are now running.

Previous cutoff-free record remains 3,197 on both allocations, source b824f5d:
+9.7% versus NMS4, +0.8% versus NMS32. New continuation cutoff-free performance
has not yet been measured; this is a useful next ablation.

## Fresh validation

V1 is complete: frozen source b824f5d at protocol22e7cd1. Seeds50001/50002 give
3386 vs NMS2920/2957 (+14.5%), and3178 vs2898/2915 (+9.0%); aggregate+11.8%.
All6 valid on4 physical EPYC9354 cores. Never tune on these inputs and still call
them held out. See FRESH_VALIDATION.md and results/fresh-validation-v1/audit.json.

**V2 is running**, frozen before generation at commit **e6b2dbe**:
- Candidate source e896201 / build-v45, the exact 3,501 four-core configuration.
- New seeds50003/50004, unchanged generator, no tuning on these inputs.
- One candidate plus2 NMS repeats per input; use stronger valid NMS per input.
- Four physical EPYC9354 cores on research33/39, 1s, 30s preprocessing,32GB.
- Protocol: FRESH_VALIDATION_V2.md and experiments/fresh-validation-v2-protocol.json.
- Inputs: runs/random05/fresh-inputs-v2/; generation hashes archived under
  results/fresh-validation-v2/generation.json. Not colleague private instances.
- Batch: fresh-validation-v2-split-full-v45; jobs8899954–8899959.
- Audit after all6 finish:

```sh
python3 random05/tools/audit_fresh.py \
  --batch random05/results/fresh-validation-v2-split-full-v45 \
  --output random05/results/fresh-validation-v2/audit.json \
  --protocol-commit e6b2dbe \
  --protocol-json random05/experiments/fresh-validation-v2-protocol.json
```

The audit now verifies source hashes as well as frozen settings, binary/input
hashes, allocation, validity, latency and RAM. Its original V1 invocation still
passes unchanged against the completed real dataset. Report every outcome,
including failures; do not substitute later candidates into the frozen V2 run.

## Source/build versions

- v42 a6ad284: continuation averaging. K total rollouts, B branches per root;
  same first actions and promises, mutation .3 after start1 by default. B1
  preserves the old full trajectory. This produces3492/3501/3555 before optimizations.
- v43 131d6be: optional operation success protection and terminal-turn exclusion.
  Tests pass; full policies remain far below the pipeline and stay off.
- v44 da00823: exact shared-prefix reuse, including task turnover/discount state.
- v45 e896201: packed float priority+agent-ID ordering and exact sparse/hole
  dispersion counts. Full controls match all3555 decisions. Current four-core3501.
- v46 d933023: thread-local reusable policy arrays and removal of temporary
  chosen-vector copies. Tests pass, full3555 controls exact. Current overall3596.
- v47 f9b1143: optional standard-deviation penalty on continuation scores.
  Dense regressions pass; all nonzero full settings lose. Keep risk0.
  No source edits after this commit; later commits add protocol/results only.

Build logs/completions/source hashes are under runs/random05/build-v*/.
All builds above pass regressions. Source v45 binary hash d2d7b43c2566403df40c7ba12316d446bb8a67feebb90db3fa38b66a8b7d2775;
v46 e29c44b90bbcd4f0a11b2738112ca78eef89db63bd0f0e3463c221a7fd309b41;
v47 d73e7331238632166dd4dc7500ff65b3c1769f6788616e86bb773aabe2fb2cff.

## Completed latest comparisons

- Continuation initial/validation/scale batches v42 all complete and archived.
  K1024 B4/B8 five-seed means3392.6/3417.6 vs3338.4, all five pairs positive.
  K2048/B4/start1=3492; K2048/B8/start2=3501; K4096/B4/start2=3555;
  K4096/B8/start2=3548. Extra local trials did not help consistently.
- Operation reference rules v43: control1973, protect2099, finish-move1742,
  both1820, both-moving2008. All valid, none competitive.
- Shared-prefix v44 controls3492/3450 exact. Four3492 exact; K4096/B8 four fails
  at step0 after1133ms. Later3501 attempt onresearch58 fails at step3 after1068ms.
- Continuation refinement v44 complete: start3/4=3449/3389; mutation.1/.6/1=
  3314/3460/3484; B4/start2/mutation.6=3523; depth6/10=3284/3503. No new best.
- Continuation coupling v44 complete: turn.4/.8/1=3480/3451/3394; noise100/400=
  3452/3460; contrast1.6/3.2/4=3395/3482/3499; no-flip3500; fields5/17=3236/3332;
  NMS imported field2961. All12 valid, none beats3501 at the same K2048/B8 budget.
- Hotpaths v45: control/packed/dispersion/both all3555, exact full trajectories.
  Means194/269/192/173ms; CPU time11573/10911/11418/10248s. Packed-only has lower
  CPU time but worse wall time on shared hosts; do not claim isolation.
- Hotpaths four v45:3492 and3501 exact full trajectories; K3072/B4=3454. K4096/B4
  fails step0 after1100ms. Batch complete; best remains3501.
- Scratch reuse v46: control/reuse3555, exact. Means186/246ms, CPU11090/10547s;
  another shared-host timing difference. K8192/B4=3439; K8192/B8=3596. All4 valid.
- Scratch four v46: K4096/B4, K4096/B8, K3840/B4 fail step0 at1084/1042/1019ms;
  K3584/B8 remains running. Preserve failed records; no implicit restart.
- Risk v47: at K2048/B8 weights0/.25/.5/1/2=3501/3332/3390/3305/3386; atK4096/B4
  3555/3454/3499/3392/3369. All valid; default full controls exact. Keep0.
  Generations2/4/8 atK2048/B8=3512/3520/3482. Gen4 four-core confirmation running.

## Pending jobs at checkpoint

- `scratch-four-split-full-v46` / `k3584-b8-four`: job 8899922, state r.
- `fresh-validation-v2-split-full-v45` / `seed50003-ours`: job 8899954, state r.
- `fresh-validation-v2-split-full-v45` / `seed50003-nms-repeat1`: job 8899955, state r.
- `fresh-validation-v2-split-full-v45` / `seed50003-nms-repeat2`: job 8899956, state r.
- `fresh-validation-v2-split-full-v45` / `seed50004-ours`: job 8899957, state r.
- `fresh-validation-v2-split-full-v45` / `seed50004-nms-repeat1`: job 8899958, state r.
- `fresh-validation-v2-split-full-v45` / `seed50004-nms-repeat2`: job 8899959, state r.
- `continuation-larger-split-full-v46` / `k8192-b16`: job 8899963, state r.
- `continuation-larger-split-full-v46` / `k16384-b8`: job 8899964, state r.
- `continuation-larger-split-full-v46` / `k16384-b16`: job 8899965, state r.
- `continuation-larger-split-full-v46` / `k16384-b32`: job 8899966, state r.
- `continuation-larger-split-full-v46` / `k8192-b8-seed0`: job 8899967, state r.
- `continuation-larger-split-full-v46` / `k8192-b8-seed1`: job 8899968, state r.
- `continuation-larger-split-full-v46` / `k8192-b8-seed2`: job 8899969, state r.
- `continuation-larger-split-full-v46` / `k8192-b8-seed4`: job 8899970, state r.
- `generation-four-split-full-v47` / `generations4-four`: job 8899971, state r.

Do not restart jobs just because they disappear from qstat; NFS metadata may
lag30–60s. Check their own summary/scheduler.log/qacct first. Preserve any failed
attempt. Batch manifests list exact jobs and frozen configurations.

## Next useful work

1. Collect pending full runs, preserve failed deadline cases, update every valid
   best with source/time/allocation and explicit tricks. Keep fresh inputs out of
   configuration selection; report V2 in its own table when all6 runs finish.
2. Measure cutoff-free continuation performance. Older3197 applies only to v31.
3. If continuing CPU optimization, profile actual phases before more guesses.
   K4096 four-core misses largely at startup; cannot move task work into
   preprocessing because CompetitionSystem.cpp reveals tasks AFTER initialize.
   `perf` is installed; login perf_event_paranoid=3; no successful profile yet.
   An optional guided-matching cache of minimum goal-orientation distances could
   be tested, but it is only a hypothesis and has not been implemented.
4. Small generation4 gain needs validation. Nonzero continuation risk loses.
5. Run audit_progress.py before promotions and commit only our scope.

## Older evidence and safeguards

Full replay in results/action-audit-v39 validates moves, task locks and1.6M
robot-steps. Old solver has more loaded waits than NMS despite fewer turns.
These are observations, not causal/recoverable-throughput estimates. Event
checkpoints show new gains already by step1000/1500; they do not validate using
short prefixes for selection. Full2000 steps remain the criterion.

Already rejected under earlier regimes: early forward fill; field seeds1–32
except selected15; confidence weighting; age caps/aging/push costs; completion
bonuses; visible-pool virtual tasks; more local search; regional mutations;
reverse-turn penalties; static loads; score blends; startup task-length rules.
See progress log and RESEARCH.md. Recheck only with a specific changed mechanism.

Shared workspace: a separate Warehouse session edits cgar/ and
experiments/construction-20260918/. Only edit/stage/commit random05/ and
RANDOM05_PROGRESS.md. Use git commit --only with these paths. Public fywu85/lorr
push is authorized; do not change visibility. No subagent delegation is authorized.
Most commands need require_escalated because bwrap namespace creation fails.

GRID: checked physical-core binding/model/quota, shared hosts allowed. Keep
four-core timing comparisons on research33/39. Verified EPYC9354 wider hosts for
32-worker runs:32,33,34,35,36,37,39,40,41,42,45,46,47,48,49,50,51,53,54,55,56,58.
Do not alter Warehouse jobs. Raw runs/binaries stay ignored; copy compact JSON
summary/spec/allocation/completion evidence only.

Optional Fable exact-source payload approval remains pending after auto-review
rejection. Do not transmit/repackage it without the specific approval. Local
work is unblocked. Reserved uninitialized session27a4316e-b79d-46cf-86b4-41b0f558938a;
payload runs/random05/fable-review-01/payload.txt, SHA256
 ded0889fcaa2cb82a3f4fd9d5dc46f737bdf50cf7a9e466bb66f9114f9661d0f.
