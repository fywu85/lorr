# RANDOM-05: active 4,000-task goal

Updated 2026-09-21 01:03 UTC. The persistent goal is ACTIVE: reach at least
4,000 tasks in a full 2,000-step combined run, then validate the selected
configuration on fresh inputs. The verified best is 3,928, leaving 72 tasks.
The user explicitly accepts a selected planner seed. Do not mark the goal
complete or stop at a plan.

## Scope and permissions

Only edit, stage and commit `random05/` and `RANDOM05_PROGRESS.md`. Other agents
share main, staging and GRID; leave their files and jobs alone. No delegation.
The public `fywu85/lorr` repository may be pushed; preserve its visibility.
Throughput is primary and waiting is secondary. Map tricks require the explicit
`--trick RANDOM-05` flag and labels in commits and logs.

Shared hosts and relaxed timing diagnostics are authorized. The strict frontier
still requires 2,000 steps, 1s entry, 30s preprocessing and a 32 decimal GB guard.
Finish all declared search work or raise a timeout; never return partial work.
Task/start seeds 50001–50008 are excluded from tuning. No later fresh inputs
have been generated. Freeze the next protocol, source and configuration first.

## Verified records

- Overall: **3,928**, source `233f5bf`, build-v69, planner seed5, first K7968,
  then K16320/B18, screening2/keep4, G4/E8/P8. Other settings are pinned in
  `best-32-workers.json`: field15, one flip with seed5, directional cutoff
  mix0.5/scale1.25. Startup weighting and finalist rescoring are OFF.
  Finished 2026-09-21T00:19:49.212073+00:00. Mean499.181/max554.944ms,
  RSS546252KiB. 32 workers on16 physical EPYC9354 cores. +23.8% versus NMS3172.
  Evidence: `branch-seeds-split-full-v69/32-branch-seed-b18-seed5`.
  Independent replay passed; all79 frontier rows are audited. Maximum completed
  wait1952; initial unfinished130/unopened88; eventual maximum is censored>=2000.
- B18 is a selected maximum, not a replicated mean improvement. Seven paired
  planner seeds total26705 versus B14's26778 (-0.273%), with four positive pairs.
- Previous3,917: source `1e266b0`, build-v74, seed4, B14/first8000/K16320,
  rank power0.25 during the first250 steps. Replay passed. Mean488/max532ms.
  Its five paired seeds total19155 versus19182 (-0.141%), two positive pairs.
- Four-core record remains **3,770**, source `5f81613`, build-v65, seed3,
  first4608/K5760/B12, G4/E8/P8, triage1.5/cache512. Mean788/max845ms.
  +29.4% versus strongest NMS4=2914. `worker-affinity-four-full-v65` is UNSPLIT.
- Frozen freshV4: 3680 versus NMS2907/2870 on50007; 3641 versus2930/2918
  on50008. Aggregate+25.42% against stronger repeats. All six original runs,
  compiled source checks and independent replays pass. Protocol `a7bad0c`
  preceded generation. Later records do not replace this frozen comparison.

## Current source and tooling

Build-v76 adds optional `R05_RESCORE_STATIC_WEIGHT`: -1 preserves the original
mean; [0,1] fixes the unchanged-priority branch's weight independently of the
number of randomized futures. All declared branches finish. Nonzero risk with
explicit weighting is rejected. Endpoint, duplicate-sample, worker, checkpoint
and unchanged-decision tests pass (31.82 s). Binary SHA:
`5332e1acf082031df3d6a56baadf526e15541427836b86cabb9b1bd65c330213`.
The source and current evidence are being committed together; link a later
record to that source commit, not the old build-spec HEAD from the shared tree.
Source75 is `e84533a`; source74 is `1e266b0`. The current overall frontier
continues to use source69 `233f5bf`, with rescoring and startup weights off.

`audit_fresh.py` now accepts a frozen allocation declaration (legacy default:
four cores) and optional `baseline_source_commit` / `baseline_binary_sha256`
with `seedN-baseline` cases. It checks solver and simulator files plus protocol
commit order. Three allocation tests pass and the old V4 comparison reproduces
exactly+25.42%; evidence is in `fresh-auditor-32-preparation`. No new protocol or
candidate has been frozen yet.

## Active GRID batches

Collect with `python3 random05/tools/split_grid.py collect --output runs/random05/BATCH`.

- `rescore-static-mixture-split-full-v76`, jobs 8901070–77: eight full strict
  cases, submitted around 00:57 UTC. B64 weights -1/0/.05/.1/.25 and B128
  weights 1/64/.05/.1. R16, B14 main search, planner seed4, no startup.
  Legacy control must reproduce 3,906 exactly.
