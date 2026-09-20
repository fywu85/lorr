# RANDOM-05: active 4,000-task goal

Updated2026-09-20 23:57UTC. Goal ACTIVE: at least4,000 full2,000-step combined
completions, then independent fresh-input validation of the selected improvement.
Current best3,872; goal not achieved. Continue rather than stopping at a plan.

## Scope and rules

Only edit/stage/commit random05/ and RANDOM05_PROGRESS.md. Warehouse shares main,
staging and GRID; leave its files/jobs alone. No subagent delegation. Public
fywu85/lorr; pushing authorized, preserve visibility. Throughput is primary;
waiting secondary. Selected seeds/tricks can count. Explicit map tricks require
--trick RANDOM-05 and labels in commits/logs. User accepts shared hosts and
relaxed timing diagnostics. Strict frontier remains full2000,1s entry/30s prep,
32decimalGB. Complete fixed work or raise timeout; no partial portfolios.
Held-out task/start seeds50001–50008 are excluded from all tuning. None beyond
those generated yet. Freeze next source/config/protocol before new fresh inputs.

## Verified frontiers

- Overall3,872: source233f5bf/build-v69, seed4, first8000/K16320/B14,
  screening2/keep4/G4/E8/P8, directional cutoff mix.5/scale1.25, other settings
  best-32-workers.json.32workers/16physical EPYC9354, mean499.963/max535.477ms,
  RSS565672KiB. Finished2026-09-20T22:49:18.963497+00:00. +22.1% vs NMS32=3172.
  results/directed-triage-split-full-v69/...mix0.5-scale1.25-seed4/summary.json.
  Independent full replay passed,76timestamped frontier rows audited. Max
  completed wait1944steps; initial unfinished138/unopened96; censored>=2000.
- Four-core3,770 unchanged, source5f81613/build-v65,seed3,first4608/K5760/B12,
  screen2/q4/G4/E8/P8/triage1.5/cache512; mean788/max845ms,RSS485MB. +29.4%
  vs strongest NMS4=2914. worker-affinity-four-full-v65 is UNSPLIT.
- Frozen freshV4 four-core source65 candidate:3680vsNMS2907/2870 on50007;
 3641vs2930/2918 on50008. Aggregate+25.42% vs stronger repeats, allsixoriginal
  strict runs and independent replays passed,29compiled source/header hashes
  audited. Protocola7bad0c preceded generation. Later records do not replace it.

## Source and controls

- Source74 commit1e266b0 pushed, build-v74: optional R05_SCORE_RANK_STEPS limits
  positive task-rank scoring to startup. Default0 means unlimited; rank power0
  remains baseline. Boundary/same-state/checkpoint/worker tests passed19.23s.
  SHA4c2f9b78b10e0f83d30d4b71bf4a12f3d70b2fc07f558e5d70649598acd65e13.
- Source75 CURRENT, build-v75 tests passed20.30s: R05_RESCORE_ROOTS default0,
  R05_RESCORE_BRANCHES default64, R05_RESCORE_BLEND weight of original score.
  Completed distinct first-decision finalists get independent common priority
  futures. Separate RNG, no unrevealed tasks, full work, ordinary parent history
  with chosen root first. One-root/blend1controls exact. Replan/component
  combination rejected. SHA2c13ae9c84c81b5100391cc3ae23de1671cb85907abe9d87498ab5e26662b02d.
- Source73 build-v73: bounded nested workers and optional faithful G/E/P for
  closed-loop forecasts. Defaultoff. Initial heavy strict attempts failed.
- Source72 rank weights (explicit trick) normalize reverse chain-cost ranks
  to mean1, without changing PIBT aging. Whole-run settings rejected below.
- Source71 waypoint-age retention default0, all nonzero tests rejected.
- Source70 optional closed-loop forecast reuses real solver on currently visible
  pool, no replacement stream. Copy runtime, share immutable graph/chains.
  Defaultoff; initial full variants lose. Source68 component recombination loses.

## Active GRID batches

Collect using python3 random05/tools/split_grid.py collect --output runs/random05/<batch>.
All development cases use the archived input and seed4 unless explicitly noted.

