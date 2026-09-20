# RANDOM-05 continuation

Updated: 2026-09-20 19:41 UTC.

The goal remains ACTIVE. The user wants continued throughput improvement until
our evening check-in; waiting time is secondary. This turn made clear progress:
strict four-core3709 (+27.3% vs NMS) and overall3794 (+19.6%), plus a new general
search method, full controls, diagnostics and audits. This is not blocked.
Only edit/stage/commit `random05/` and `RANDOM05_PROGRESS.md`. Warehouse work
shares main; leave other files, staged changes and GRID jobs alone. No subagent
delegation. Public fywu85/lorr; pushing is authorized. Do not change visibility.

## Verified records

- Four physical cores: **3709**, source **f81b760**, build-v61.
  First K5120 then K6144/B8, start2/local0, generations4/E8/P8, seed3,
  future elite blend0.5, cache512, kinematic and cycle masks enabled.
  Mean819.611ms, max888.504ms, RSS451716KiB (463MB), user6530s/wall1641s.
  **+27.3% vs strongest matched NMS4=2914**. Finished18:59:34.727026 UTC.
  Evidence: results/cycle-mask-strict-candidate-split-full-v61/
  four-first5120-k6144-blend0.5-cycle-mask1-strict/summary.json.
  The whole trace equals the earlier relaxed5s source60c5f9b3709 result.
  That older run is separate; its mean923/max1495ms was never a strict frontier.
  This reaches the colleague's reported gain range on the development input;
  it is not yet fresh-input validation of this selected configuration.
- Overall /32 workers (16 physical cores, SMT2): **3794**, source **5f81613**,
  build-v65. First8000 thenK16320/B14, screen2/keep4, generations4/E8/P8,
  seed3, blend0, cache64, kinematic mask1, cycle mask0. Mean473.240ms,
  max525.342ms, RSS564328KiB (578MB). **+19.6% vs matched NMS32=3172**.
  Finished19:23:22.219359 UTC. Evidence:
  results/staged-initial-budget-32-split-full-v65/
  32-screen2-keep4-k16320-b14-seed3-blend0-first8000/summary.json.
- Intermediate records this turn are retained:3755 atK8192/B10/seed3,
  mean257/max353ms;3776 atK16384/B10/seed3, mean567/max768ms, finished
  19:21:39.535205 UTC. All source5f81613. Best manifests select3709/3794.
  Progress audit PASSED69 frontier rows. Waiting history/manifest/report updated.

All canonical frontiers: 2000 steps, strict1s,30s preprocessing,32 decimal GB
address-space cap, zero errors/timeouts, EPYC9354, verified GRID affinity.
Shared hosts allowed. Fixed work; timeout exits124, no partial search returned.
Map-specific generated guidance and known-horizon triage require --trick
RANDOM-05. Search/cache mechanisms are general; best cases still use the tricks.
NMS4 strongest2914 (other repeats2902/2903), NMS32=3172. Both local builds retain
the documented constructor fix; four-worker variant only changes worker count.

Common policy: generated flow field15, average/normalize, power3,
betweenness.75, penalty2.4, one flip/flipseed5; turn.6/wait.5;
depth8/noise200/dispersion.8; guided Hungarian1000, keep.5/length.25;
horizon2000/triage1.5; per-step RNG. Exact flags include shareprefix, packedorder,
fastdispersion, scratchreuse, goalcache, radixorder, candidatecache.
Use measured best manifests, not defaults. Mutation/future mutation.3, decay1,
risk0. No tuning on independent inputs50001–50006.

## New method and validation

Current engine source5f81613/build-v65, SHA
5c012a610f8ccae75a515ae281f3099822f76a629725af3ebc30b34b7e1bfcf3.
R05_SCREEN_BRANCHES=s (default0) and R05_SCREEN_KEEP=q (default4): screen every
root with s complete futures; retain exactly1/q, including first anchor within
quota; finish all B futures for survivors. At least2 survivors per generation
ensure both anchor and leader survive. Only fully evaluated candidates compete,
become elite parents or persist. K is exact branch-evaluation work: each q-root
group costs q*s+B-s, and every generation must have complete groups. Prefix and
score sums resume in branch order. Runtime count assertion checks global work.
No clock-based selection. Local trials retain their separate declared budget.

