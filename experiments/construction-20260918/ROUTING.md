# Generic routing and shared-plan repair experiments

These optional settings are implemented and regression-tested, but have no
completed full-run performance result yet. They do not change the established
107,413-task three-seed recommendation or its defaults. Full warehouse runs,
then six-seed confirmation of the strongest candidate, remain required.

## Weighted turns (build v9)

`CGAR_TURN_COST` raises the guidance cost of a rotation while retaining unit
forward cost. The cache uses complete reverse shortest paths: the original BFS
for cost 1, and bounded-integer Dijkstra for larger costs. The extra cost is also
charged to rotations in candidate paths and to virtual rotations during terminal
waits. No map identity, supplied weights, aisle classification, or lane template
is used. The certified unweighted spatial potential stays separate.

An independent forward heap Dijkstra checks 3,600 state/goal/cost combinations.
Another 101,704 path checks verify that weighted turns cannot claim unpaid
progress over five action slots. Protected-primary, pocket, capacity and parallel
replay checks pass with turn cost 4. Unit cost preserves the original scoring
formula; full trajectory confirmation is still queued.

## Regional repair (builds v10/v11)

Each complete round starts from the same feasible five-step plan for all workers.
Rectangular partitions depend only on dimensions and the configured part count.
A robot may change only if its entire current path, including its start, lies in
one region. New candidates must stay inside that region. Crossing paths and all
CGAR-protected robots remain fixed and visible in every worker's reservations.

Workers use prescribed attempts and predetermined seeds. They join before any
merge, and an exception fails the entire decision. The merged selected paths
rebuild all cell/edge reservations and receive the adapter's independent final
collision check. Boundaries shift between complete rounds. This uses multiple
cores to improve different parts of one plan, rather than selecting one result
from a larger independent portfolio.

Twenty-four dense fixtures compare serial and four-thread repair, checking 1,152
robot results, RNG states, work counters, all five collision layers, fixed paths,
and complete-plan score monotonicity. All 24 fixtures independently contain
boundary-crossing initial paths. A timeout is injected inside worker work and
propagates. The production adapter also passes primary/pocket/capacity fixtures.
These are finite safety checks, not a general liveness proof. Build v11 strengthens
boundary coverage reporting and preserves initial construction diagnostics;
v10 and v11 use the same search policy.

## Deadline-only screens

Each row runs 50 warehouse steps, seed 0, with a one-second complete-entry limit.
Turn tests reserve one physical core; region tests reserve four physical cores.
Each screen has 4 GiB total allocation on an EPYC 9354. Prefix task totals are
retained in raw summaries but are not used to rank performance.

| Setting | Maximum successful entry time | Outcome |
|---|---:|---|
| Turn cost 2 | 0.645951082 s | 50 steps complete |
| Turn cost 4 | 0.840716733 s | 50 steps complete |
| Turn cost 8 | — | Explicit timeout, exit 124 |
| 4 regions, 1 round × 25k attempts | 0.673920251 s | 50 steps complete |
| 4 regions, 2 rounds × 25k attempts | 0.895433925 s | 50 steps complete |
| 4 regions, 2 rounds × 50k attempts | — | Explicit timeout, exit 124 |
| 4 regions, 4 rounds × 25k attempts | — | Explicit timeout, exit 124 |

The larger settings are rejected from the full comparison. Passing a cold screen
does not guarantee a later state meets its deadline. Failed runs receive no
accepted score. [Turn evidence](results/turn-cold-v9/),
[region evidence](results/regions-cold-v11/),
[exact frozen builds](build-provenance/).

## Full comparison queue

- Job 8898338, `runs/cgar-temporal-turn-full-v9-20260918`: costs 2/4 and the
  unit-cost 50k control; one core, 24 GiB total, held after work-limit job 8898333.
- Job 8898343, `runs/cgar-temporal-regions-full-v11-20260918`: one/two 25k regional
  rounds and the same control; four cores, 24 GiB total, held after job 8898338.
- Full runs use 5,000 steps, the EPYC 9354 allowlist and an explicit CPU-model
  check, exact entry timing, and a measured process limit of 32,000,000,000 bytes.
  Reserved full-run allocations do not overlap. Compact analysis uses one core.

A second Fable review was prepared for these frozen changes. Automatic approval
review rejected the CLI source transfer twice, including after a metadata check
confirmed public repository visibility. Specific payload approval is pending;
no second CLI review process started. Local validation and benchmarks continue.
The completed earlier [Fable review](fable/review.md) remains available.
