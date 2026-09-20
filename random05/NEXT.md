# RANDOM-05 handoff: 4,000-task goal

Updated2026-09-20. The user explicitly authorized a new goal: reach4,000tasks
on the full archived RANDOM-05 combined benchmark, then independently validate
the selected improvement. The previous reasonably-close-to-colleague goal is
complete; its evidence remains in RESULTS.md and results/completion-audit.json.

Only edit/stage/commit random05/ and RANDOM05_PROGRESS.md. Warehouse shares main;
leave its files, staged work and GRID jobs alone. No subagent delegation.
Pushing fywu85/lorr is authorized; preserve public visibility. All map-specific
techniques require --trick RANDOM-05. Throughput is primary, waiting secondary.

## Verified frontiers

- Overall32workers/16physical EPYC9354cores: **3857**, source5f81613/build-v65,
  seed4, first8000/K16320/B14/s2/q4/G4/E8/P8, horizon2000/triage1.25.
  Mean490.729ms/max551.626ms/RSS564632KiB. Strict1s/32decimalGB/full2000,
  zeroerrors/timeouts; independent full replay passed. Evidence:
  results/record-triage-split-full-v65/32-record-triage1.25-seed4/summary.json.
  Onlyfive tasks above3852(scale1.5); no robust seed gain claimed. Scale1.75=3798.
  Matched NMS32=3172, gain21.6%. Record source remains65, not the new prototypes.
- Four physical cores/four workers: **3770**, same source65, seed3,
  first4608/K5760/B12/s2/q4/G4/E8/P8/triage1.5/cache512.
  Mean787.573ms/max845.398ms/RSS473328KiB, +29.4%vsNMS4=2914.
  Evidenceworker-affinity-four-full-v65/summary.json (UNSPLIT).
- All75frontier rows audited. best.json/best-32-workers.json select3857;
  other best manifests unchanged. Current completed max1941/1937vsNMS1997/1976;
  initial unfinished137/135vs219/206. Every solver still has initial orders
  unfinished at2000, so eventual max is unknown and >=2000.
- Frozen freshV4 candidate3770config:3680vsNMS2907/2870 and3641vs2930/2918,
  **+25.42% aggregate** over stronger repeats. Allsix original strict runs valid,
  full independent replay and29compiled source/header hash checks. Protocol
  a7bad0c precedes generation50007/50008. ALL50001–50008 are excluded from tuning.
  Previous freshV3=25.46% on different inputs/config; not a paired causal estimate.
- Historical no-horizon ablations atscale1.5:3503/four and3632/32 vs3770/3852,
  cutoff+7.6%/+6.1%; guidance remains a trick. Do not claim no-tricks performance.

## Current source and diagnostics

- Best source65 binarySHA5c012a610f8ccae75a515ae281f3099822f76a629725af3ebc30b34b7e1bfcf3.
- Source9c7a78b/build-v67 adds exact saved-state diagnostics; regressions15.05s.
  Full snapshot capture reproduces ALLsix fields of the3852 trajectory.
  Saves states250..1750 and8distinct fully evaluated alternatives at expanding
  ranks. No extra random draws or selection changes. No snapshot atstep0.
- Separate random05_decision_probe restores state, forces one completed root,
  then128steps of closed-loop replanning atK1280 under common seeds0/1/2.
  Simulator harness reveals actual task stream only as tasks finish; planner
  never sees future tasks. Allseven snapshot states match original pools,
  stages, positions, headings and assignments. Full-budget continuation control
  matches5cumulative task/motion metrics at every one of128steps.
- results/decision-probes-v67/REPORT.md: chosen mean is best at250; lower-scored
  alternatives do better atother sampled states. Spearman at128steps mostly
  negative later; leave-one-seed-out gains mixed, positive in last3states.
  Tiny correlated sample and cheaper downstream policy: no causal scoring-defect
  or throughput-gain claim. These are diagnostics, never frontier scores.