Example: K5120/B14/s2/q4/g4 gives1024 roots and256 full finalists. K8192/B10
uses2048 roots/512 full finalists. This changes search, unlike cache flags.
Tests compare exhaustive/staged identical-future searches with equal root draws,
shared prefix on/off, smaller first-step budget; stochastic risk/turnover checks
verify worker determinism and collision/task locks. Complete suite PASSES13.73s.
Failed build-v64/source21f0cb8 is preserved: tiny fixture allowed only one
survivor and forcing the anchor removed the leader. v65 rejects that contract.

Branch diagnostics: source d4a6047/build-v62 added observational per-branch
scores; source678f905/build-v63 buffers each generation into one log write.
An unbuffered3709 diagnostic failed step0=1035.4ms; corrected build completes
3709 on32workers, mean160.9/max234.7ms. Original failure remains archived.
Three complete diagnostic traces EXACTLY reproduce3743/3741/3709 counterparts.
At20 sampled steps x4 generations each, two-future/top-quarter screening keeps
all240 batch winners and99.1%/97.2%/97.0% of top-eight candidates.
Evidence: results/branch-ranking-20260920/{audit.json,REPORT.md,equivalence.json}.
This supports testing, not a throughput guarantee or independent-input claim.

Unchanged source65 control reproduces every field of3743, mean220/max326ms.
Initial staged32 results (all full strict): K5120/B14/blend0/.5=3670/3574;
K5120/B10/blend0=3635. K8192/B10/blend0 planner seeds0–4:
3730/3642/3611/3755/3715, mean3690.6, versus ordinaryK8192/B8
3743/3492/3730/3705/3513, mean3636.6 (+1.485%,3/5 positive pairs).
This is one development input, not independent task/start validation.
Paired evidence: results/staged-seeds-and-budget-32-split-full-v65/paired-seeds.json.

Direct four-coreK5120/B14 controls score3670/3574, means693/690ms,
maxima825/825ms, exactly matching fast32 traces. Unchanged control3655 also
matches its old trace, mean672/max810ms. No throughput promotion over3709.
Four-corefirst5120 thenK6000/B14 andK6144/B10 score3664/3654, mean818/848ms,
max891/912ms. Both EXACT fast32 counterparts; neither improves3709.
Evidence in staged-continuations-four and staged-larger-four-split-full-v65.

Larger32 work: fullK16384/B10/seed0 andK16320/B14/seed3 fail atstep0=1093.3/
1133.0ms, preserved. New startup budgets8192/8000 complete3667/3794.
FullK16384/B10/seed3 completes3776. Explicit startup counts can change the
trajectory; they are new configurations, not silent retries or clock fallbacks.

Fixed1024-root/256-finalist comparisons: s2/B12/K4608=3718;
s2/B14/K5120=3670; s2/B16/K5632=3695; s4/B16/K7168=3664. Allseed3.
First4608 thenB12/K5760 scores3770, versus3660 atK6120. Direct four-core
confirmations are pending; fast32 results do not establish four-core scores.

## GRID work to collect next

All source5f81613/build-v65. Full2000/strict1s. Inspect completion/summary,
not only qstat. Never assign throughput to a failed or partial run.

- staged-b16-four-split-full-v65, job8900496, nearing completion:
  s2/B16/K5632/roots1024/seed3. Expected3695 from fast32; verify whole trace.
- staged-b12-four-split-full-v65, job8900514, started about19:26 UTC:
  s2/B12/K4608/roots1024/seed3. Expected3718; promote only after full4-core
  deadline and trace checks. Cache512 versus64 on fast32 should be exact.
- staged-next-budget-32-split-full-v65, jobs8900515–17:
  first4608/K5760/B12 COMPLETE3770; K6120 COMPLETE3660.
  first8000/K24000/B14/seed3 STILL RUNNING, attempts more work than3794.
- worker-affinity-four-full-v65, job8900535, started about19:35 UTC:
  TWO cases in ONE allocation, each4 physical cores, disjoint CPU masks.
  first4608/K5760/B12 (fast32=3770), envOMP_PROC_BIND false vs close,
  OMP_PLACES threads, OMP_DISPLAY_ENV/AFFINITY true in both. This is a direct
  four-core confirmation plus a worker-placement comparison. Whole traces
  must match fast32 and each other. Job reserves8 cores total, each solver4.
