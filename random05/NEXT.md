# RANDOM-05 continuation

The persistent goal is active and not complete: work toward the colleague's
roughly27–28% matched advantage in the combined track. Their private instances
and code are unavailable. Our archived RANDOM-05 input is fixed at800 robots,
2,000 steps. Do not equate absolute task counts across different inputs.

Read `RANDOM05_PROGRESS.md` first. Best overall:3,231 on32 workers, compared
with3,172 NMS under the same allocation. Four-core best:3,127 versus2,903 NMS.
`random05/best.json` and `random05/best-four-cores.json` contain exact cases.
Both bests use explicit `--trick RANDOM-05`, tuned guidance and known-horizon
triage. Best without horizon triage:2,914. General default guidance-none was
1,774 early in the campaign and has not been re-evaluated with every improvement.

## Pending full runs

| GRID job | Run directory under runs/random05 | Purpose |
|---|---|---|
|8899454|nms4-repeats-full-v3|Two repeat four-worker NMS references; about32minutes each|
|8899456|large-search-full-v15|32-worker K4096, mutation0.1/0.05, noise400|
|8899457|compound-search-full-v16|32-worker scheduler combinations and blocked-cycle/baseline mixture|
|8899458|blocked-cycle-thresholds-full-v16|Four-core cycle-gain thresholds and mixed portfolio|

Check `summary.json` and each case's `summary.json`; final summaries may appear
slightly after the solver log reaches step1900. Native NMS logs are buffered and
can remain empty while the job is healthy. Do not restart a live NMS run for that.

Next: collect those results, update best manifests and the progress log for any
valid new maximum, then validate promising compounds on additional seeds. Keep
single-seed maxima distinct from averages. The blocked-cycle change improves
some maxima but lowers the six-seed mean slightly; do not call it a robust win.
The 32-worker NMS reference is one timed run; repeated references characterize
its noise before a stronger claim. The goal remains well short of the colleague's
reported relative advantage despite a first match of the local winner.

## Source and experiments

- Build-v15 binary: source commit134faa8. Highest known overall score3,231.
- Build-v16 binary: source commit6aed8ba. Blocked-cycle and portfolio features;
  highest four-core score3,127.
- Build-v17 binary: source commite55f951. Experimental discounted-progress
  scoring and turn-load weights; all tested settings lower throughput, defaults
  remain off. Build and regression tests passed.
- `random05/results/` holds committed compact summaries/specs/allocations;
  ignored `runs/random05/` holds frozen sources, binaries and full traces.
- `random05/results/progress-audit.json` verifies23 frontier entries against
  their recorded score, UTC completion, validity, binary SHA and exact planner
  source at the linked commits (checked via frozen build source hashes).
- No default environment preset silently enables the best trick configuration.
  Reproduce from the relevant manifest rather than running bare defaults.

The repo is shared with the Warehouse session. Stage only `random05/` and
`RANDOM05_PROGRESS.md`; do not modify/stage its CGAR changes. User permits pushes
to public fywu85/lorr. Do not change repository visibility.

## GRID and limits

Use `python3 random05/tools/grid.py submit --kind build|benchmark ...`.
The shell requires `sandbox_permissions=require_escalated` because local bwrap
fails. Builds use env/bin/cmake and env/bin/x86_64-conda-linux-gnu-c++.
Full cases are fixed-work,1s entry limit,30s preprocessing,32GB process cap.
The 32-worker allocation is16 physical cores with two SMT threads, matching the
archived 2024 evaluation topology (NMS readme/Evaluation_Environment.md).
Four-worker cases use four physical cores, one thread each.

The runner reserves both logical slots per physical core, requests enough free
cores through m_topology_inuse, and rejects an unbound allocation. This load
attribute needs qsub -w n; static -w e incorrectly rejects suitable queues.
Memory reservation is at least32GiB total, and prlimit never raises an inherited
hard limit. Older nms4-repeats-full-v2 failed before solver startup because its
inherited limit was smaller; v3 is the corrected resubmission.

## Fable review remains pending

Automatic approval review explicitly rejected sending the prepared79KB source
bundle to external Fable5.1 via Claude Code. An exact-payload approval question
was sent asynchronously; no answer has arrived. Do not resend or bypass it.
Payload: runs/random05/fable-review-01/payload.txt
SHA256:ded0889fcaa2cb82a3f4fd9d5dc46f737bdf50cf7a9e466bb66f9114f9661d0f
Session:27a4316e-b79d-46cf-86b4-41b0f558938a (not initialized).
If that payload is approved, the prepared command is:
`python3 random05/review/run_review.py --execute --output runs/random05/fable-review-01`
The payload describes an older baseline. Keep subsequent source transfers within
whatever exact approval the user provides. Continue local work independently.