- `record-rescore-allocation-repeat-split-full-v75`, job 8901018: identical
  B18/seed5 + R16B64 after a research58 preflight refusal. Original preserved.
  Collect before assuming completion; this is a separately declared attempt.

All earlier batches through new-guidance-fields are complete and archived.

## Latest completed comparisons

- Source75 controls reproduce 3,928 and 3,917 exactly in all six trajectory
  fields. Independent R16B64 rescoring at B14 improves planner seeds0/3/4
  by27/48/34 tasks: totals11,604 versus11,495 (+0.948%). These are paired
  planner seeds on one input, not fresh task/start validation.
- Startup + rescoring scores3,867 (R4B32) and3,769 (R16B64), below3,917.
  At B18/seed5, startup alone3,747; R4B32 alone3,898; both startup and
  R16B64 give3,845. The R16B64-only case had an allocation refusal.
- The sixteen predeclared B18 planner seeds9–24 produce15 valid full runs,
  best3,904 atseed24. Seed11 hits the strict deadline atstep1455,1,189ms;
  no claim about its unproved cause and no completed score. Preserve it.
- Guidance fields49–56 score3,394/3,323/3,722/3,533/3,670/3,755/3,367/3,899.
  All valid; none improves3,928. These remain labeled map-specific tricks.
- The separately declared startup0.25/125 allocation repeat scores3,718.

## Important completed results

- Independent rescoring: off/R1 controls3872, exact in all six trajectory fields.
  R4B32=3885, R16B64=3906, R16B128=3659, R16B64/blend0.5=3819, R32B128=3867.
  All valid;3906 is below the overall frontier. The three paired checks are summarized above.
- Startup initial batch: controls3872/3845 exactly reproduced. Power0.25/250
  gives3917; power0.5 for100/250/500 gives3879/3756/3801; power1/250 gives3755.
  The3879 run finished after3917, so it is not a separate frontier.
- Startup follow-up: powers0.125/0.375 at250 give3800/3926; power0.25 at375/500
  gives3724/3881; B18 coupling onseed4 gives3754. Seeds0/3/5/6 give
  3762/3862/3780/3834. Original125-step case never launched: research42 exposed
  64 cores instead of16. Its separately named identical repeat scores3,718.
- Branch allocation: B18/B22/B26 near fixed work give3877/3732/3705;
  fixed3264 roots give3837/3871/3772. All six valid.
- Branch seeds: B18 seeds0/3 give3666/3675. B14/B18 pairs: seed5=3819/3928,
  seed6=3868/3832, seed7=3832/3851, seed8=3764/3876. All valid.
- All closed-loop forecast batches are terminal and lose. Legacy source73
  control3752 is exact; smaller K64/depth8 gives3785. K128 strict variants and
  K96/depth6 time out. Heavy5s diagnostics give3723 and3682. Keep disabled;
  original failures and earlier source70 negatives remain archived.
- Whole-run rank weighting, waypoint-age retention, completion bonuses,
  component recombination and the eight local guidance retunes all lose.
  Keep these failed presets off. Full details remain in progress/research logs.

## Audits, resources and consultation

Phase audit `startup-rescore-phase-v75` shows startup3917 is29 behind atstep500,
then recovers. Rescoring3906 also gains mainly later. Different trajectories mean
blockwise gains cannot be added. Older seven-state counterfactual diagnostics
are a small correlated sample, not proof of recoverable throughput.

The verified EPYC9354 list is `results/reference-host-expansion-20260920/hosts.txt`.
New batches exclude research34,42 and58 after binding refusals. The installed GRID
manual says binding is advisory; `m_topology_inuse` is not consumable. The specific
cause of each refusal remains unproved. Keep the actual affinity/quota/32GB guard.
Never alter other jobs or allocations, or silently discard refused attempts.

Archive only direct `*.json` and `*/*.json` metadata named summary, completion,
spec, allocation, submission or batch. Never copy raw traces or NMS cwd into git.
For a new maximum: independent action/task replay; source/binary/input checks;
best manifests; timestamped progress row; waiting manifest; `audit_progress.py`;
`audit_task_waits.py`; `render_waiting_report.py`. Keep selected maxima separate
from paired means and independent-input validation.

Fable CLI session `27a4316e-b79d-46cf-86b4-41b0f558938a` failed provider credits
at21:55UTC. No new review was received. Do not retry unchanged quota or claim
feedback. The exact approved79KB payload remains under `runs/random05/fable-review-01/`.

Push using the per-command helper to bypass stale VSCode askpass:
`env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main`.
Use `git commit --only ... -- random05 RANDOM05_PROGRESS.md` to preserve other staging.
