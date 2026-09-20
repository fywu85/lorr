# RANDOM-05 next experiments

The campaign targets the colleague's reported roughly 27–28% advantage over
matched NMS combined-track runs. Their private synthetic inputs and code are
unavailable. We use archived RANDOM-05: 800 robots and 2,000 steps.

Read [RANDOM05_PROGRESS.md](../RANDOM05_PROGRESS.md) for the current frontier and
all measured successes and failures. Exact configurations are in `best.json`
(overall) and `best-four-cores.json`. These include explicit guidance and
known-horizon tricks; bare defaults do not reproduce the frontier.

## Runs to collect

Directories below are under `runs/random05/`.

| GRID job | Directory | Question |
|---|---|---|
| 8899454 | nms4-repeats-full-v3 | How much does the timed NMS reference vary? |
| 8899456 | large-search-full-v15 | Do K4096, smaller mutations, or noise400 help? |
| 8899457 | compound-search-full-v16 | Do scheduler changes or a policy mixture help at 32 workers? |
| 8899458 | blocked-cycle-thresholds-full-v16 | Which cycle threshold helps on four cores? |

Collect each final `summary.json`, validate all 2,000 steps, and retain exact
source, binary and input hashes. Promote valid maxima while keeping them separate
from averages. Repeat promising configurations on additional planner seeds.
NMS logs are buffered and may remain empty during a healthy 32-minute run.

Cycle preparation improved some maxima but slightly lowered the six-seed mean.
The larger-search results also vary with the cycle policy. Keep both policies
available; investigate mixtures without assuming they will improve throughput.
The broad remaining gap calls for better decisions as well as parameter search.

## Reproducible source versions

- Build v15: commit `134faa8`, best overall 3,231 tasks.
- Build v16: commit `6aed8ba`, best on four cores 3,127 tasks; adds cycle portfolios.
- Build v17: commit `e55f951`, tests discounted progress and load-weighted turn
  costs. Tested settings did not improve throughput; both default to off.

All three builds passed regression tests. Compact evidence is in `results/`;
frozen sources, binaries and full traces are under ignored `runs/random05/`.
The progress audit checks frontier timestamps, scores, validity, binary hashes,
and the exact planner sources at the linked commits.

## Allocation notes

Use `tools/grid.py` to freeze and submit builds or benchmarks. A 32-worker case
uses 16 physical cores with two SMT threads, matching the archived competition
CPU topology. Four-worker cases use four physical cores, one thread each.

The runner reserves both SMT slots per physical core, requests available cores
through GRID's `m_topology_inuse`, and verifies the resulting affinity. This
dynamic resource requires `qsub -w n`; static verification incorrectly rejects it.
Memory reservation is at least 32 GiB total. The per-process address-space cap is
32 GB or the inherited hard limit, whichever is lower. Earlier NMS repeat attempts
failed before solver startup because the wrapper attempted to raise that limit;
`nms4-repeats-full-v3` is the corrected submission.
