# RANDOM-05 continuation

Active goal: approach the colleague's roughly 27–28% advantage over matched NMS
in the combined track. We use archived RANDOM-05, 800 robots, 2,000 steps. The
private synthetic instances and code are unavailable. The goal is not complete.

## Verified evidence

- Four-core best: **3,379**, planner seed3, source `3228b9c`; mean326ms/max430ms.
  NMS repeats2,902/2,903/2,914; use strongest2,914: **+16.0%**.
- Same configuration across seeds0–4:3,374/3,231/3,365/3,379/3,350, mean3,339.8.
  These are planner seeds on one fixed instance, not independent task instances.
- Best32-worker result:3,379, exactly the same trajectory as four-core seed3.
  Source32b333a, sixteen physical cores with SMT, mean105ms/max235ms. Matched
  NMS32=3,172, so the competition-allocation lead is6.5%. Active-row validation now preserves all
  control actions/schedules/events and averages309.6ms vs325.6ms on four cores.
- All recent matched runs use EPYC9354, strict1s deadlines, and a32GB ceiling.
- `best.json`, `best-four-cores.json`, `best-32-workers.json` freeze the producing
  configuration and link compact evidence. Map tricks require `--trick RANDOM-05`.
- Best config: flow field15, output contrast2.4, K1024/depth8/noise200,
  dispersion0.8/local5/equal acceptance, wait0.5/turn0.6, exact guided matching
  with keep0.5/length0.25, horizon2000/triage1.5, independent per-step RNG.
- Best without horizon triage remains2,914 (older configuration).

## Latest batches

Use `tools/split_grid.py collect --output runs/random05/<batch>`. Each child
has a standard benchmark spec/allocation/summary; copy compact metadata into
`results/`. Link child summaries in the frontier so provenance audit can resolve
its spec. Raw traces and binaries remain ignored. Inspect qstat before deciding
whether a missing summary is still running or an allocation failure.

| Jobs | Batch | Purpose |
|---|---|---|
| 8899588–8899594 | startup-length-split-full-v30 | Seed3,32 workers. Control3,379; global length0.5/1/2 =3,342/3,294/3,139; initial1 for250 steps=3,330. Initial2 for250=3316, initial1 for500=3367. All complete and valid; all variants lose. |
| 8899560–8899567 | local-rotation-split-full-v26 | Control3,374; penalties0.1/0.3/1 =3,335/3,252/3,123; local25=3,294; mutation0.1/0.03 =3,310/3,148. Local100=3,294. All complete and valid; every variation loses. |
| 8899572–8899577 | regional-mutation-split-full-v27 | All complete: control3,374; radius2/4/8 at mutation0.3 =3,195/3,217/3,348; radius2/4 at mutation1 =3,224/3,222. All lose. |
| 8899579–8899580 | active-cost-split-full-v28 | Four-core seed0 control for task-cost lookup speed; seed3 on32 workers now repeats3,379 exactly; control-four repeats3,374 exactly; mean309.6ms versus325.6ms before. Complete. |

All earlier batches through fixed-scale contrast and step-RNG validation have
finished and are collected. Current full-run changes are experimental until
validation; no unmeasured option has been enabled in the best configuration.

## Findings

- Stronger output contrast1.6→2.4 improved all five paired seeds: mean3,208.6→
  3,324.8 (+3.6%). Step RNG then gives mean3,339.8 (+0.45%, four positive pairs).
- More K is weak. Step-RNG K2048=3,343 versus K1024=3,374; depth6 loses.
  K3072 exceeds1s and exits124; do not count that as a score.
- Static load1/1.5, objective blends with unweighted distances, multi-generation
  refinement and intent-component pinning all lost full K1024 comparisons.
- Fixed preferred-edge scale with counterflow penalty4.8 scores3,362 versus
  coupled-RNG control3,363. Its K128 improvement did not transfer.
- Short prefixes are weak screens: at500 steps the top third retains only7 of10
  trajectories within2% of the best, among26 competitive fixed-K full runs.
  This retrospective sample does not establish a reliable stopping rule.
- Performance optimizations preserved complete trajectories: shared sorting
  lowered mean500→378ms, insertion sorting→335ms, direct recursion→323ms.
  v28 resolves active cost rows once per policy step; full control actions,
  assignments and events match exactly, with observed mean310ms (one pair).
- See `RESEARCH.md`: inspected the 2024 winner authors' EPIBT follow-up and its
  public MIT implementation. A three-step operation kernel with bounded revisits
  and inherited valid operations is a possible structural next experiment.
  No EPIBT source has yet been incorporated. Checkout `/tmp/r05-epibt-reference`
  at `3808e832593c9fb46b3e119cbb36035cccd830e1`.

## Source versions

- v22 `3228b9c`: direct recursive PIBT; produces current score records.
- v23 `b3329d1`: optional unweighted score blend, off.
- v24 `8856336`: optional intent components, off.
- v25 `d070f2e`: fixed normalization reference, off.
- v26 `3b80801`: optional opposite-turn score penalty, off.
- v27 `3e25c56`: optional regional priority mutation, off.
- v28 `32b333a`: cache active task-cost row pointer per simulated step.
- v30 `33f358e`: startup task-length preference trick, default off. Regression
  checks phase expiry and protection of started tasks. Build passed.
- v29 `5630ba1`: same engine asv28; adds cached/uncached full toy-trajectory
  equivalence regression across task completions and replacements.

All these builds passed regression tests. Frozen sources/binary hashes are in
`runs/random05/build-v*/spec.json` and `completion.json`. Run
`python3 random05/tools/audit_progress.py` before committing new frontier rows.

## Resources and shared workspace

Use GRID with disjoint physical cores; exclusive hosts are not required. For
matching hardware use `--hosts 'research33*|research39*'` and
`--cpu-model 'AMD EPYC 9354 32-Core Processor'`. The runner checks CPU model and
binding, reserves both SMT slots per physical core, and enforces32GB address space.
32 workers can run K1024 full horizons in about3min; confirm winning candidates
on four cores before claiming a four-core result or latency.

A separate Warehouse session edits cgar/ and experiments/construction-20260918/.
Only stage/commit RANDOM05_PROGRESS.md and random05/. Shared main may advance.
Public fywu85/lorr push is authorized; do not change repository visibility.

Next measured bottleneck: in the first500 steps, ours visits3,393 task stops
versus NMS4's3,346 but completes779 tasks versus879. It completes fewer two-stop
chains (407 versus486). Test stronger task-length weighting at startup, guarded
by --trick RANDOM-05; the new option is off by default and preserves opened tasks.

A second promising structural direction is local mutation of guidance directions
around field15. Current field search mostly selected entire unrelated layouts at
lowK, which is not reliable for K1024. With soft costs, flipping a few pairs of
directional weights preserves graph connectivity and keeps every physical edge
available. Test at fullK/full horizon, retaining explicit map-trick flags. The
EPIBT operation kernel in RESEARCH.md remains the larger independent alternative.

All RANDOM-05 GRID jobs through v30 are now terminal and collected. No operation
kernel or guidance-mutation prototype has been implemented yet. Those remain
the next structural directions; do not restart completed parameter batches.
