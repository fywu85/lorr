# RANDOM-05 continuation

Updated: 2026-09-20 14:01 UTC.

Active goal: approach the colleague's roughly 27–28% advantage over matched NMS
in the combined track. Archived RANDOM-05, 800 robots, 2,000 steps. Their code
and synthetic instances are unavailable. The goal is not complete. Absolute
scores from different inputs are not matched gains.

## Current verified records

- Overall: 3,555 tasks on 32 workers / 16 physical EPYC 9354 cores, source
  `a6ad284`, build-v42, K4096/B4/start2/local0, planner seed 3. +12.1% versus
  matched NMS32 = 3,172. Four-core feasibility is unverified.
- Four cores: 3,422, same source, K1024/B8/start1/local0. +17.4% versus strongest
  NMS4 repeat = 2,914. Mean 322 ms, maximum 435 ms, peak RSS 282 MB. Every action,
  assignment and task event matches its 32-worker run.
- K2048/B4/start1 = 3,492; its four-core repeat is still running.
- K2048/B8/start2 = 3,501; its four-core and planner-seed checks are submitted.
- All successful frontier runs use full 2,000 steps and strict 1s deadlines.
  A timeout exits 124, never silently returns a partial search. Memory <=32 GB.
- Log: RANDOM05_PROGRESS.md, with timestamps, producing commits, CPU allocation,
  explicit tricks, and evidence. Machine-readable best*.json freezes each best.

Shared configuration: generated flow field 15, one directional pair reversed
(flip seed 5), contrast 2.4; depth 8, noise 200, dispersion 0.8, wait 0.5, turn
0.6; exact guided matching, keep bonus 0.5, length weight 0.25; horizon 2,000,
triage scale 1.5; independent RNG per real step. Guidance and horizon remain
explicit `--trick RANDOM-05`. Continuation averaging is generic.

At K1024/local0, five planner seeds 0–4:
- B4: 3379 / 3393 / 3383 / 3400 / 3408, mean 3392.6.
- B8: 3451 / 3447 / 3328 / 3422 / 3440, mean 3417.6.
- Old configuration: 3356 / 3311 / 3325 / 3395 / 3305, mean 3338.4.
Both improve every pair, averaging +1.6% and +2.4%. Same development input;
these are not independent task/start instances.

Previous cutoff-free best remains 3,197 on both allocations (+9.7% versus NMS4,
+0.8% versus NMS32), source b824f5d. New continuation cutoff-free runs unmeasured.

## Current batches

Check qstat and completion files before any restart. GRID jobs disappearing from
qstat may need 30–60s for NFS metadata to appear. Never restart implicitly.

- continuations-split-full-v42: 10/10 valid, archived. Default control exactly
  reproduces the old 3,395 trajectory. B4/K2048 = 3,492 versus single/K2048 = 3,240.
- continuations-validation-split-full-v42: 8/8 valid, archived, including exact
  four-core reproduction at 3,400. Local allowances 4/16/64 = 3419/3366/3370.
- continuations8-validation-split-full-v42: 8/8 valid, archived, including exact
  four-core reproduction at 3,422. B8/K2048 = 3458; B16/K1024 = 3384; local32 = 3448.
- operation-reference-rules-split-full-v43: 5/5 valid, archived. Control 1973;
  success protection 2099; terminal-turn exclusion 1742; both 1820; both with
  moving-only repair 2008. Not competitive; leave the operation policy off.
- continuations-scale-split-full-v42: jobs 8899823–8899831. Seven runs collected:
  K2048/B4/start2 = 3427; K2048/B8/start2 = 3501; K2048/B16/start1 = 3485;
  K4096/B4/start1 = 3477; K4096/B4/start2 = 3555; K4096/B8/start1 = 3442;
  K1024/B8/start2 = 3410. Remaining: four-core 3492 reproduction and
  K4096/B8/start2. Collect them before promoting or restarting anything.
- shared-prefix-split-full-v44: jobs 8899838–8899841. The 3492 and 3450 full
  controls reproduce every action, assignment and event. K4096/B8/start2 on four
  cores fails at step 0 (1133 ms, exit 124). Four-core K2048/B4 remains running.
  The queued 32-worker jobs were expanded to verified same-model hosts via
  qalter; host-expansion.json preserves the original request and adjustment.
