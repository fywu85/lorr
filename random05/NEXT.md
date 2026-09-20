# RANDOM-05 continuation

Active goal: approach the colleague's roughly27–28% advantage over matched NMS
in the combined track. Archived RANDOM-05,800 robots,2000 steps. Private code
and synthetic instances are unavailable. The goal is not complete.

## Verified frontier

- Best3,395 on both four physical cores and32 workers/16 physical cores.
  Source `b824f5d` / build-v31. One directional pair reversed in generated
  field15, flip seed5, planner seed3. Explicit `--trick RANDOM-05`.
- Versus strongest matched NMS4=2,914: +16.5%. Mean312ms/max425ms; RSS294MB.
- Versus matched NMS32=3,172: +7.0%. Mean79ms/max181ms; RSS443MB.
- All actions, assignments and task events match between allocations. Full2000,
  strict1s, no errors/timeouts. All recent comparisons use EPYC9354 CPUs.
- Local mutations at counts1/4/16 and eight flip seeds each produce maxima
  3,395/3,365/3,306 versus control3,379. This is a selected maximum.
- New field across planner seeds0–4:3,356/3,311/3,325/3,395/3,305, mean3,338.4.
  Previous field:3,374/3,231/3,365/3,379/3,350, mean3,339.8. Two pairs improve;
  no average gain. These are planner seeds on one input, not fresh instances.
- Best JSONs freeze source/configuration/evidence. Log audit verifies36 rows.
- Config: flow seed15, one flip seed5, output contrast2.4, K1024/depth8/noise200,
  dispersion0.8/local5/equal, wait0.5/turn0.6, exact guided matching,
  keep0.5/length0.25, horizon2000/triage1.5, independent per-step RNG.
- Without the horizon cutoff:3,197 on both allocations, exact actions/schedules/
  events; +9.7% versusNMS4, +0.8% versusNMS32. Four-core mean316ms/max421ms,
  RSS293MB. Guidance remains an explicit map trick. Cutoff contribution198 tasks
  (+6.2%). New best-no-horizon JSONs preserve both configurations.

## Completed batches and next work

As of 2026-09-20 13:22 UTC, all Random05 jobs through flow-confidence v41 are
complete, collected and archived. No Random05 benchmark is intentionally pending.
Check qstat before any restart; unrelated Warehouse jobs must remain untouched.
Compact summary/spec/allocation metadata is under results/; raw traces are under
runs/. Do not restart completed batches.

- Fresh validation:3386 vs2920/2957 (+14.5%);3178 vs2898/2915 (+9.0%). Aggregate
  +11.8%. All six valid; audit checks frozen protocol22e7cd1, sourceb824f5d,
  binaries, generated inputs, four physical EPYC9354 cores, latency and RAM.
  See results/fresh-validation-v1/audit.json. No tuning on these inputs.
- Fields17–32: all16 valid and below field15; best3283 at17. Earlier subsets
  had been screened at smaller K or different settings. Current comparison uses
  K1024/planner3/contrast2.4, no local flips.
- Policy recheck: rollout aging3333; age cap100=3337, cap200=3352;
  push price1=3117, price2=3121; prospective idle-turn costing3331. All lose.
- Confidence exponents0/.25/.5/1/2:3395/3295/3200/3153/2899. All valid; default
  actions/schedules/events exactly equal v31. Keep the feature off.

Next useful structural hypothesis: compare each first pipeline decision under
several different future priority sequences, using a mean score, at fixed total
rollout work. Current search evaluates one constant-offset continuation per root;
more K has not reliably helped. Keep the root's actions and pending moves fixed
across branches, preserve default behavior at one continuation and deterministic
worker counts. This is not implemented or a claimed gain. RESEARCH.md records it.

## Structural experiment

The optional three-step policy is implemented in src/operations.cpp. It adapts
reservation/revisit structure from the public MIT EPIBT implementation; notice
retained in licenses/EPIBT-MIT.txt. Public source checkout:
/tmp/r05-epibt-reference at3808e832593c9fb46b3e119cbb36035cccd830e1.
RESEARCH.md links the paper and records the task/horizon comparison limits.

