# RANDOM-05 continuation

Updated: 2026-09-20 16:31 UTC.

Goal ACTIVE and not yet complete: approach the colleague's roughly 27–28%
matched gain over NMS on four cores in the combined track. Their code/inputs
are unavailable; the pasted dev log guides this independent implementation.
Latest user steering: throughput remains PRIMARY; waiting time is a useful
secondary metric. Keep working; touch base this evening. No new fairness bound
or throughput-sacrificing requirement was imposed.

## Current verified records

- Archived-input overall /32 workers: **3,689**, source **8eb59d3**, build-v54,
  K8192/B8/start2/local0, generations4, elites8, planner seed3. Mean238ms,
  max359ms, RSS590MB. **+16.3% vs NMS32=3172**. Evidence:
  results/elite-scaling-32-split-full-v54/k8192-elites8-workers32/summary.json.
- Four physical cores: **3,648**, source **6ce9312**, build-v52,
  K5120/B8/start2/local0, generations4, elites1, planner seed2. Mean791ms,
  max927ms, RSS about309MB. **+25.2% vs strongest NMS4=2914** (other repeats
  2902/2903). Evidence: results/k5120-seeds-four-split-full-v52/
  k5120-generations4-seed2/summary.json. best-four-cores.json selects it.
- Previous four-core best3,637 uses the same config/source, planner seed3.
  Its exact configuration remains the frozen candidate for fresh validation V3.
- K5120/four-generation planner seeds0–4:3611/3526/3648/3637/3608, mean3606.0.
  K2048/four-generation:3562/3499/3514/3520/3562, mean3531.4. All five pairs
  improve, +2.1% mean. These are planner seeds on ONE development input.
- Previous overall3,657: sourced933023/build-v46,K16384/B8/gen1/seed3.
  Its raw result remains in continuation-larger-split-full-v46/k16384-b8/.
- Cutoff-free records: four3285 (+12.7%), sourcee896201/K2048/B8/gen1/seed3;
  32workers3408 (+7.4%), sourced933023/K8192/B8/gen1/seed3. They still use
  the guidance trick. Known-horizon controls3501/3596; effects+6.6%/+5.5%.

All frontier rows: full2000 steps, strict1s, no errors/timeouts, <=32decimalGB.
32 workers means16 physical EPYC9354 cores using SMT; four workers meansfour
physical cores. CPU bindings verified by GRID. Shared hosts allowed. Fixed work;
timeout exits124 rather than returning a partial search. Budget and allocation
must match NMS when quoting local gains. Published server numbers are indirect.

Common config: field15, one direction pair flipped (flipseed5), contrast2.4,
turn.6/wait.5, depth8/noise200/dispersion.8, guided Hungarian matching,
keep.5/length.25, horizon2000/triage1.5, per-step independent RNG, risk0.
Explicit --trick RANDOM-05 gates field/horizon/age-cap tricks. Search/caching
are general mechanisms; recorded bests still use these declared tricks.
All current optimized builds enable shareprefix, packedorder, fastdispersion,
scratchreuse, goalcache, radixorder, candidatecache. Code defaults stayoff.

## Waiting-time tracking (new user request)

Completed-order latency = final waypoint minus release time, in simulation
steps. Include oldest unfinished age and initial-cohort unfinished/unopened
counts; completed-only maxima hide the censored tail. Initial pool1200.
Old3637four vsNMS2914: completed maxima1947/1997; initial unfinished133/219;
initial unopened101/102. Old3657 vsNMS3172: maxima1954/1976, unfinished134/206,
unopened96/91. New3689: max1929, unfinished141, unopened96. Both solvers leave
some step-zero orders unfinished after2000, so eventual maximum wait is UNKNOWN.
New3648: maxcompleted1890, initialunfinished135, unopened98, startedunfinished37.

random05/tools/audit_task_waits.py checks events/releases/completion counts and
hashes raw results. audit_progress.py now checks waiting metrics on every
historical frontier and generates random05/WAITING_PROGRESS.md automatically.
It resolves early shortened archive names by frozen spec timestamps and exact
spec/summary equality, then checks raw trajectory hashes. Main audit remains
results/progress-audit.json. Matched latency report+manifest:
results/task-waiting-frontiers-20260920T1612/. Keep updating it for new bests.
Throughput remains primary; do not pick a lower-throughput solver for latency.

