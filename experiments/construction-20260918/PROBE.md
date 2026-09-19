# Construction and search have separate, measurable effects

Each row is a 100-step fixed-goal diagnostic with 10,000 robots, identical initial
positions/goals for all methods, uniform action costs, no task reassignment, and
no new goals after arrival. These are not full lifelong throughput scores.

| Starting state | CGAR cache | NMS construction, 1 worker | NMS search, 1 worker | NMS construction, 32 workers | NMS search, 32 workers |
|---|---:|---:|---:|---:|---:|
| CGAR seed 0, step 100 | 2,269 | 2,954 | 4,106 | 3,078 | 4,116 |
| CGAR seed 0, step 4,000 | 1,502 | 2,407 | 3,217 | 2,386 | 3,281 |
| KittyKnight, step 4,000 | 3,383 | 3,626 | 4,305 | 3,714 | 4,350 |

Numbers are goal arrivals. Every run completes without invalid actions or late
decisions. Every NMS worker completes its entire initial robot order without a
clock cut: 100/100 constructions per single-worker run and 3,200/3,200 per
32-worker run. The archived implementation still consults its clock; this result
does not make it a fixed-work policy.

Construction alone improves on the CGAR control by 30.2%, 60.3%, and 7.2% in the
three states. Single-worker search adds 39.0%, 33.7%, and 18.7% over construction.
The 32-worker search adds just 0.24%, 1.99%, and 1.05% over single-worker search.
This points first to construction semantics and useful search work, with limited
portfolio benefit in these particular probes. It does not establish that the
same worker scaling holds over full lifelong runs or with fixed search counts.

The single-worker modes have one planning helper, one solver worker, and verified
affinity to one reserved logical CPU on a physical core. Preprocessing uses the
reserved 32-logical/16-physical-core allocation. Parallel modes use that full
allocation throughout. NMS construction averages 101–173 ms per decision with
one worker; most of that is candidate preparation. Search uses about 953–955 ms.
CGAR averages 64–67 ms. Node models and complete measurements are retained in
`results/probe/summary.json` and each case's allocation file.

NMS peaks at about 30.64 GiB with one worker and 31.19–31.24 GiB with 32 workers.
These exceed the project's **32,000,000,000-byte** target. NMS also precomputes
its full oriented-distance matrix, while the fresh CGAR instances populate
their bounded caches during the probe. These resource and warmup differences
remain limitations of the comparison. The CGAR implementation keeps its bounded
distance cache rather than importing the full NMS matrix.

The native constructor uses candidates with at most three non-trailing-wait
actions, embedded in five-step collision reservations. Search later changes
candidate depth between three and five. Native construction reached displacement
depth 1,295 in the late CGAR probe, far beyond the earlier port's 63-level cap.
Its work counter counts recursive displacements, not every candidate examined.
Waiting ordinary plans remain movable; non-worsening joint-score acceptance and
randomized displacement of moving plans are also significant semantics.

# Direct kernel comparison

The new generic temporal kernel is compared against the frozen, instrumented
NMS implementation using native candidate scores/order and independently
computed geometry. All 51,600 geometry cases match. Both construction and
256-step repair match every selected operation across 96 dense 80-robot fixtures
per mode: **15,360 robot decisions** total. Complete cases have no construction
budget exhaustion. This validates the search kernel for these inputs, not the
whole CGAR integration or a liveness theorem.

The first repair comparison incorrectly treated normal search branch-budget
rollback as incomplete initial construction. The corrected comparison checks
construction exhaustion before repair. Both attempts and their sources remain
under `results/conformance/`; the planner kernel did not change between them.

Additional CGAR regressions check a 1,500-robot displacement chain, bounded-work
rollback, propagated deadlines, 48 randomized collision/protection cases, a
higher-index primary completing its live intent, and existing pocket/recovery
and capacity fixtures with the temporal layer enabled. The default remains
unchanged; full 5,000-step results decide whether any profile should be adopted.
