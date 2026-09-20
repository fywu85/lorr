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
- Best JSONs freeze source/configuration/evidence. Log audit verifies34 rows.
- Config: flow seed15, one flip seed5, output contrast2.4, K1024/depth8/noise200,
  dispersion0.8/local5/equal, wait0.5/turn0.6, exact guided matching,
  keep0.5/length0.25, horizon2000/triage1.5, independent per-step RNG.
- Previous no-horizon best2,914 has not yet been rerun with latest changes.

## Pending batches

Collect using `tools/split_grid.py collect --output runs/random05/<batch>`.
Check qstat for authoritative running/queued state. Child summary/spec/allocation
provide complete provenance; copy compact JSON metadata into results/, excluding
raw traces and binaries. Frontier links should point to the child summary list.

| Jobs | Batch | Purpose |
|---|---|---|
| 8899654–8899669 | field-revalidation-split-full-v31 | Full K1024/planner seed3, flow seeds1–16, unmutated fields. Original field selection used lowK; production-budget rankings can differ. Source b824f5d. |
| 8899670–8899675 | operation-moving-split-full-v35 | Optional moving-footprint search under the three-step operation policy; K1/8/32/128 plus cost/no-inheritance ablations. Source0754ed8. |

Complete and collected: guidance-local-split-full-v31 (25/25),
guidance-local-validation-split-full-v31 (5/5), operation-policy-split-full-v34
(8/8). All earlier batches throughv30 are also terminal; do not restart them.

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
choices. Dense regression passes. Full K1/8/32=343/817/1118; cost1=743 and no
inheritance=577. K128 remains pending. This does not close the gap. Default optionoff.

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
  Late known-horizon triage explains much of the final advantage. Best non-horizon
  result remainsolder2914; do not present the frontier as horizon-independent.

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

Frozen build source hashes and tests are under runs/random05/build-v*/.
Before promotion/commit run `python3 random05/tools/audit_progress.py`.

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
