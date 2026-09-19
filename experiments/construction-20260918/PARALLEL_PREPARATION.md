# Parallel candidate preparation, v28

`CGAR_TEMPORAL_PREP_THREADS` defaults to 1 and accepts 1–32 workers. It
parallelizes per-robot candidate scoring before the unchanged temporal search.
The planner checks that CPU affinity can accommodate the requested workers.
Each worker scores a fixed contiguous robot range and owns its heuristic scratch.
All candidates are complete before search begins. Every worker joins, including
after exceptions; a deadline failure rejects the decision. No work limit or
random draw is selected by elapsed time.

Turn-cache lookup promotes LRU recency. With multiple workers, those lookups
therefore occur first in the original robot order. Workers only read the resolved
tables, and no table is built or evicted during scoring. The one-worker path
retains its original lookup order. Candidate vectors also reserve their fixed
maximum capacity once; all score arithmetic, sorting and search RNG are unchanged.

Build **8898592** passes all regressions. The new cache-pressure fixture checks
3,840 identical one-/four-thread actions with 105 turn-table builds and matching
cache statistics. Four injected worker failures propagate only after all workers
join. Existing protected, flow, guide, refinement, warm-start and regional
fixtures also compare one-/four-thread preparation. The archived patch reproduces
every requested source hash.

Screen **8898601** runs all four profiles sequentially on the same four reserved
EPYC 9354 physical cores, with 8 GiB reserved memory and one-second decisions.
All four pass 200 steps without errors or timeouts. The complete paths, schedules,
events and tasks are identical within each pair and to frozen v27.

| Search policy | Preparation workers | Wall seconds | Max entry seconds | Peak RSS bytes |
|---|---:|---:|---:|---:|
| Global 4M candidates | 1 | 47.550 | 0.807436320 | 4679512064 |
| Global 4M candidates | 4 | 40.924 | 0.777120212 | 4750016512 |
| Global 25k + two regional rounds | 1 | 85.279 | 0.688213878 | 4793262080 |
| Global 25k + two regional rounds | 4 | 78.209 | 0.656879124 | 4824559616 |

These single short pairs show 13.9% and 8.3% lower wall time. They do not establish
full-run timing or a throughput improvement. The one sampled global candidate
phase at step 200 falls from 0.055178 to 0.021928 seconds; do not treat one phase
sample as a run-wide average. Regional search already uses four workers in both
regional cases; its preparation is the changed thread count.

Full **8898606**, analysis **8898607**, completes both pairs, with two
independent four-core instances and 24 GiB total reservation. All cases use
5,000 steps and the same one-second/32,000,000,000-byte process limits.

[Source provenance](build-provenance/v28/),
[screen evidence](results/parallel-prepare-screen-v28/),
[exact equivalence](results/parallel-prepare-screen-equivalence.json).


## Full results

Both policies preserve every trajectory, schedule, event and task field across
one/four preparation workers and against frozen v27. All four runs pass 5,000
complete decisions and the memory limit on exclusive research52.

| Policy | Tasks (both) | One-worker wall (s) | Four-worker wall (s) | Reduction | Max entry one/four (s) |
|---|---:|---:|---:|---:|---:|
| Global 4M | 109,244 | 1,101.855 | 970.680 | 11.90% | 0.803148 / 0.770470 |
| Global25k + two regional rounds | 111,997 | 1,371.622 | 1,264.205 | 7.83% | 0.674428 / 0.645945 |

RSS stays below 11.917 GB. These are single full timing pairs on disjoint physical
cores of the same reserved host. They support an implementation efficiency gain;
repeated timing distributions have not been measured. Throughput is unchanged.

[Full results](results/parallel-prepare-full-v28/),
[independent equivalence and timing pairs](results/parallel-prepare-full-v28/equivalence.json).