- branch-allocation-split-full-v69, jobs8900883..88: B18/22/26 at near-fixed16k
  work and fixed3264roots. Fullstrict, unchanged3872settings otherwise. Current
  progress900–1600steps around23:54UTC; no results yet.
- startup-rank-split-full-v74, jobs8900896..8900902: powers0/unlimited.5controls,
  .25/.5/1 for250steps and.5for100/500. Fullstrict. Around500–600steps23:54.
- independent-rescore-split-full-v75, jobs8900910..16: off/one-root controls,
  R4B32,R16B64,R16B128,R16B64blend.5,R32B128. Fullstrict just started.
- faithful-replan-split-full-v73: legacy-policy R4F2K32/threads4 control must
  reproduce3752; running around1800steps23:54. R4F2K128/R8F1K128 strict both
  failed atfirstenabledstep50 (1135/1078ms). Heavy5sdiagnostics R4F4K128 and
  R4F2K256 running around1300/800. Preserve original strict failures.
- faithful-smaller-split-full-v73: K96/depth6 failedstrict at23:38; K64/depth8
  stillrunning around1400steps23:54. Both R4F2/threads4/faithfulpolicy.

## Recent completed evidence

- Whole-run rank powers0/.25/.5/1/2 =3872/3867/3845/3670/3716; rejectnonzero.
  .25 original refused before launch on invalid binding; its separately named
  allocation repeat valid. Archived both batches, zero exactly matches sixfields.
- Phase audit results/rank-progress-phase-v72: .5 first250=471vs413baseline,
  last250=562vs621; full3845vs3872. Different trajectories, not a predicted
  switching gain. Startup-only experiment tests this phase tradeoff directly.
- waypoint-age-split-full-v71 retain0/.25/.5/.75/1=3872/3790/3711/3785/3749,
  allvalid, zeroexact. Archived, rejectnonzero.
- replanning-split-full-v70 R0/R1 controls3872exact; R4F2K32=3752; valid
  allocationrepeatR8F2K32=3790. Otherstrict R8F1K32/R4F2K64timedout; separate
  5sdiagnostics=3682/3723. Allarchived, no promotion.
- guidance-retune-split-full-v69 all8valid/alllose:3746/3827/3707/3714/3714/
 3780/3740/3663. Keepfieldunchanged.
- directional cutoff mix.5 across plannerseeds0/3/4=3778/3845/3872 vs
 3780/3785/3857 (two positive, aggregate+.64%, notfreshvalidation). Mix.75
  seed4=3858; mix.25strict timeout1575 then declared5sdiagnostic3856.
- Completionbonus2/4/8 underB14=3746/3811/3781vs3852, allvalid/rejected.
- Saved-state diagnosticsv67: sevenrealstates,168valid128-stepcontinuations at
  cheapK1280. Weak late root-score correlation is a small correlated diagnostic,
  not proof of a scoring defect or achievable gain. Full-budget controls exact.

## Resources, audit and consultation

23verifiedEPYC9354hosts in results/reference-host-expansion-20260920/hosts.txt.
New batches exclude research34 after two refused core-binding allocations;
no cause asserted. Exactmodel and physical16/logical32 guards,32GBguard. Keep
allocation failures before any identical-setting repeat. Warehouse jobs unrelated.
Archive direct *.json and */*.json metadata named summary/completion/spec/
allocation/submission/batch only; no raw traces or NMS cwd. New strict frontier:
independent action_audit replay, source/binary checks, best manifests, timestamped
progress row, waiting manifest, audit_progress/audit_task_waits/render_waiting_report.
Keep selected maximum separate from replicated means and fresh-input validation.

Fable persistent session27a4316e-b79d-46cf-86b4-41b0f558938a.21:55UTC summary-only
attempt failed provider credits; no review received. Do not retry unchangedquota
or claim feedback. Exact approved79KBpayload stays runs/random05/fable-review-01/payload.txt.
Push works per-command (stale VSCode helper):
env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git -c credential.helper=
-c 'credential.helper=!gh auth git-credential' push origin main
Only commit our paths with git commit --only ... -- random05 RANDOM05_PROGRESS.md.
