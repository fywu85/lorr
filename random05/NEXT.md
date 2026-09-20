# RANDOM-05 continuation

The active campaign targets the colleague's roughly27–28% advantage over matched
NMS in the combined track. Their private instances/code are unavailable. We use
archived RANDOM-05,800 robots,2,000 steps. The goal is not complete.

## Current evidence

- Best: **3,363**, source `2ead4f4`, four physical EPYC9354 cores, strict1s limit.
  Mean333ms/max435ms. Configuration is `best.json` and `best-four-cores.json`.
- NMS4 repeats:2,902/2,903/2,914. Use the strongest2,914 as the conservative
  matched reference: current best **+15.4%**. NMS32 reference remains3,172.
- The best uses generated field15, final directional penalty2.4, K1024,
  exact/guided matching with keep0.5, and known-horizon triage1.5. All map tricks
  require `--trick RANDOM-05`. It is currently a single planner seed.
- Preceding contrast1.6 scored3,299/3,185/3,284/3,214/3,061 over seeds0–4,
  mean3,208.6. Do not transfer that mean to the new contrast2.4 configuration.
- Best32-worker record is still3,299 (`best-32-workers.json`). Its trajectory
  reproduced exactly on four cores. Best without horizon triage is2,914.
- `RANDOM05_PROGRESS.md` records the complete frontier and negative results.
  `tools/audit_progress.py` verifies source/binary hashes, result timestamps,
  validity, CPU metadata, matching NMS input hashes and every reported gain.

## Runs to collect

All directories are under `runs/random05/`.

| Jobs | Directory | Purpose |
|---|---|---|
| 8899503–8899505 | direct-recursion-split-full-v22 | Exact trajectory/latency after replacing std::function recursion; K2560. K3072 already failed its deadline. |
| 8899508–8899512 | contrast-seeds-split-full-v22 | Replicate contrast2.4 across five planner seeds using the equivalent direct-recursion implementation. |
| 8899518–8899524 | plain-score-split-full-v23 | Separate lane guidance from evaluation using exact unit-action distances; K128 blends0/0.25/0.5/0.75/1 and K1024 blends0.25/0.5. |

These are one-job-per-case batches. Use `tools/split_grid.py collect --output
<batch>` to collect completed cases. Child directories each contain a standard
single-case benchmark spec/allocation/summary. Copy those into compact `results/`
when recording evidence; use a child's summary for a new frontier link so the
audit finds its matching spec/allocation. Raw traces/binaries remain ignored.

The earlier monolithic batches are all collected: `candidate-sort-full-v20`,
`contrast-validation-full-v20`, `generations-full-v21`, and field searches. The
first has one invalid K3072 timeout, which must never count as a score.

## Findings guiding the next step

1. More compute alone is weak: K2048 gives3,262 at contrast1.6 and3,343 at2.4,
   below corresponding K1024 bests3,299/3,363.
2. Multi-generation search helps K128 (G2=3,182 vs3,073), but loses at K1024
   (G2=3,181,G4=3,234 vs3,299). It remains off. More optimization may be exposing
   a mismatch between the rollout objective and completed tasks; this is a
   hypothesis, not an established cause.
3. The new plain-score test preserves the traffic field for policy decisions
   but blends its evaluation with exact movement/turn distances over the full
   remaining task chain. It adds a second graph and task-cost cache. Defaults
   preserve the old behavior. Unit tests verify equivalence when metrics agree
   and deterministic worker counts; full performance tests are pending.
4. At K128/contrast2.4, static load multipliers0/0.25/0.5/0.75/1/1.5 give
   3,013/3,101/3,190/3,199/3,217/3,221. Values1 and1.5 merit K1024 validation,
   but their small screen gain is not a new frontier.
5. Source-level efficiency: shared sorting reduced K1024 mean500→378ms;
   five-element insertion sorting reduced it further to335ms, with exact full
   trajectory identity. Direct recursion is under validation. A possible next
   low-risk optimization is caching the active task-cost table pointer per
   robot at each simulated step, avoiding repeated stage/chain checks in every
   candidate cost lookup. Require exact trajectory identity.

## Source versions

- v18 `79d0e79`: shared candidate/priority sorting.
- v19 `b3bf5d3`: independent final guidance contrast.
- v20 `2ead4f4`: stable insertion sorting; current3,363 frontier.
- v21 `9090996`: optional fixed-budget generations, off in the frontier.
- v22 `3228b9c`: direct recursive lambda for PIBT, same policy.
- v23 `b3329d1`: optional unit-action evaluation blend, off in the frontier.

All builds passed their regression tests. Frozen source and binary hashes are
in each `runs/random05/build-v*/spec.json` and `completion.json`.

## Resources and shared workspace

Use GRID. Four-worker validation gets four physical cores, one thread each.
Competition32-worker topology is16 physical cores with two SMT threads each.
The user permits shared hosts; reserve disjoint physical cores and report load.

For matching reference hardware use `--hosts 'research33*|research39*'` and
`--cpu-model 'AMD EPYC 9354 32-Core Processor'`. The runner selects matching queue
instances, checks CPU model/affinity, reserves both SMT slots per physical core,
and retains a32GB address-space ceiling (or the lower inherited hard limit).
Mixed-duration batches should use `split_grid.py` so finished cases release cores.

A separate Warehouse session edits `cgar/` and `experiments/construction-20260918/`.
Only stage/commit RANDOM05_PROGRESS.md and random05/. The shared main branch can
advance with its commits. The public fywu85/lorr remote is authorized for push.
Use the existing GitHub credential helper if the stale VSCode askpass socket fails.
