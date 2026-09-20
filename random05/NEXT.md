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
| 8899468 | shared-sort-full-v18 | Collect the final old/new K1024 comparison; new build already validated3,299 on four cores. |
| 8899474 | field-contrast-full-v19 | Does changing directional contrast or turn cost improve the fixed field? |
| 8899475 | field-search-validation-full-v18 | Validate four alternative fields at K1024. |
| pending | candidate-sort-full-v20 | Check stable insertion sorting, K2048/3072, and four new planner seeds. |

Collect each final `summary.json`, validate all2,000 steps, and retain exact
source, binary and input hashes. Promote valid maxima while keeping them separate
from averages. The first two hardware-restricted jobs have an explicit allocation
request amendment: queue instances replace the ineffective HOST expression.
The CPU-model guard remains active. Copy that amendment into compact evidence.

NMS four-core repeats completed:2,902 and2,914 versus2,903 initially. Our3,299
trajectory is identical on four cores and32 workers; its four-core mean/max
latency is378/486ms. Compare conservatively against2,914, the strongest repeat.

The additional32-field K128 screen did not beat existing field15. Higher-K
validation remains useful because field rankings have changed with the search
budget. Avoid interpreting a selected best seed as a demonstrated mean gain.

## Reproducible source versions

- Build v15: commit `134faa8`, preceding32-worker best3,231 tasks.
- Build v16: commit `6aed8ba`, best overall 3,299 tasks and four-core best 3,127; adds cycle portfolios.
- Build v17: commit `e55f951`, tests discounted progress and load-weighted turn
  costs. Tested settings did not improve throughput; both default to off.

- Build v18: commit `79d0e79`, shared sorting;3,299 on four cores and exact trajectory reproduction.
- Build v19: commit `b3bf5d3`, independent final guidance contrast.
- Build v20: pending commit, stable insertion sort for five-element candidates.

Completed builds passed regression tests. Compact evidence is in `results/`;
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

The shared-sort full comparison is still awaiting the slower original K1024
case. Its completed K64 pair is exactly equal in trajectory and25.8% faster.