## Fresh validation

V1 complete: sourceb824f5d/protocol22e7cd1, inputs50001/50002:
3386 vsNMSmax2957 and3178 vs2915, aggregate+11.8%, all6valid.
V2 complete: sourcee896201/protocole6b2dbe, inputs50003/50004:
3494 vsNMSmax2984 and3387 vs2892, aggregate+17.1%, all6valid.
Different V1/V2 inputs: gain difference is not a paired causal estimate.

V3 COMPLETE, ALL6 AUDITED: frozen **81bdfbd**, source **6ce9312**, exact3637four config,
K5120/B8/gen4/elites1/risk0/plannerseed3; DO NOT substitute3648/3689/newer code.
New inputs50005/50006 generated after freeze; never use50001–50006 for tuning.
Candidate results3698/3619 are valid, max927/924ms. NMS repeats2926/2900
and2906/2831; stronger-repeat gains26.38%/24.54%, aggregate25.46%.
Audit passed; all inputs stay excluded from tuning. All six runs use four physical
EPYC9354 cores, full2000, strict1s/30spreprocess/32decimalGB AS guard.
Generator800unique starts,100000tasks, lengths2–5 INCLUSIVE, uniform free-cell
stops, reveal pool1.5*800. Generation metadata includes helper/NumPy/input hashes.
Protocol: FRESH_VALIDATION_V3.md, experiments/fresh-validation-v3-protocol.json.
After all6 outcomes exist, archive compact evidence and run:

    python3 random05/tools/audit_fresh.py --batch random05/results/fresh-validation-v3-split-full-v52 --output random05/results/fresh-validation-v3/audit.json --protocol-commit 81bdfbd --protocol-json random05/experiments/fresh-validation-v3-protocol.json

No silent restart, hidden failure, new candidate, changed input or relabeling.

## Recent implementation and experiments

v49 bd01f97 immutable minimum-heading goal cache; v51 9ad642e exact radix sort;
v52 6ce9312 bounded per-agent/worker candidate-ranking cache. Dense and full-run
controls preserve trajectories. Four K4096/B4 repeats3555 exactly; ranking cache
reduces mean824->669ms. Four K5120/B8 gen1/2/4 scores3537/3591/3637. These CPU
changes enabled the larger four-core search. All keep collision search current.

v53 94a8519 signed continuation coefficient. Optimism(-.25) gen4 seeds0–4 mean
3492.8 vszero3531.4, only2/5positive. Reject optimism; keep coefficient0.
Guidance fields33–48 alllose tofield15=3520; bestnewfield47=3444. Do not repeat
blind field sweeps or previously failed search tricks without a new hypothesis.

v54 **8eb59d3** multiple distinct evaluated parents between search generations.
Defaultelites1 preserves old algorithm. Full default controls are exact; tests
pass. K2048/gen4 parents1/2/4/8/16=3520/3537/3618/3465/3497 onseed3.
Four-parent seeds0–4=3553/3610/3484/3618/3394, mean3531.8 vs3531.4, only2/5pairs
positive. Preserve selected maxima, but no replicated average improvement.
K8192/elites8/gen4=3689; K16384/elites8/gen4=3673. Four-core K4096/5120
parent tests are complete; none improves3648. K8192 oneparent/gen4=3582, gen1=3596.

Predictive matching (existingflagR05_SCHED_PREDICT): currentK2048four3450 vs3520;
K5120=3541 vs3637 on bothfour/32 withexact fulltrajectory. No promotion.
Four-core extra parents:K4096/E4=3604,K5120/E4=3565,K5120/E8=3610; allvalid.

v55 **05559b7**, build-v55, new R05_PERSIST_ELITES (default1). Keep several
priority vectors across real steps, incumbentfirst. Next first-generation
non-global slots try each unchanged once, then mutate in rotation; global slots
remain global. Re-evaluate all scores/actions/reservations from current poses,
tasks and ages. Fixed work unchanged. One carry uses original algorithm.
Refactored elite selection is shared with intrastep parents; full default controls
must reproduce3637(K5120/E1) and3689(K8192/E8). Regression tests pass5.99s,
including task replacement, virtual task matches, duplicates and worker counts.
BinarySHA695c6c37a57c90d63fb935a9dc276b84556f106b60d300a92598c9b25c36b6d3.
Full comparisons queued; do not claim gain yet. Manifestprefixpersistent-elites-.

