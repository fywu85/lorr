# RANDOM-05 continuation

Updated: 2026-09-20 20:17 UTC. Goal ACTIVE. The user wants continued throughput
improvement and an evening check-in; waiting time is secondary. There is clear
progress, not a blocker. Finish the pending frozen validation and full controls.
Only edit/stage/commit `random05/` and `RANDOM05_PROGRESS.md`; Warehouse work
shares main. Leave other files, staged changes and GRID jobs alone. No subagent
delegation. Public fywu85/lorr; pushing is authorized. Do not change visibility.

## Current verified frontiers

- Four physical cores: **3770**, source **5f81613/build-v65**, seed3, firstK4608
  thenK5760/B12, screen2/keep4, generations4/E8/P8, blend0, cache512,
  kinematicmask1/cyclemask0. **+29.4% vs strongest matched NMS4=2914**.
  Mean787.573ms, max845.398ms, RSS473328KiB (485MB), user6258s/wall1577s.
  Finished2026-09-20T20:02:29.833542+00:00, case
  four-k5760-b12-first4608-omp-bind-false, in
  `results/worker-affinity-four-full-v65/summary.json` (UNSPLIT batch).
  Both false/close binding cases score3770 and exactly match all six trajectory
  fields of the earlier fast32 run. Both use four physical cores; the job reserves
  eight total. All observed worker masks stay within disjoint case allocations.
  Binding means788/788ms, maxima845/843ms: no useful speedup claim.
- Overall /32 workers (16 physical cores, SMT2): **3852**, source **5f81613**,
  seed4, firstK8000 thenK16320/B14, screen2/keep4, generations4/E8/P8,
  blend0/cache64/kinematicmask1/cyclemask0. **+21.4% vs NMS32=3172**.
  Mean478.098ms, max538.443ms, RSS565044KiB (579MB), user28875s/wall959s.
  Finished2026-09-20T20:07:08.133068+00:00. Evidence:
  `results/staged-record-seeds-32-split-full-v65/32-staged-k16320-b14-first8000-seed4/summary.json`.
- Intermediate records remain:3718 four-core K4608/B12 (628ms mean/756ms max),
  earlier3709 four-core;3755/3776/3794 overall. Best manifests select3770/3852.
  Progress audit PASSED72 rows, including raw waiting accounting.
- Build-v65 SHA:
  5c012a610f8ccae75a515ae281f3099822f76a629725af3ebc30b34b7e1bfcf3.
  Canonical records are full2000/strict1s/30s preprocessing/32decimalGB guard,
  zero errors/timeouts, verified EPYC9354 allocation. Shared hosts are allowed.
  No deadline-driven partial result. Fixed work completes or exits124.

Exact config is in best manifests. Common tricks: generated flow field15,
1flip/flipseed5, contrast2.4; known horizon2000/triage1.5. All require --trick
RANDOM-05. Search mechanisms/caches are general; records still use these tricks.
Common depth8/noise200/dispersion.8, turn.6/wait.5, guided Hungarian1000,
keep.5/length.25, per-step RNG, continuationstart2/local0/futuremutation.3.
All exact prefix/order/cache optimizations enabled. Use manifests, not defaults.

## Frozen V4 validation RUNNING -- do not change candidate

Protocol commit **a7bad0c**, files FRESH_VALIDATION_V4.md and
experiments/fresh-validation-v4-{protocol,full}.json. Committed BEFORE generating
new inputs50007/50008. Source5f81613/build-v65, exact3770 four-core false-binding
configuration, including affinity diagnostics. No later source/seed substitution.
Six predeclared full runs: candidate once and NMS4 twice on each input.
`fresh-validation-v4-split-full-v65`, jobs8900630-35, started about20:05UTC.
All six allocations checked: four physical EPYC9354 cores, no CPU quota cap.
Expected candidates ~20:32, NMS ~20:37UTC. Preserve all attempts.
Collect, archive compact metadata, then run:

    python3 random05/tools/audit_fresh.py --batch runs/random05/fresh-validation-v4-split-full-v65 --output random05/results/fresh-validation-v4/audit.json --protocol-commit a7bad0c --protocol-json random05/experiments/fresh-validation-v4-protocol.json

Use stronger NMS repeat per input. Add separate per-input waiting audits if
useful; do not mix different task/start streams into the archived-input waiting
manifest (it correctly rejects that). Inputs50001--50008 remain OUT OF TUNING.
Fresh V3 stays frozen:3698 vs2926/2900 and3619 vs2906/2831, aggregate+25.46%,
all six valid. V3 uses different inputs: comparisons between percentages are
not paired causal effects. Colleague's private inputs/code remain unavailable.

## Other live jobs and next decisions

1. **setup-fusion-four-full-v66**, job8900602, UNSPLIT paired job, each four
   cores. Full K5760/B12/first4608 control build65 versus new build66.
   Started19:56:55UTC, expected~20:24. Both have policy profiling enabled.
