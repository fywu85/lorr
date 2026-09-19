# Static reverse-neighbor cache

The orientation oracle previously recalculated a grid neighbor, including
coordinate division and obstacle indexing, for every popped state in every
complete goal table. Build v18 precomputes only the static reverse-forward
predecessor once per orientation state. Each traversal still applies the same
goal-specific pocket restriction, costs, predecessor order, cache admission and
deadline checks. This adds four integers per free cell (617,376 bytes for the
benchmark graph). No route or task policy changes.

The full regression suite passes, including independent weighted forward
Dijkstra comparisons and pocket/protected-robot episodes. The archived patch
reconstructs every tested source hash. A separate microbenchmark builds 128
complete tables per mode on one reserved EPYC 9354 physical core. Six paired
repetitions alternate old/new order. Table checksums match in every repetition.

| Table metric | Baseline median seconds | v18 median seconds | Reduction |
|---|---:|---:|---:|
| Unit action costs | 0.159296 | 0.129143 | 18.9% |
| Turn cost 4 | 0.262443 | 0.193217 | 26.4% |
| Directed synthetic forward costs | 0.386753 | 0.301027 | 22.2% |

These numbers time only complete table construction, including compact storage.
They are not whole-planner speedups or throughput gains. Synthetic costs exercise
the weighted code; they are not a planner guidance policy. The first local job
setup failed because Python 3.7 lacks `shlex.join`; the corrected setup and its
predecessor failure are retained. [Timing evidence](results/oracle-timing-v18/).

The 200-step integration screen passes for the unit control, learned flow, and
regional repair. Every path, schedule, event and task exactly matches the prior
implementation in all three cases. [Screen evidence](results/oracle-screen-v18/),
[trajectory checks](results/oracle-trajectory-equivalence.json). Full planner
timing and full-horizon trajectory preservation still need verification before
attributing an end-to-end benefit.

The v19 full control now completes all 5,000 steps at 107,457 tasks with the exact
v14/v16 trajectory fingerprint. See `results/oracle-full-equivalence.json`.
This validates policy preservation over the full horizon, not an end-to-end
runtime speedup across different matrices.