## Work still running

- `fresh-validation-v3-split-full-v52` / `seed50005-nms-repeat1`: job8900247.
- `fresh-validation-v3-split-full-v52` / `seed50005-nms-repeat2`: job8900248.
- `fresh-validation-v3-split-full-v52` / `seed50006-nms-repeat1`: job8900250.
- `fresh-validation-v3-split-full-v52` / `seed50006-nms-repeat2`: job8900253.
- `elite-scaling-four-split-full-v54` / `k5120-elites4`: job8900270.
- `elite-scaling-four-split-full-v54` / `k5120-elites8`: job8900271.
- `persistent-elites-four-split-full-v55` / `k2048-elites1-persist2`: job8900281.
- `persistent-elites-four-split-full-v55` / `k2048-elites1-persist4`: job8900282.
- `persistent-elites-four-split-full-v55` / `k2048-elites1-persist8`: job8900283.
- `persistent-elites-four-split-full-v55` / `k2048-elites4-persist4`: job8900284.
- `persistent-elites-32-split-full-v55` / `k5120-elites1-persist1`: job8900285.
- `persistent-elites-32-split-full-v55` / `k5120-elites1-persist2`: job8900286.
- `persistent-elites-32-split-full-v55` / `k5120-elites1-persist4`: job8900287.
- `persistent-elites-32-split-full-v55` / `k5120-elites1-persist8`: job8900288.
- `persistent-elites-32-split-full-v55` / `k8192-elites8-persist1`: job8900289.
- `persistent-elites-32-split-full-v55` / `k8192-elites8-persist4`: job8900290.
- `persistent-elites-32-split-full-v55` / `k8192-elites8-persist8`: job8900291.

Collect using split_grid.py; only compact JSON should be archived. Use direct
one-/two-level globbing for batch/spec/allocation/summary/completion JSON rather
than recursively traversing copied NMS working directories. Rawresult/native
logs/binaries stay ignored. Do not restart a job merely because qstat disappears;
NFS summaries can lag. Inspect original completion/qacct before any conclusion.

## Fable

User approved the exact79KB payload. Attempt at15:01UTC through claude-fable-5-1,
max, toolsdisabled, persistent session27a4316e-b79d-46cf-86b4-41b0f558938a failed
with providerHTTP429/model_requires_usage_credits. NO REVIEW, zerousage. User
already informed. This is not a remaining approval rejection. Optional review
does not block local work; do not re-ask permission for the same payload.
Payloadruns/random05/fable-review-01/payload.txt, SHA
 ded0889fcaa2cb82a3f4fd9d5dc46f737bdf50cf7a9e466bb66f9114f9661d0f.
It is an older7506a snapshot from08:16. Retry exactbytes in same initialized
session via--resume after creditsreturn; newoutputdirectory withcopiedmetadata
because runner refusesexistingstatus.json. Preservefailedattempt. Do not silently
replace the authorized payload with newsource. No subagent delegation authorized.

## Workspace / tools

Shared main with Warehouse agent. Only edit/stage/commit random05/ and
RANDOM05_PROGRESS.md. Leave cgar/,WAREHOUSE_PROGRESS.md,experiments/construction-
20260918/ and their jobs/stagedchanges alone. Commit using --only forourscope.
Publicfywu85/lorr; push authorized, no visibilitychange. Lastourscommits:
ca6e4ca waitingtracking/3689,05559b7 v55source. More recent sharedcommits mayexist.
Use require_escalated for shell (bwrap namespace failure); routine work allowed.
Python3.7; env/bin/python for NumPy generator. apply_patch broken; use Python.
GRID scripts:grid.py submit build; split_grid.py submit/collect. Four-core hosts
research33*|research39*, CPUexactAMD EPYC 9354 32-Core Processor. Wide32worker
host list is inexisting submissions. Never changefrozen case/specs after submission.

Push:
GIT_TERMINAL_PROMPT=0 GIT_ASKPASS= git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main
