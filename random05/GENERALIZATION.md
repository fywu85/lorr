# Frozen solver generalization evaluation

The frozen RANDOM-05 recipe does not establish a lead outside RANDOM-05.
On RANDOM-01, removing its map tricks gives near parity with NMS. NMS finishes
more tasks on RANDOM-02 through RANDOM-04. The five large inputs cannot fit the
frozen pipeline's distance representation within32GB; the user requested that
we report this limitation before developing a memory-bounded version.

## Full-horizon density comparison

| Instance | Robots | Steps | Same search, tricks disabled | Frozen RANDOM-05 recipe | Shipped NMS32 | Frozen recipe versus NMS |
|---|---:|---:|---:|---:|---:|---:|
| RANDOM-01 | 100 | 600 | 647 | 592 | 649 | -8.8% |
| RANDOM-02 | 200 | 600 | 1,079 | 1,122 | 1,228 | -8.6% |
| RANDOM-03 | 400 | 800 | 1,582 | 2,171 | 2,359 | -8.0% |
| RANDOM-04 | 700 | 1,000 | 1,558 | 2,456 | 2,580 | -4.8% |
| RANDOM-05 control | 800 | 2,000 | 2,226 | 3,928 | 3,172 historical matched reference | +23.8% |

One archived input and one declared planner seed per case; these are descriptive
outcomes, not a variance estimate. The four lower-density profiles were frozen
before their results were seen. All fourteen successful new trajectories pass independent movement, collision,
assignment and task-event replay checks. The transferred RANDOM-05 control
reproduces the original3,928 result in all six trajectory fields exactly.
RANDOM-05 controls are additional to the nine requested transfer cases.
Subsequent development records belong to the continuing optimization campaign
and do not replace the recipe frozen for this evaluation.

All five RANDOM cases use the same32x32/819-free-cell layout. This measures
transfer across robot densities, not across unseen layouts. No per-density
retuning or sparse-traffic LNS implementation was introduced.

## Large-map limits

| Input | Robots | Full horizon | Pipeline distance table alone | New NMS attempt |
|---|---:|---:|---:|---|
| CITY-01 | 1,500 | 3,000 | 142.82GB | 30s preprocessing timeout |
| CITY-02 | 3,000 | 3,000 | 142.82GB | 30s preprocessing timeout |
| GAME | 6,500 | 5,000 | 119.17GB | 30s preprocessing timeout |
| SORTATION | 10,000 | 5,000 | 188.84GB | 30s preprocessing timeout |
| WAREHOUSE | 10,000 | 5,000 | 95.29GB | 30s preprocessing timeout |

The pipeline allocates a float distance table over every pair of oriented free
cells: `(4 * free_cells)^2 * 4 bytes`. Other allocations are additional. These
are analytical capacity failures under the32 decimal GB process guard; no
large-map pipeline run or throughput score is claimed.
[Capacity inventory](results/generalization-20260921/capacity-inventory.json).

The five new NMS attempts exit124 at about30s, with observed peak RSS12.1–16.1GB
before termination. Their observed failure is the preprocessing deadline, not
a demonstrated memory-cap failure. No full-horizon NMS score is available from
these attempts. Earlier larger-memory or shorter-horizon runs are not substitutes.

## Frozen protocol and performance

Both solvers receive16 physical EPYC9354 cores /32 logical workers, a1s entry
limit,30s preprocessing and32GB address-space guard. GRID runs separate jobs
in parallel. Actual affinity, topology and absence of CPU quota are recorded;
hosts are shared, not exclusive. Input, executable and compiled-source hashes
are checked. The pipeline completes its fixed declared work each step; NMS
uses its shipped time-bounded search.

The no-tricks profile uses source[233f5bf](https://github.com/fywu85/lorr/commit/233f5bf),
planner seed5, B18/firstK7968/K16320/screen2/keep4/G4/E8/P8. Generated map
guidance, field flips and known-horizon triage are disabled; other RANDOM-05
search settings stay fixed. It is an ablation of this recipe, not a separately
optimized general solver.

The transfer profile retains the3,928-task RANDOM-05 recipe: field seed15,
one flip with seed5, triage mix0.5/scale1.25, same search and planner seed5.
Only the declared horizon and explicit `--trick RANDOM-01`..`RANDOM-05` label
change. Source[6b40cd5](https://github.com/fywu85/lorr/commit/6b40cd5) extends those
labels with32x32 and matching-agent-count validation. No search logic changes.
Move bias, startup weighting and finalist rescoring remain off.

NMS is the unchanged shipped32-thread binary, SHA256
`2dfe6f35b3ae532d075aa487de88ec8d436a07ddef97e4dec24e94b550820539`.
Its shipped map-specific settings stay enabled. RANDOM-05 NMS3,172 is the
existing matched archived-input reference; the other nine attempts are new.

| Instance | No-tricks mean / max entry | Frozen recipe mean / max entry | NMS mean / max entry |
|---|---:|---:|---:|
| RANDOM-01 | 60 / 74ms | 56 / 70ms | 950 / 950ms |
| RANDOM-02 | 123 / 132ms | 105 / 119ms | 951 / 952ms |
| RANDOM-03 | 244 / 269ms | 219 / 242ms | 951 / 951ms |
| RANDOM-04 | 426 / 466ms | 408 / 477ms | 951 / 952ms |
| RANDOM-05 control | 709 / 993ms | 536 / 720ms | 951 / 951ms |

The fixed recipe uses substantially less than the allowed second at lower
densities. This comparison measures its current transfer behavior, not the
best score it might reach after density-specific tuning or more search.
Pipeline peak RSS across these eight runs is163–446MB; NMS is1.69–1.73GB.

## What the traces show

Guidance and the horizon rule together help at higher densities but reduce
RANDOM-01 throughput. These runs change both settings, so they do not isolate
which setting causes each difference. On RANDOM-04, completed loaded travel
uses2.11 forward moves per shortest-path cell with the frozen recipe, versus
1.67 for NMS; on RANDOM-03 these ratios are1.42 and1.32. The solvers complete
different task sets, so this is an observational efficiency diagnostic, not a
causal decomposition of the throughput gap.

[Source/resource/replay audit](results/generalization-20260921/audit.json),
[matched historical RANDOM-05 NMS reference audit](results/generalization-20260921/reference-random05.json).
All successful runs and original failed attempts remain in the linked evidence.