2. **setup-fusion-32-full-v66** COMPLETE, both EXACT3794 across all six
   trajectory fields. Old mean505.759/max576.620ms; fused518.560/576.888ms
   (+2.53% mean). Audit: equivalence-and-timing.json. Reject fusion; engine.cpp
   has been restored byte-for-byte to source5f81613, and all compiled source
   hashes match build65. Four-core comparison still runs; archive it when done.
3. **staged-seed2-timeout-check-full-v65**, job8900641, UNSPLIT paired job.
   The original high-budget seed2 failed at step1850/1422.350ms (exit124), while
   other seeds completed. Explicit follow-ups use identical fixed work under
   strict1s and relaxed5s on disjoint same-host allocations. Started~20:11,
   expected~20:28. Preserve the original failure. Relaxed result cannot enter
   strict frontier. Determine whether spike repeats; don't assert contention
   without evidence. Compare full traces if both complete.
4. **staged-headroom-32-split-full-v65**, jobs8900643--46, started~20:14:
   B12/first4608, seed3, K6480/6768/7056; B14/first8000, seed4, K28800.
   Fast32 full strict runs. First three probe potentially feasible four-core
   work, but cannot be promoted as four-core without direct strict confirmation.

Current working C++ source is restored to **5f81613/build-v65** after rejecting
setup fusion. All compiled source hashes checked against build65. Failed
optimization sourcefde9172/build-v66 stays in history and immutable run copies;
its regression suite passed13.61s and full32 trace is exact, but mean time is
worse. `results/setup-fusion-v66/` retains build provenance. Do not discard the
pending four-core comparison or accidentally reintroduce the fusion.

## Completed comparisons this turn

- Fixed1024 roots: B12/K4608=3718;B14/K5120=3670;B16/K5632=3695;
  s4/B16/K7168=3664. B12/B16 directfour exactly match fast32 full traces.
- K5760/B12/first4608 planner seeds0--4:3663/3669/3732/3770/3654,
  mean3697.6 vs earlier ordinaryK5120/B8/E1/P1 mean3606 (+2.54%,5/5positive).
  Complete configurations and different work, not an isolated screening effect.
  `results/staged-record-seeds-32-split-full-v65/k5760-paired-seeds.json`.
- Higher-budget K16320/B14/first8000 seeds0--4:
  3785/3721/FAILED/3794/3852. Original seed2 failure prevents a five-seed strict
  mean; `k16320-original-seeds.json` retains all attempts. Do not omit the failure
  or silently replace it with the declared repeat.
- Near-K5760 retention comparison: s2/q2 gives3675, s2/q8=3509,
  s1/q4=3639, s1/q8=3737. All valid, below3770. Slightly different exact budgets
  ensure whole groups; see manifests. Retain s2/q4. Observational old-score
  diagnostic showed q8 loses some leaders; audit in branch-ranking-20260920.
- Larger K24000/B14/first8000/seed3=3702, below3794 atK16320. More work not monotonic.
- 32-worker binding false/close both EXACT3794; means503/500ms, maxima585/874ms.
  All observed affinity records inside allocation. No isolated speedup claimed.
- Earlier staged K8192/B10 seeds mean3690.6 vs ordinaryK8192/B8 mean3636.6
  (+1.485%,3/5 positive). All five strict. Separate from newer failed-seed batch.

Staged search source65: screen each root with s full futures, retain exactly1/q
(including anchor within quota), complete B for survivors; only fully evaluated
candidates can win/become parents/persist. K exact branch count, group cost
q*s+B-s, complete groups per generation, >=2 survivors. Tests13.73s pass.
Failed v64 tiny-anchor test was corrected before benchmarks; preserve history.

## Waiting and operations

Current completed max:3770four=1941,3852overall=1940; NMS4=1997,NMS32=1976.
Initial unfinished137/138 vs219/206. Every solver has initial orders unfinished
at2000, so eventual longest latency unknown and >=2000; completed stats censored.
`tools/render_waiting_report.py` now regenerates current REPORT.md from audited
best manifests. Run after audit_task_waits; it validates manifest/raw hashes and
rejects missing frontier rows. Executed successfully for3770/3852.

Collect split batches with split_grid.py collect. UNSPLIT jobs write their own
root summary; do not use split collector on them. Archive only direct *.json
and */*.json metadata named summary/completion/spec/allocation/submission/batch.
Never recursively copy NMS cwd or raw outputs. Full equivalence fields:
actualPaths,plannerPaths,actualSchedule,plannerSchedule,events,tasks; hash raws.
On promotion update best manifests, timestamped progress, waiting manifest/audit,
run audit_progress.py (regenerates WAITING_PROGRESS), render_waiting_report.py.
Scope all commits to random05/ and RANDOM05_PROGRESS.md; never touch Warehouse.
Push with GIT_TERMINAL_PROMPT=0 GIT_ASKPASS= and gh credential helper.

Fable exact approved79KB payload attempt at15:01UTC failed provider429 /
model_requires_usage_credits, zero usage, NO REVIEW. Session
27a4316e-b79d-46cf-86b4-41b0f558938a. Payload under
runs/random05/fable-review-01/payload.txt, source7506a, not current staged search.
Do not claim feedback or silently replace the authorized payload bytes.