v34 full scores: K1=152,K8=1018,K32=1019,K128=1246. AtK32, travel-cost1=823,
no inheritance=624,revisits16=1886. Pipeline control exactly repeats3379.
These are valid but weak. The pipeline remains the default.

v35 R05_OPERATION_MOVING=1 excludes stationary footprints from active repair
candidates, retaining the whole inherited plan as fallback. This addresses one
concrete difference from the reference, which excludes all-wait from its active
choices. Dense regression passes. Full K1/8/32/128=343/817/1118/711; cost1=743 and no
inheritance=577. All are valid but weak; default optionoff.

Larger revisit budgets also lose: revisit64/K32=1973, revisit128/K32=1929,
moving-only/revisit64/K32=1890, revisit64/K128=2106. All valid.

Other potential operation differences: the reference protects successful paths
for the rest of the search pass, while our recursion clears the stack on success;
ours also permits terminal rotations. Do not claim either is the bottleneck
without controlled tests. Current strict priority comparison lacks an explicit
agent-ID tie order. Fixed work must finish; no deadline-triggered partial return.

## Findings and failures

- More K is weak: K2048=3343 versus K1024=3374; depth6 loses; K3072 times out.
- Contrast1.6→2.4 improves all5 planner-seed pairs: mean3208.6→3324.8 (+3.6%).
  Independent per-step RNG then raises mean to3339.8 (+0.45%,4 positive pairs).
- Opposite-turn penalties, local25/100, reduced mutation, regional mutation,
  static load, unweighted score blends, multigeneration search and stronger
  startup/global task-length preferences all lost full-budget comparisons.
- Short prefixes are unreliable: at500 steps, topthird retained only7/10 full
  scores within2% ofbest among26 selected competitive trajectories. Use fullruns.
- Active-cost-row optimization preserves every control action/schedule/event;
  observed four-core mean326→310ms, one pair on shared EPYC9354 hosts.
- First500: ours3379 trajectory visits3393 stops versus NMS4's3346, but completes
  779 tasks versus879. Startup length preference did not fix total throughput.
  The latest cutoff-free control now scores3197; known-horizon triage adds198
  tasks on the frontier seed. Do not present3395 as horizon-independent.

## Waiting-time diagnosis

Independent replay validates all moves, task locks, events and 1.6M robot steps
in four trajectories. On common completed task IDs, ours has +25.77 loaded waits
per task against NMS4 and +35.26 against NMS32. Turns are fewer. Work on tasks
unfinished by2000 is13.41% for ours3395 versus31.03%/30.04% for NMS4/32.
These are observations, not recoverable-throughput estimates; assignments and
congestion histories differ. See results/action-audit-v39/REPORT.md and report.json.

Source56b0970/v40 tests R05_EARLY_FILL=1: safe aligned chains/cycles may augment
currently promised moves. Positive gain is required relative to idle quarter-turns;
previous moves remain fixed and swaps are rejected. Tests pass, full comparisons
lose: thresholds0/1/4=3369/3355/3381; K256=3332; no-cutoff3101. Control3395 is exactly
equal to v31 in actions/schedules/events. Keep off. No new frontier.

## Latest rejected hypotheses

- Depth10/12/16=3302/3284/3199 versus depth8=3395. Noise100/400/800=3349/3335/3348 versus noise200=3395. All valid.
- Full K1024 field seeds1–16: field15 remains best3379 without its local flip;
  other fields range3012–3270. The field choice survives full-budget validation.
- Cutoff scales0.75/1/1.25/1.5/1.75/2/2.5/3:
  3372/3380/3367/3395/3367/3335/3236/3117. No cutoff3197. Keep1.5.
