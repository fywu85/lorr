# RANDOM-05: active 4,000-task goal

Updated2026-09-20 23:12UTC. Continue toward at least4,000tasks on full2000-step
Random05 combined runs and independently validate the selected improvement.
The persistent goal is ACTIVE;3872 does not complete it. Previous goal (close to
colleague's matched gain) was achieved on frozen fresh inputs; retain that audit.

Only edit/stage/commit random05/ and RANDOM05_PROGRESS.md. Warehouse shares main;
leave its changes/staging/jobs alone. No subagent delegation. Public fywu85/lorr;
pushing is authorized, preserve visibility. User prioritizes throughput over
waiting, accepts selected seeds/tricks, and allows shared hosts and relaxed
timing diagnostics. Strict frontier remains full2000,1s/30s,32decimalGB cap.
No partial search portfolio: finish fixed work or timeout. Map-specific tricks
require --trick RANDOM-05 and explicit labels in commits/logs.

## Verified frontiers

- Overall: **3872**, source233f5bf/build-v69, seed4, first8000/K16320/B14,
  screen2/keep4/G4/E8/P8, cutoffscale1.25/directional mix.5, all other settings
  in best-32-workers.json.32workers/16physical EPYC9354cores, mean499.963ms,
  max535.477ms,RSS565672KiB. Finished2026-09-20T22:49:18.963497+00:00.
  +22.1% vs matched NMS32=3172;128tasks from4000. Selected maximum.
  Evidence results/directed-triage-split-full-v69/...mix0.5-scale1.25-seed4/.
  Independent full action/task replay passed, controlmix0 reproduces3857.
- All76 timestamped frontier rows audited. best.json/best-32-workers.json,
  RESULTS.md, progress log and waiting reports updated. Current max completed
  order1944steps, initial unfinished138/unopened96; eventual max censored>=2000.
- Prior3857 source65 differs from3852 only in cutoffscale1.5->1.25.
  Replication of that scale alone failed: seed0 3780vs3785, seed3 3785vs3794.
  Newmix.5 seed3=3845(+60vs3785); seed0=3778(-2). Across0/3/4,total+0.64%,
 2of3positive; see paired-outcomes.json.
  mix.75seed4=3858. mix.25strict failed at1575 (1985ms), diagnostic queued.
- Four-core best remains3770, source5f81613/build-v65,seed3,first4608/K5760/B12,
  screen2/q4/G4/E8/P8/triage1.5/cache512; mean788/max845ms,RSS485MB.
  +29.4% vs strongest NMS4=2914. Evidence worker-affinity-four-full-v65 is UNSPLIT.
- Frozen freshV4 (a7bad0c) candidate3770config gives3680vs2907/2870 on50007,
  and3641vs2930/2918 on50008,+25.42% aggregate over stronger repeats. Allsix
  original attempts strict valid, independent replay/source audit passed.
  ALL task/start seeds50001–50008 excluded from tuning. Later records do not
  replace this frozen result. Generate fresh inputs only after freezing the next
  selected candidate; private colleague absolute counts are not matched inputs.
- Earlier no-horizon ablation source65:3503/four,3632/32 vs3770/3852(scale1.5).
  Cutoff+7.6%/+6.1%. Guidance remains enabled; do not call them no-tricks.

## Current source

- 0eb4781/build-v70: optional closed-loop forecast reranking. Default
  R05_REPLAN_ROOTS=0. Share immutable graph/chain tables, separate runtime/caches.
  Pick fully evaluated distinct first actions/promises at expanding ranks;
  execute first then7 repeated compute calls atinnerK32/B4 (or64), common
  future seeds, one generation/one worker per forecast, parallel across futures.
  Only current visible pool: no replacement stream or invented tasks. Rebase
  progress when tasks change; certify every action, task lock and pool count.
  All declared work completes before choosing. One-root dense control preserves
  actions, schedules and all persistent state; worker/checkpoint/depletion tests
  pass27.78s. SHA2f2349293485bbc12b2bb1c5cc1dac37e98102549f41f8e41759d7322d82327b.
- 2fe4937/build-v71 CURRENT, pushed: optional R05_WAYPOINT_AGE_RETAIN in[0,1],
  default0 exactly resets as before. Retain partial/full age at intermediate
  waypoint; whole-task completion still resets. Dense regression17.89s covers
  age lifecycle, locks, worker/checkpoint determinism and simulated aging.
  SHA72a72860a73b397f4612a55bf40c50ac5908787bfae589fdff64ceed282ffc48.
  No performance benefit established yet. Best manifest stays source69.
- source69 directional cutoff mix defaults0. Normalize oriented remaining costs
  to preserve total hop estimate, blend and apply existing cutoff. Explicit trick.
- source68 motion-component recombination defaultsOFF, REJECTED:3702/3721/3740
  vs3852 exact control; all valid. Don't reactivate these presets.
- source67 checkpoint/counterfactual diagnostics preserve3852 full trajectory.
  Seven states250..1750,8alternatives*3seeds*128steps atK1280,168valid traces.
  Original-budget replay control exact every step; all snapshots match true
  simulator pools/positions/headings/stages. REPORT results/decision-probes-v67:
  late score rankings weak/negative; tiny correlated cheap-policy sample, not
  proof of scoring failure or an achievable throughput increase.

## Active GRID batches (collect before archiving)

Use python3 random05/tools/split_grid.py collect --output runs/random05/<batch>.
- directed-triage-followup-split-full-v69:8900825..28;mix.5 seeds0/3 and
  mix.25/.75 seed4,scale1.25. seed3=3845, .75=3858; .25strict failed1575;
  seed0=3778; batch complete. First strict failure retained.
- directed-triage-quarter-diagnostic-split-full-v69: same .25case at5s, job8900863; read batch.json for details. Diagnostic only, no strict promotion.
- replanning-split-full-v70:8900832..37; baseline3872 settings, rerank starts50.
  CasesR0F1K32control;R1F1K32control;R4F2K32;R8F1K32;R8F2K32;R4F2K64.
  R8F2 preflight binding failure(expected16cores,observed64),solver neverstarted.
  R4F2K64strict timeout62 (1051ms);R8F1K32timeout459 (1020ms). R0control=3872exact allsix fields; other2running.
- replanning-allocation-repeat-split-full-v70:8900839,R8F2K32same solver,
  new valid binding on research39; original failure retained. Strict.
- replanning-relaxed-diagnostic-split-full-v70:8900854/55,R8F1K32 andR4F2K64
  with5s cap. Assess throughput despite strict failures; not strict records.
- guidance-retune-split-full-v69:8900840/41/43..48. **Trick** eight single-factor
  changes at3872preset/seed4: noflip;2/4prefixflips(seed5);contrast2/2.8/3.2;
  turn.4/.8. Full strict runs, not short screens. No held-out inputs.
- waypoint-age-split-full-v71:8900856..60,retain0/.25/.5/.75/1,3872preset/seed4,
  fullstrict. Retain0 control must reproduce3872 in all trajectory fields.
- Warehouse jobs are unrelated. Never alter their jobs/files/staging.

## Diagnostic finding for next decisions

results/4k-gap-audit-v69 (reproduce tools/audit_phase_gap.py): ours3872 vs NMS32
first250steps:413vs521 tasks,2196vs2181waypoints,64590vs65286forwards. Initial
assignments both314two-stop chains; ours slightly less internal hopwork.
Ours wins every later250-step block. This is observational concentration of
progress, not108recoverable tasks. NMS priority is remaining-task distance and
Random05 rank weights squared (read-only nms robot_handler.cpp/pibts.cpp).
Age retention tests chain continuity without replacing stable inheritance with
shortest-remaining-work priority. It may also hurt by favoring long chains.
Completion bonus2/4/8 with14-future averaging already failed:3746/3811/3781
vs3852. First250only401/436/428vs413; not an obvious108-task fix.

## Resources, records and consultation

23 verified EPYC9354 hosts are in results/reference-host-expansion-20260920/hosts.txt.
Use --hosts with that pipe-separated list and exact --cpu-model check, avoiding
needless33/39queue restriction. Runtime guards exact physical binding, logical
quota and32GB. One research34binding failure above was refused before solver
launch; retry excludes34. No cause asserted for occasional strict runtime spikes.

Archive only direct *.json and */*.json metadata named summary/completion/spec/
allocation/submission/batch. No recursive copying raw traces or NMS cwd. For each
new strict frontier: independent action_audit replay; best manifests; timestamped
source/evidence row; waiting manifest; audit_progress; audit_task_waits; render.
Keep selected maximum separate from replicated means and fresh-input validation.

Fable persistent session27a4316e-b79d-46cf-86b4-41b0f558938a:21:55UTC summary-only
review attempt failed due provider credits, no review received. Original exact
approved79KB payload stays runs/random05/fable-review-01/payload.txt. Do not claim
feedback or keep retrying unchanged quota. User authorized consultation through CLI.

Push succeeds with per-command GitHub CLI credential helper (stale VSCode socket):
`env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git -c credential.helper=
-c 'credential.helper=!gh auth git-credential' push origin main`.
No global auth changes or credentials in output. Only commit our owned paths.


## Latest continuation (source73; update older running labels above)

- Source72 rank-weighted progress is the new optional trick, power0default.
  Powers0/.25/.5/1/2 full runs in rank-progress-split-full-v72, jobs8900865..69.
  .25 original failed binding before launch; repeat in
  rank-progress-allocation-repeat-split-full-v72. Other4running. Sourcef76c88e,
  regression27.92s, SHA18b3fd752a588bbd6423bc6e0400fd27c054c9dfe3b25894f58cddd8faf383a3.
  Real binary refuses positive power without --trick RANDOM-05. Weights rank
  remaining chain cost, tied ranks shared, normalizedmean1; prioritiesunchanged.
- Source73 current: nested worker allocation and optionally preserve liveG/E/P
  inside forecasts. Defaults threads1/policy0 preserve source70 behavior.
  Regressions28.72s cover worker-split equivalence. Five cases in
  faithful-replan-full-v73.json; new strict legacy-policy control must reproduce
 3752. Two K128 strict candidates and two5s diagnostic heavier candidates.
  OMP_WAIT_POLICY=PASSIVE,OMP_THREAD_LIMIT=32. Active workers bounded by32.
- Initial replan strict batch complete: R0/R1 controls3872exact allsixfields;
  R4F2K32=3752; valid allocation repeatR8F2K32=3790. Leave defaultoff.
  RelaxedR8F1K32 andR4F2K64 stillrunning; originalstrictfailuresretained.
- Guidance retune all8complete andalllose:3746/3827/3707/3714/3714/3780/3740/3663.
- Age retention0control3872; .25=3790,.5=3711,.75=3785;1stillrunning.
- Best remains3872 source69. No new frontier. Updated waiting/audits remainvalid.
