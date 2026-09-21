# CGAR competition progress

Updated 2026-09-21T05:54:40.459053+00:00. We are developing general CGAR improvements and
explicit instance tricks across all ten LoRR2024 instances. Throughput is primary;
fairness is reported separately. The independent RANDOM-05 solver is untouched.

All selected scores below are complete runs with enforced **1,000 ms** entry limits
and **32 decimal GB** memory limits. A timeout fails the run. WAREHOUSE and
SORTATION use eight physical cores; the other selected rows use four.

| Instance | CGAR tasks | Published NMS | Difference | Seed | Max step (ms) |
|---|---:|---:|---:|---:|---:|
| WAREHOUSE | 155,173 | 154,795 | +0.24% | 0 | 944.77 |
| SORTATION | 150,894 | 152,714 | -1.19% | 0 | 993.90 |
| CITY-01 | 8,427 | 8,420 | +0.08% | 2 | 753.90 |
| CITY-02 | 16,315 | 16,787 | -2.81% | 0 | 821.72 |
| GAME | 23,977 | 23,274 | +3.02% | 0 | 842.09 |
| RANDOM-01 | 635 | 639 | -0.63% | 2 | 194.20 |
| RANDOM-02 | 1,197 | 1,221 | -1.97% | 4 | 183.38 |
| RANDOM-03 | 1,902 | 2,334 | -18.51% | 0 | 298.41 |
| RANDOM-04 | 1,999 | 2,547 | -21.52% | 0 | 238.42 |
| RANDOM-05 | 2,915 | 3,050 | -4.43% | 0 | 895.64 |

Every selected profile is a **TRICK**, enabled through `--trick INSTANCE`. These
are best individual runs under declared settings, not the mean of one universal
configuration. Published NMS scores are historical targets, not matched local runs.
WAREHOUSE has exclusive-host qualification and independent action replay. The other
rows use shared GRID hosts with disjoint bound physical cores, no CPU quota,
simulator validation and complete movement/waiting accounting.

[Timestamped best history and source commits](experiments/allmaps-20260920/BEST_HISTORY.md),
[exact configurations and evidence](experiments/allmaps-20260920/selected-full-results.json),
[all-ten record checks](experiments/allmaps-20260920/selected-results-checks-20260921-0550.json),
[published targets](experiments/allmaps-20260920/TARGETS.md).

GAME reaches **23,977 / 23,917 / 23,888** on seeds 0/2/4, all above published NMS.
A fixed subset of 3,250 robots receives new tasks while all 6,500 remain in CGAR
movement. This adapts a Kitty Knight idea and deliberately changes fairness: half
the robots receive no new assignments. Held and started tasks are preserved.
The independent fleet-mask and assignment audit passes. Nearby fleet sizes lost.
[Three-seed evidence](experiments/allmaps-20260920/game-fleet/uniform3250-three-seed-summary.json).

RANDOM-04 reaches **1,999** with KK forward guidance, turn price 2, higher regional
search temperature and peak retention. Retention is a general, default-off option:
complete the prescribed search work, then choose the best complete plan visited.
Timeout remains failure. It produced the best individual hot run, but reduced the
three-seed mean against the hot control without retention. At the colder setting it
lost substantially; RANDOM-03 also lost four tasks. Extra construction candidates
did not improve the RANDOM-04 record. This is an optional combination, not a new
universal default. [Measured interaction](experiments/allmaps-20260920/regional-search/random04-three-seed-summary.json).

RANDOM-05 reaches **2,915** with twelve repair rounds and no peak retention. Eight
rounds improved all three tested seeds over four rounds; twelve rounds is currently
a single-seed result. Eight rounds plus retention separately scored 2,898. A wider
regional partition on eight physical cores is queued to test useful work per second.

RANDOM-02's scheduling change improves all three tested seeds and reaches **1,197**.
CITY-02's finite second reassignment also improves all three seeds; its best remains
**16,315**. RANDOM-01 reaches **635** on seed2 using the earlier profile without
known-horizon admission. The horizon candidate has small, mixed three-seed gains.
Started-task ownership and CGAR primary/recovery safeguards remain enforced.

SORTATION reaches **150,894** with matching across 64 groups. Its 993.90 ms maximum
leaves little timing margin; reduced-work comparisons are running. A separate
dispatch variant timed out at step 0 and has no accepted score. An earlier RANDOM-05
control also timed out, then passed an identical retry with the exact prior
trajectory. Both failures are preserved; their causes are unresolved.

The full regression suite passes, including analytic peak restoration, unchanged
disabled RNG trajectories, timeout-after-peak failure, and serial/parallel action
validation. The frozen 37 source/test files match commit `90df94f`; the solver
retains CGAR primary/recovery mechanisms and its documented NMS-derived temporal
PIBT component. Guidance, scheduling and fleet adaptations do not replace it with
either competitor's complete solver.

Running: GAME horizon and dispatch tricks, CITY-02 horizon admission, SORTATION
runtime comparisons, RANDOM-05 diversity/retention/parallel repair, and RANDOM-01
horizon replication. [Next experiments](experiments/allmaps-20260920/TRICK_ROADMAP.md).
The earlier narrative remains in the [progress snapshot](experiments/allmaps-20260920/CGAR_PROGRESS_20260921_0404.md).