- R05_COMPLETE_BONUS0/2/4/8/16/32:3395/3281/3327/3289/3253/3054. All lose.
- R05_ROLLOUT_MATCH1 predicts task turnover using only the visible free pool.
  K256/512/1024=3254/3225/3335; bonus8 atK1024=3379; no-cutoff3122 versus3197.
  All lose. The default control remains3395 with exactly identical actions,
  schedules and events acrossv31/v36/v37. Keep both new options off.
- The virtual forecast is approximate: fixed existing assignments, no new hidden
  task reveal, no reapplication of horizon triage to new virtual assignments.
  It also loses without cutoff, so the horizon mismatch alone is not established
  as the explanation. Do not assume a more detailed forecast helps throughput.

Fresh validation is held out from configuration selection: sourceb824f5d and
its settings were frozen before generating inputs. Do not tune on50001/50002
and continue calling them held out. Report every candidate result and both NMS
repetitions, using the stronger valid NMS run per input. No matching colleague
private instances or exact regeneration protocol is available.

## New guidance hypothesis

R05_FLOW_CONFIDENCE_POWER (default0) scales opposing-edge penalties by normalized
forward/reverse traffic imbalance to the specified exponent. Weak directional
evidence gets a smaller penalty. Preferred edges and direction ordering stay
fixed before global normalization. Still requires explicit --trick RANDOM-05.
v41/sourceffe75c7 tests pass. Full exponents0/.25/.5/1/2 are complete and lose
except the unchanged0 control:3395/3295/3200/3153/2899. Default actions/schedules/
events exactly match v31; equivalence JSON is archived under results/. Keep0.

## Source versions and tests

- v22 `3228b9c`: direct recursive PIBT, previous four-core3379.
- v28 `32b333a`: active cost row optimization, previous32-worker3379.
- v30 `33f358e`: optional startup task-length trick, all variantslose.
- v31 `b824f5d`: seeded directional-pair flips, producescurrent3395.
- v32 `32082ff`: optional three-step policy, first regressionspass.
- v33 `4df6f33`: prunes redundant rotations; added one-visit mobility ablation
  fails (not collision/suffix/cycle checks). Preserve the failed build record.
- v34 `b72a51e`: rejects one-visit performancecandidate; four-visit/inheritance
  and no-inheritance mobility tests pass. Same enginebinary asv33.
- v35 `0754ed8`: optional moving-footprint repair; regressions pass.
- v36 `369e2df`: optional whole-task completion reward; contested-goal test passes.
- v37 `5db6827`: optional visible free-task turnover inside rollouts; no-spare
  trajectory identity and dense spare-task worker determinism pass. Full control
  equality is verified. Every full run is valid, but none improves throughput.

- v40 `56b0970`: early forward augmentation; ready-cycle/swap/dense turnover
  and worker determinism tests pass. Full variants lose, option remains off.

Frozen build source hashes and tests are under runs/random05/build-v*/.
Before promotion/commit run `python3 random05/tools/audit_progress.py`.

Next optional structural test: successful-path protection and exclusion of
terminal rotations in the weak three-step operation kernel. The pinned public
EPIBT source has both; ours lacks both. Test separately if revisiting the kernel,
without inferring performance from this observation. RESEARCH.md has the details.

## Resources, authorization, shared workspace

GRID physical-core binding is required; exclusive hosts are not. Matched hosts:
`--hosts 'research33*|research39*' --cpu-model 'AMD EPYC 9354 32-Core Processor'`.
Runner reserves both SMT siblings, checks binding/model/quota, enforces32GB.
32-worker fullK1024 takes roughly3min; four-core confirmations roughly10min.

A separate Warehouse session edits cgar/ and experiments/construction-20260918/.
Only stage/commit random05/ and RANDOM05_PROGRESS.md. Use git commit --only with
those paths to avoid the shared index. Public fywu85/lorr push is authorized;
do not change visibility. No subagent delegation is authorized in this turn.

Optional Fable exact-source-bundle approval is still pending after auto-review
rejection; do not transmit or repackage the blocked payload without approval.
Local work remains unblocked. Details in runs/random05/NEXT_session.md.
