# Full warehouse results

The NMS-style temporal kernel raises the three-seed mean from **43,406.7** to
**107,413.3 completed tasks** (+147.5%). These are complete 5,000-step runs with
10,000 robots. The independent six-seed cache control averages 43,176.2.
The new temporal profile has three-seed confirmation so far, not six.

The local KittyKnight target remains **152,981**. It used 16 physical / 32 logical
CPUs and 38,857,801,728 bytes peak RSS, above this project's 32,000,000,000-byte
target. CGAR uses one physical core for the main profile. These are local
reproductions, not official scores or equal-resource comparisons. The NMS local
run has one timeout and remains a diagnostic reference only.

## Construction and fixed-count search

| Profile | Seed 0 | Seed 1 | Seed 2 | Mean |
|---|---:|---:|---:|---:|
| Previous 8 GiB orientation cache | 43,446 | 44,174 | 42,600 | 43,406.7 |
| Temporal construction, goal-distance order | 38,962 | 35,202 | 39,264 | 37,809.3 |
| Temporal construction, ticket order | 51,506 | 43,473 | 38,908 | 44,629.0 |
| 1,000 repair attempts | 50,043 | 45,116 | 46,866 | 47,341.7 |
| 10,000 repair attempts | 89,093 | 89,518 | 89,006 | 89,205.7 |
| 50,000 repair attempts | 104,573 | 104,788 | 104,523 | 104,628.0 |
| 50,000 attempts, equal robot weights | 107,457 | 107,579 | 107,204 | 107,413.3 |

The first matrix is retained in [full-v1](results/full-v1/); equal-weight results
are in [full-v3](results/full-v3/). All rows above finish without planner errors,
schedule errors, or reported timeouts. The 150,000-attempt variant **fails in all
three seeds** with explicit exit 124, so it has no accepted score.

Initial construction alone does not close the gap. The goal-distance constructor
looks better after 1,000 steps but worse after 5,000. Equal-weight 50k repair
maintains about 21,900–22,000 completions in the final 1,000 steps. This is why
short throughput screens cannot replace the full run.

## Priority, implementation efficiency, and parallelism

The [v2 matrix](results/full-v2/) tests remaining-task-chain and ticket priorities
at 10k repair attempts. Their means are 87,991.3 and 86,773.3, below goal-distance
order's 89,205.7. With equal weights and 50k attempts, chain and ticket orders
average 107,280.7 and 106,462.3, again below goal-distance order.

Memoized candidate heuristic lookups preserve every trajectory in the three
matched 10k pairs. Do not infer a runtime speedup from v1 versus v2: those
matrices ran on EPYC 9354 and EPYC 7532 respectively.

| Seed-0 portfolio | Tasks | Wall seconds |
|---|---:|---:|
| 4 workers × 12,500 attempts, serial execution | 93,328 | 1,101.994 |
| Same work on 4 reserved physical cores | 93,328 | 674.337 |
| 4 workers × 50,000 attempts, ranked weights | 105,007 | 960.066 |
| 4 workers × 50,000 attempts, equal weights | 108,047 | 968.867 |

The first two rows have identical paths, schedules, events and tasks, not just
equal totals. Their speedup is 1.63×. A larger portfolio adds little quality:
108,047 is only 0.55% above the single-worker equal-weight seed-0 score.
[Full parallel results](results/full-v4/), [equivalence checks](results/equivalence.json).

## Timing and memory

All full temporal profiles above peak around 15.0–15.2 GiB of total process RSS.
The [precise-timing seed-0 repeat](results/timing-v5-seed0/) records:

| Profile | Tasks | Maximum complete entry time | Wall seconds | Peak RSS bytes |
|---|---:|---:|---:|---:|
| Cache control | 43,446 | 0.410239394 s | 140.604 | 15,289,643,008 |
| Equal-weight 50k | 107,457 | 0.505097663 s | 886.848 | 16,211,615,744 |