- Source332ccdb/build-v68 adds general optional motion-component recombination.
  Default R05_COMPONENT_TRIALS=0. Extra fixed root trials*rounds*B fully finish.
  Couple agents through either parent's promises, including shared empty targets.
  Swap complete dependency groups, force same first decision in every branch,
  reevaluate later interactions normally, keep incumbent. No wall-clock stopping.
  Dense regression25.69s covers empty-hole collision trap, independent turns,
  shared-prefix exactness, worker determinism, task locks and checkpoint replay.
  Components incompatible with operation planner or early-fill, which change
  first positions. Build-v68 SHA91f28662893f1abaff2806e8ecc8dda6f2b6e338e27a01b85e178dba6033fdfa.

## Active experiments

- runs/random05/components-split-full-v68, jobs8900771--8900774:
  source68 controloff; trials32/rounds4/min1; trials32/rounds4/min2;
  trials128/rounds2/min2. Allsource65record settings, seed4/triage1.5,
  strict1s on matched hosts. Early extra work45–91ms; no full outcome yet.
- runs/random05/record-triage-seeds-split-full-v65, jobs8900778/8900779:
  scale1.25 seeds0/3, compare historical3785/3794 atscale1.5.
- runs/random05/averaged-completion-split-full-v65, jobs8900781--8900783:
  bonus2/4/8 atbase3852settings. Earlier single-future bonuses failed; the
 14-future averaged score is a specific reason to recheck reward noise. Same
  fixed budget and strict deadline, no source change. Do not assume a win.
- All other Random05 work from this turn completed. Warehouse jobs unrelated.

## Fable

Persistent session27a4316e-b79d-46cf-86b4-41b0f558938a. At21:55UTC a6664byte
summary-only follow-up (no new source payload) again failed: provider usage credits,
zero cost/usage, no review. Evidence results/fable-4k-diagnosis-01.
Do not claim feedback. Earlier exact approved79KB payload remains preserved in
runs/random05/fable-review-01/payload.txt; never silently replace approved bytes.

## Prior failures and cautions

More work is not monotonic:K24000seed3=3702vs3794,K28800seed4=3706vs3852.
K16320 completed five-seed mean3771.4 vsK8192/B10 mean3690.6(+2.19%,5/5), but
one original seed2 attempt timed out; declared strict repeat and5s diagnostic
both exact3705. Original failure retained. Cannot assert proven contention cause.
FourK7056 attempts fail atstep1; no score. Setup-loop fusion was slower despite
exact output, reverted. Most guidance variants, simplecompletion rewards,
longer horizons, greedy future task turnover, predictive matching, variance
optimism, mutation decay, local cycle tweaks and early-fill failed. See RESEARCH.

## Recording and reproduction

Full2000steps decide frontiers. No partial portfolio on timeout: exit124.
Use GRID frozen builds/cases, disjoint physical bindings,32GB process guard.
Shared hosts are allowed. Reference CPU9354 differs from historical7R32, so
published competition scores are indirect. Collect split_grid.py batches before
archiving only direct *.json and */*.json metadata namedsummary/completion/spec/
allocation/submission/batch; never recursively copy raw traces or NMS cwd.
For new frontiers update best manifests, timestamp/source/evidence table, waiting
manifest, audit_progress.py, audit_task_waits.py and render_waiting_report.py;
independently replay actions/tasks. Capture exact6trajectory fields for controls.
Commit/push only the Random05 scope, preserving others' staged files.

- New build-v69 directional deadline experiment (see RESEARCH):
  R05_TRIAGE_GUIDED_MIX=0/.5/1 normalizes oriented chain costs to the same total
  hop estimate. Requires declared horizon/trick, keeps started assignments.
  Regressions16.81s passed, SHA
  b2d044351e254b40e6dddab7828f2de32defd60470edaa3fcaa6f8b7f9335746.
  Five strict full cases in directed-triage-full-v69.json: mix0scale1.25control,
  .5/1 atscales1.25/1.5, seed4. Pending results; current source69 defaults mix0.
- Plain git push failed through a stale VSCode socket. The successful per-command
  override uses `env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git
  -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main`.
  No credential was printed and repository authentication settings unchanged.
