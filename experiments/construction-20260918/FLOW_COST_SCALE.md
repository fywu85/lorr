# Gentler learned traffic costs

The strength-1 flow experiment's smallest penalty doubles one directed edge's
cost from 1 to 2. Its seed-0/1 gains do not survive seed 2. V31 adds optional
`CGAR_FLOW_COST_SCALE=1|2|4|8` (default 1), allowing a strength-1 penalty of
25% or 12.5% by expressing a physical slot as 4 or 8 integer units. This tests
penalty magnitude separately from the refresh-frequency experiment. It is not
an established throughput improvement; defaults are unchanged.

The complete scoring basis changes together: forward and rotation distances,
wait slots, goal-arrival reward, operation tie term, fallback distances and
remaining task-chain terms. Paid forward surcharges subtract the matching
base unit. A neutral rescaling multiplies every candidate score by the same
factor. Nondefault scales require temporal planning, enabled learned flow and
physical turn cost 1. The oriented oracle uses exact uniform-cost BFS for a
neutral field and weighted Dial search for a nonuniform field. Repeated neutral
publications preserve tables and LRU effects.

## Validation

Build **8898626**, binary SHA-256
`72c6bb3d43fb05b8663af97098ee5ed1872eca50af30fce2e0214cae885c2ed5`,
passes the complete regression suite. The archived patch independently
reconstructs all requested source hashes. New checks include:

- 4,096 independent forward-Dijkstra state/goal distance comparisons.
- 241,536 candidate paid-progress bounds and 120,768 exact neutral score checks.
- 11,520 identical robot decisions across scales 1/2/4/8 with cache pressure,
  four-worker preparation, warm starts and five neutral field publications.
- 4,800 protected serial/four-worker decisions under active scaled, refreshed
  flow, plus primary, pocket and capacity invariants.

GRID screen **8898627**, four physical EPYC 9354 cores, 8 GiB allocation,
200 steps, one-second complete decisions:

| Profile | Valid | Max entry (s) | Wall (s) | Peak process RSS (bytes) |
|---|---:|---:|---:|---:|
| margin50 scale1 | yes | 0.778883082 | 43.403 | 3,678,842,880 |
| margin50 scale4 | yes | 0.778295070 | 43.346 | 3,678,728,192 |
| margin50 scale8 | yes | 0.776930540 | 43.501 | 3,678,613,504 |
| neutral scale8 | yes | 0.769802058 | 40.825 | 4,750,356,480 |

Scale1 exactly matches the previous frozen-flow screen's paths, schedules,
events and task data. Neutral scale8 exactly matches the prior no-flow 4M
screen. These short runs establish feasibility and control equivalence, not
throughput rankings. Full **8898628**, analysis **8898629**, compares margin50
scales 1/4/8 at seeds 0 and 2, all 5,000 steps. It follows the refresh matrix in
a separate experiment queue from the kernel/preparation validation. Each case
has four reserved physical cores; the matrix runs two cases concurrently under
a 24 GiB allocation, with the same per-process 32,000,000,000-byte requirement.
No full scale result exists at submission.

[Build evidence](build-provenance/v31/validation.json),
[screen evidence](results/flow-cost-scale-screen-v31/),
[full configuration](flow-cost-scale-full-variants.json).
