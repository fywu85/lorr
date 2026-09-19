# Generic routing and shared-plan repair experiments

These settings are optional and regression-tested. Compact storage now has a
complete, identical 5,000-step control trajectory with 11.883 GB RSS. Corrected
turn cost 2 loses throughput and cost 4 fails a later deadline; regional full
results remain pending. The established 107,413-task three-seed recommendation
and defaults are unchanged. Six-seed confirmation remains required.

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

## Compact tables and prefetch (builds v12/v13)

`CGAR_TURN_COMPACT=1` stores complete distance tables in 16 bits when all finite
values fit below 65,535. Unreachable values use a separate reserved sentinel.
Any table with larger finite values stays 32-bit; an actual 70,000-cell corridor
regression verifies the fallback. Logical cache capacity, admission and eviction
still use the original 32-bit size, so storage changes cannot alter decisions.

The full regression suite passes. Twenty-five matched 50-step cases in five
comparison groups have identical paths, assignments, events and tasks: unit-cost
controls, serial/parallel prefetch, turn costs 2/4, and one/two regional rounds.
[Exact comparisons](results/compact-prefetch-equivalence.json).
Both compact control repeats use approximately 2.21 GiB RSS at step 50, versus
2.67 GiB for wide tables, with similar timing. [Screen evidence](results/compact-cold-v13/).
A generic BFS diameter bound proves all orientation tables for this workload and
turn costs 1–4 fit 16 bits; no policy setting is derived from map structure.
[Memory-planning bound](compact-distance-bound.json). The complete v14 control now measures 11.883 GB RSS versus 16.208 GB with
wide storage, with every full trajectory field identical.
[Full equivalence](results/compact-full-equivalence.json).

`CGAR_TURN_PREFETCH_THREADS` is a disabled-by-default speculative experiment.
Workers build complete tables privately and join before normal serial demand
admits results in the original cache order. Unused results are discarded. Its
short-run trajectories match, but repeated cold tests show no timing benefit:
four-thread total wall time is approximately 19.0 seconds versus 18.7 for the
control; one-thread speculation takes 20.1 seconds. Turn cost 8 and the larger
regional setting still fail explicitly. Leave this option off.
[Positive and failed screen records](results/prefetch-cold-v12/).

## Review fixes and diagnostic screen (build v14)

The explicitly approved Fable review is complete. P1 reproduced two weighted
fallback corner stalls; v14 uses unit-action scoring for robots without an exact
orientation table. All 64 rotated exact/fallback progress cases pass. P2 adds
non-vacuous regional tests and contribution counters: 18/24 dense fixtures improve,
and 4,800 production robot actions match between one and four regional threads.
The unit-cost control and one/two-round regional profiles retain every path,
schedule, event and task in the earlier 50-step warehouse prefixes.
[Exact checks](results/review-fix-unit-prefix-equivalence.json).

The corrected 200-step screen uses four reserved physical cores and 8 GiB total
on EPYC 9354. Every accepted case has 200 complete entry samples and zero errors.

| Profile | Maximum entry time | Screen outcome |
|---|---:|---|
| Compact 50k control | 0.507414954 s | Pass |
| Turn cost 2 | 0.588724246 s | Pass |
| Turn cost 4 | 0.778583220 s | Pass |
| Turn cost 8 | — | Explicit timeout, exit 124 |
| 50k global + 1 regional round | 0.676871476 s | Pass |
| 50k global + 2 regional rounds | 0.893240484 s | Pass |
| 25k global + 2 regional rounds | 0.727221937 s | Pass |

At step 200, one round retained all four regions, improving the same complete
plan's score from 2,965,995 to 3,042,519 in 56 ms. Two rounds retained 6/8 regions;
the 25k-global variant retained 7/8. Score improvement is not a throughput claim.
The timing reduction after the fallback fix does not establish Fable's proposed
causal explanation: bucket-Dijkstra work can also vary with edge weights.
[All screen records, including failure](results/review-screen-v14/).

## Full comparison queue

- The full 4M-candidate matrix and motion analysis are complete: 109,244 for
  4M alone, 111,118 for 4M plus direct-cost/weight-5/global-64 scheduling, and
  107,457 for the paired control. All are valid single-seed exploratory results.
  [Full evidence](results/work-full-v8/).
- Job **8898387**, `runs/cgar-temporal-review-full-v14-20260919`, runs the six
  passing profiles above over all 5,000 steps. **Two concurrent instances use
  four disjoint physical cores each, with 24 GiB total reserved memory.**
  The frozen source is v14. Cost 8 is excluded solely because it timed out.
- One-core analysis job **8898389** waits for the whole matrix. The first-pair
  analysis is already complete: the compact control has identical full
  trajectories, 11.883 GB RSS and a 0.506-second maximum entry time. Turn cost 2
  reaches 100,323, while cost 4 fails at timestep 902 in temporal repair.
  Regional full outcomes are still pending.
- Every full profile uses seed 0, the EPYC 9354 host allowlist and explicit
  CPU-model verification, a one-second complete-entry limit, and measured process
  RSS below 32,000,000,000 bytes. Six-seed confirmation remains outstanding.
- Original never-started sequential jobs were replaced by compact v13, which was
  then held before starting when Fable identified P1. Its source was not altered;
  the held matrix and analysis were canceled only after v14 was submitted.
  [Replacement history](compact-parallel-plan.json),
  [v13 frozen job archive](results/replaced-compact-full-v13/).

[Approved review](fable-regions/review.md), [local assessment](fable-regions/assessment.md),
[CLI metadata](fable-regions/metadata.json). The review covers frozen v11;
prefetch and compact storage are outside its scope. The earlier
[Fable review](fable/review.md) remains available.