- worker-affinity-32-full-v65, job8900536, about19:35 UTC:
  TWO cases, each16 physical cores/32 workers, disjoint masks, same host.
  Repeats3794 with the two binding policies above. Job reserves32 cores total.
  Do not use split_grid collect on these TWO unsplit affinity batches; native
  summaries are directly BATCH/summary.json and raw traces BATCH/CASE/result.json.

Binding probe: results/worker-affinity-probe-20260920/{probe.cpp,audit.json}.
Uses same libgomp as solver; confirms close binds four workers to four distinct
single-CPU masks within taskset allocation. Running paired-job affinity logs
also checked: every observed mask inside its case's allocated CPUs; close has
one unique singleton per worker. Snapshots in each result batch's early-affinity-
audit.json. Recheck full logs after completion. Early timing is mixed; no
speedup claimed. OMP_DISPLAY_AFFINITY emits again for fresh teams each step,
so both controls retain the same diagnostic overhead. GNU docs linked in RESEARCH.

All earlier staged/diagnostic/cycle batches complete and archived. Cycle32mask
controls exactly3743, different-host means305/279ms, no isolated speedup claim.
Four mask0 control3655, mean670/max793ms; mask1 control failedstep0=1278ms.
The standalone3709 configuration did complete strictly. Preserve all failures.
Runner preflight failures now produce terminal invalid summaries without a
solver launch; three-condition regression test passes. Source64 failed tests
were corrected in65 before any throughput run.

Possible next steps after the pending results: freeze a new independent-input
validation for the best confirmed four-core candidate; consider planner seeds
for3794 (not yet checked). Earlier start3/4 continuations lost52/112 tasks at
K2048/generation1; do not blindly repeat that setting. A changed method might
justify a specific compute/quality comparison, but none is queued. CPU setup
still consumes significant sampled time; exact loop fusion or carefully keyed
cross-step ranking caches are unimplemented ideas, not measured improvements.

## Fresh validation and secondary metrics

Fresh V3 remains frozen at protocol81bdfbd, source6ce9312/build-v52,
K5120/B8/g4/E1/P1/seed3 (development3637). Input50005:3698 vsNMS2926/2900;
50006:3619 vs2906/2831. Aggregate+25.46% vs stronger repeats, all6 valid.
No later best replaces that candidate. Inputs50001–50006 stay out of tuning.
A future V4 needs a new frozen candidate/protocol and new inputs (e.g.50007/8),
after these pending improvements settle. Do not claim the selected3709 or3794
has fresh validation yet. Colleague absolute counts use private inputs.

Current max completed latency: four3709=1947 steps vsNMS1997;32-worker3794=1950
vsNMS1976. Initial unfinished135/133 vsNMS219/206; unopened98/98 vs102/91.
All have initial orders unfinished at2000, so eventual max unknown, at least2000.
Completed-only means are censored. Throughput stays the selection criterion.
Matched report: results/task-waiting-frontiers-20260920T1612/REPORT.md.

Fable's exact approved79KB payload was attempted15:01UTC with session
27a4316e-b79d-46cf-86b4-41b0f558938a; provider429/model_requires_usage_credits,
zero usage, NO REVIEW. User informed. Old payload is source7506a, not this
new staged search. Do not claim feedback or silently replace authorized bytes.
Optional exact-payload retry if credits return; not a blocker to local work.

## Routine operations

Collect with split_grid.py collect. Archive only direct *.json and */*.json
metadata (summary, completion, spec, allocation, submission, batch). Never
recursively copy NMS cwd/raw outputs. Preserve every invalid attempt.
Full equivalence fields: actualPaths, plannerPaths, actualSchedule,
plannerSchedule, events, tasks; record both raw hashes.
Promote strict records only after source/input/allocation/trace checks; update
best*.json, timestamped progress row, matched waiting manifest/report and audit.
`audit_progress.py` regenerates WAITING_PROGRESS.md. Scope every commit:
`git add -- random05 RANDOM05_PROGRESS.md`
`git commit --only --quiet -m '...' -- random05 RANDOM05_PROGRESS.md`
Push with GIT_TERMINAL_PROMPT=0 GIT_ASKPASS= and gh credential helper.
