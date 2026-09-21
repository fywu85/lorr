# Generalization evaluation

Requested 2026-09-21 while the 4,000-task RANDOM-05 campaign continues.
Use full archived competition horizons, 16 physical EPYC9354 cores / 32 workers,
1s entry, 30s preprocessing and a 32 decimal GB process guard. Input files,
executables, environments, allocations and all failed attempts are frozen.

The first portable pipeline configuration freezes source233f5bf / build-v69,
planner seed5 and the selected B18 search settings. Disable generated directional
map guidance, field flips and known-horizon triage. No new density tuning is
performed. RANDOM-05 without tricks is an additional control for interpreting
the four other densities. This evaluates the existing reactive pipeline;
it does not introduce the colleague's LNS planner for sparse traffic.

NMS is the unchanged shipped 32-thread binary, SHA256
`2dfe6f35b3ae532d075aa487de88ec8d436a07ddef97e4dec24e94b550820539`.
Its own map-specific tuning remains part of the shipped competitor. All nine
requested non-RANDOM05 inputs receive fresh local reference attempts. Existing
RANDOM05 NMS32=3,172 remains the original reference on that input.

The five large inputs expose a capacity limit in the current frozen pipeline:
its oriented all-pairs float table alone needs95–189GB, before other memory.
[Capacity calculation](results/generalization-20260921/capacity-inventory.json).
These are analytically unsupported under32GB; no throughput score is invented.
The user explicitly chose to evaluate the frozen solver and report these limits;
no memory-bounded port is part of this evaluation.
Earlier local NMS large-map attempts also exceeded32GB; the new reference
attempts retain the cap and will report failure if the shipped code cannot fit.

Results and independent trajectory checks will be appended as runs finish.


A separately labeled transfer batch will keep the RANDOM05 field seed15,
one flip with seed5, all search settings and planner seed5 fixed across RANDOM01–04.
Only the declared horizon changes to the case's published run length. No tuning
uses the new outputs. The input geometry is the same819-cell random map; this
is a test across robot densities, not across unseen map layouts. Each case
requires its own `--trick RANDOM-01`/02/03/04 flag. The driver validates32x32
geometry and the matching100/200/400/700/800-agent count. RANDOM05 retains its
existing flag behavior. This small interface extension changes no search logic.

The five new NMS large-map attempts all exit124 during preprocessing at about30s. Their observed peak RSS before termination is12.1–16.1GB. The immediate observed failure is the preprocessing deadline, not a demonstrated memory-cap failure in these attempts. No full-horizon NMS score is available from them. Earlier higher-memory runs remain historical context only.
