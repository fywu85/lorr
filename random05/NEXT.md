# RANDOM-05 continuation

Updated: 2026-09-20 19:14 UTC.

The goal remains ACTIVE. The user wants continued throughput improvement until
our evening check-in; waiting time is secondary. This turn made clear progress:
strict four-core3709 (+27.3% vs NMS) and overall3755 (+18.4%), plus a new general
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
- Overall /32 workers (16 physical cores, SMT2): **3755**, source **5f81613**,
  build-v65. K8192/B10, screen2/keep4, generations4/E8/P8, seed3, blend0,
  cache64, kinematic mask1, cycle mask0. Mean257.275ms, max353.212ms,
  RSS517704KiB (530MB). **+18.4% vs matched NMS32=3172**.
  Finished18:59:20.474586 UTC. Evidence:
  results/staged-continuations-32-split-full-v65/
  32-screen2-keep4-k8192-b10-seed3-blend0/summary.json.
- Previous records remain logged: four3655, overall3743. Current best*.json
  selects3709/3755. Progress audit PASSED67 rows against source/full traces.
  WAITING_PROGRESS.md and the matched waiting report include both new records.

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
K5120/B10/blend0=3635; K8192/B10/seed0/blend0/.5=3730/3572;
K8192/B10/seed3/blend0=3755. Compared with ordinaryK8192/B8, seed0 loses13,
seed3 gains50. Seed2 now3611 vs previous3730 (-119). Finish seeds1/4 before
reporting a five-seed mean. Original baseline seeds0–4:
3743/3492/3730/3705/3513, mean3636.6.

## GRID work to collect next

Source65 throughout. Inspect completion/summary, not only qstat. Full raw traces
are runs/random05/BATCH/CASE/CASE/result.json; compact archives mirror BATCH/CASE.

- staged-continuations-four-split-full-v65, jobs8900464–66, almost complete:
  control3655; screen2/keep4/K5120/B14/seed3 blend0/.5 should reproduce3670/3574.
  Verify all six trace fields against fast32 counterparts. Both initial steps
  were about827ms; later samples about700ms. Strict1s.
- staged-seeds-and-budget-32-split-full-v65, jobs8900475–82:
  seeds1/2/4 atK8192/B10 (seed2 COMPLETE3611); K16384/B10 seeds0/3;
  K16320/B14 seed3; first5120 thenK6000/B14 orK6144/B10.
  K6000/B14 COMPLETE3664 (mean192/max255ms), below3670 atK5120/B14.
  Seed0 K16384 and seed3 K16320 FAILED atstep0=1093.3/1133.0ms; preserve.
  K16384/B10/seed3 continues, later lookahead sample about508ms.
- staged-larger-four-split-full-v65, jobs8900483–84:
  first5120 thenK6000/B14 orK6144/B10. Direct four-core strict confirmations.
- staged-initial-budget-32-split-full-v65, jobs8900486–87:
  NEW explicit startup budgets for failed larger searches: K16384/B10/seed0
  first8192, andK16320/B14/seed3 first8000. Different declared algorithms; not
  silent retries. Regular budgets unchanged; preserve earlier failures.
- staged-fixed-roots-32-split-full-v65, jobs8900493–95, just launched:
  keep1024 roots/256 finalists: s2/B12/K4608, s2/B16/K5632, s4/B16/K7168.
  All seed3, g4/E8/P8/blend0. Existing s2/B14/K5120 is3670. Ordinary fullB16
  with same1024 roots previously3741 atK16384; pruning can alter later parents,
  so exact trajectory equivalence is not assumed.
- staged-b16-four-split-full-v65, job8900496: direct strict four-core
  s2/B16/K5632/roots1024/seed3. First-step budget unchanged; nominal first-step
  room suggested by the827ms K5120/B14 profile. Let actual result decide.

Completed cycle controls: sourcef81b76032mask0/1 both EXACT3743,
means305/279ms on different hosts. Cycle phase samples do not establish a gain.
Four mask0 control EXACT3655, mean669.6/max792.9ms; mask1 control failedstep0
1277.9ms. No default-on cycle-mask claim. Standalone3709 configuration did pass.

Runner now produces terminal invalid summaries for CPU model/binding/quota
preflight rejection, without launching a solver. Focused tests all3 cases pass.
This avoids confusing missing summaries with jobs still running.

## Fresh validation and secondary metrics

Fresh V3 remains frozen at protocol81bdfbd, source6ce9312/build-v52,
K5120/B8/g4/E1/P1/seed3 (development3637). Input50005:3698 vsNMS2926/2900;
50006:3619 vs2906/2831. Aggregate+25.46% vs stronger repeats, all6 valid.
No later best replaces that candidate. Inputs50001–50006 stay out of tuning.
A future V4 needs a new frozen candidate/protocol and new inputs (e.g.50007/8),
after these pending improvements settle. Do not claim the selected3709 or3755
has fresh validation yet. Colleague absolute counts use private inputs.

Current max completed latency: four3709=1947 steps vsNMS1997;32-worker3755=1919
vsNMS1976. Initial unfinished135/130 vsNMS219/206; unopened98/93 vs102/91.
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