- continuation-refinement-split-full-v44: jobs 8899846–8899859. Fourteen full
  cases: optimized 3501 control and four-core confirmation, planner seeds 0/1/2/4,
  start3/4, future mutation 0.1/0.6/1.0, B4/start2/mutation0.6, depth6/10.
  Source da00823, shared-prefix enabled. No results yet at this checkpoint.

## Source versions

- v31 b824f5d: previous 3395, and frozen fresh-input validation candidate.
- v42 a6ad284: average multiple future priority sequences for each root decision.
  K is total rollouts, B continuations per root; B divides K. Shared continuation
  draws across roots; future mutation 0.3, starting after 1 step by default.
  Default B=1 preserves the complete old trajectory. Dense tests pass.
- v43 131d6be: optional reference operation success protection and no terminal
  rotations, based on pinned public EPIBT implementation. Tests pass, no frontier.
- v44 da00823: optional R05_SHARE_PREFIX=1 reuses exact common rollout prefix,
  including virtual task turnover, completion/discount score state and reverse
  turns. Dense tests and full control equivalence pass. Four-core timing pending.

## Fresh-input validation and interpretation

Source b824f5d and its settings were frozen at protocol commit 22e7cd1 before
inputs 50001/50002 were generated. Candidate 3386 vs NMS 2920/2957 (+14.5%);
3178 vs 2898/2915 (+9.0%). Aggregate +11.8%, against stronger NMS repeat perinput.
All six valid on four physical EPYC 9354 cores. Audit checks source/binary/input
hashes, allocations, deadlines and RAM. See FRESH_VALIDATION.md and
results/fresh-validation-v1/audit.json. Do not tune on these inputs and still
call them held out. A new solver validation must predeclare new inputs.

The colleague's 3531/3715 counts are from private inputs. They are reference
orientation, not directly comparable scores. Their roughly 27–28% matched gain
remains the objective; current four-core gain is 17.4%, larger candidates pending.

## Diagnosis and previous negative work

Action replay in results/action-audit-v39 independently verifies moves, task
locks and 1.6M robot-steps. The old solver has +25.77 loaded waits per common
completed task versus NMS4 and +35.26 versus NMS32, despite fewer turns. These
are observations, not a recoverable-throughput estimate.

Full comparisons already rejected: early forward augmentation; field seeds
1–32 except selected 15; confidence-weighted field penalties; age caps/rollout
aging/push costs; longer depth or other noise under the old single-continuation
regime; completion bonuses; visible-pool virtual task forecasting; extra local
search; regional mutations; opposite-turn penalties; static loads; score blends;
startup short-task preferences; multigeneration search. See progress log and
RESEARCH.md for evidence. Do not blindly repeat them. New continuation search
changes the compute-scaling regime, so old K-scaling losses are not universal.

Short prefixes fail to preserve full-run rankings reliably. Use all 2,000 steps.

## Immediate next work

1. Collect pending scale/prefix/refinement runs; record valid new bests with
   actual source, UTC, seed, allocation and explicit tricks.
2. Confirm optimized four-core timing and trajectories before claiming a speedup.
3. Investigate exact performance optimizations if larger portfolios help on
   32 workers but exceed 1s on four cores. Preserve all scoring/tie decisions.
4. Once a candidate is selected, run fresh predeclared matched input validation.
5. Audit progress rows, commit/push only our scope, keep this checkpoint current.

## Shared workspace and authorization

A separate Warehouse session edits cgar/ and experiments/construction-20260918/.
Only edit/stage/commit random05/ and RANDOM05_PROGRESS.md. Use git commit --only
with those paths; never include the other session's staged changes. Public
fywu85/lorr push is authorized. Do not change repository visibility.
No subagent delegation is authorized.

GRID physical-core binding, exact CPU-model guard and quota checks remain
required. Shared hosts are allowed. Verified EPYC 9354 hosts include research32,
33,34,35,36,37,39,40,41,42,45,46,47,48,49,50,51,53,54,55,56,58. New jobs may use
that wider list instead of only 33/39, with unchanged per-run core limits.

Optional Fable source-bundle approval is pending after auto-review rejection.
Do not transmit or repackage the blocked payload. Local work remains unblocked.
Reserved uninitialized session: 27a4316e-b79d-46cf-86b4-41b0f558938a.
Payload runs/random05/fable-review-01/payload.txt, SHA256
 ded0889fcaa2cb82a3f4fd9d5dc46f737bdf50cf7a9e466bb66f9114f9661d0f.