Both have all 5,000 entry samples and identical trajectories to their preceding
versions. The repeat uses warning-level simulator logging. It does not establish
six-seed timing coverage; remaining replications were stopped after this complete
pair to prioritize new candidates. The original matrix specification and
[interruption record](results/timing-v5-seed0/interruption.json) are retained.

Legacy `plannerTimes` includes the simulator's logging after the entry thread
returns. Some v1 rows contain a shared approximately 29-second pause there.
Fable's [timestamp audit](fable/review.md) places the pause after planner completion
and does not establish its system cause. These observations remain in the raw
and compact records. The new `entryComputeTimes` is sampled at the entry's final
absolute-deadline check, before that logging; the outer series is also retained.

The initial 12-concurrent-run replication request was rejected by automatic
approval review because it reserved 288 GiB in aggregate. Its accepted replacement
reserved **24 GiB total**, with one benchmark process at a time. Subsequent full
matrices use that smaller allocation. Parallelism inside one process remains
available and has been verified separately.

GRID has heterogeneous CPU generations. A 6M-candidate cold-start screen nearly
exhausted the deadline on an EPYC 9354, while even 4M failed on an EPYC 7532.
Host selection and an explicit CPU-model check now make hardware-controlled
comparisons possible. Neither CPU affinity nor a successful short screen proves
deadline feasibility on every CPU model or every later state.

## What still separates CGAR from the local leader

An exact unweighted shortest-path audit of the completed trajectories finds:

| Seed-0 profile | Mean completed chain length | Loaded steps / shortest-path cell | Actual empty robot-steps |
|---|---:|---:|---:|
| Previous CGAR cache | 230.30 | 3.3165 | 11,207,254 |
| CGAR equal-weight 50k | 240.14 | 1.4888 | 9,497,666 |
| KittyKnight | 233.84 | 1.0799 | 10,049,537 |

Completed task chains differ by only 2.7%, while loaded movement takes 37.9%
more steps per shortest-path cell. Both allocate roughly one fifth of the
50 million robot-steps to pre-pickup work. Coordination and route efficiency
therefore remain the larger opportunity.

Assignment is still different: CGAR's initial pickup distance averages 56.3 cells,
versus KittyKnight's 19.8. KittyKnight subsequently reassigns 88,043 tasks, reducing
the last-owner distance to 5.6. That last distance alone understates real empty
travel; the audit counts all ownership intervals, including abandoned assignments.
It also reports unfinished pickups and loaded work. These observations do not
isolate causality between scheduling, traffic and the leaders' supplied guidance.

The [travel audit](results/travel/) replays positions and computes complete BFS distances on the simulator's
free-cell graph. A hand-computed reassignment fixture checks timing and distance
accounting. CGAR's empty movement counts also exactly match the existing
independent movement analysis. The first two setup attempts failed (compiler
path, then an endpoint-symbol parsing mistake); corrected outputs are used here.

## Further experiments

`CGAR_GLOBAL_SAMPLES=64` adds a bounded global shortlist even when local candidates
exist. The independent oldest-task admission remains active. Combined with direct
cost ranking and pickup weight 5, the first full seed-0 run completes **109,676**
tasks, with maximum entry time **0.560785880 s**. The matched scheduler matrix is
still running; this is exploratory, not a six-seed promotion.

`CGAR_TEMPORAL_CANDIDATE_LIMIT` is a separate experiment in deterministic work
allocation. Construction always completes, and the work threshold is checked
only between complete repair attempts. The attempt cap also applies; elapsed
time never selects a successful stopping point. Initial 50-step runs are used
only to reject infeasible deadlines, never as throughput scores. No work-limit
profile has yet earned a full-run performance claim.

The tested checkpoint is the optional equal-weight 50k profile. Source patches
for every build are in [build-provenance](build-provenance/). Defaults remain
unchanged. Further work must retain the 1-second failure contract, 32 GB process
target, generic policies, and full-run validation.
