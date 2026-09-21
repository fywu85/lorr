# CGAR competition progress

Updated 2026-09-21T05:19:34.033105+00:00. Throughput is primary; fairness is a secondary metric.
We are developing general CGAR mechanisms and explicit instance-specific tricks
across all ten LoRR2024 instances. The independent RANDOM-05 solver remains untouched.

All selected scores below are complete runs with enforced **1000 ms** entry
limits and **32 decimal GB** process memory limits. A timeout fails the run.
WAREHOUSE and SORTATION use eight physical cores; other rows use four.

| Instance | CGAR tasks | Published NMS | Difference | Seed | Max step (ms) |
|---|---:|---:|---:|---:|---:|
| WAREHOUSE | 155,173 | 154,795 | +0.24% | 0 | 944.77 |
| SORTATION | 150,333 | 152,714 | -1.56% | 0 | 972.92 |
| CITY-01 | 8,427 | 8,420 | +0.08% | 2 | 753.90 |
| CITY-02 | 16,315 | 16,787 | -2.81% | 0 | 821.72 |
| GAME | 23,977 | 23,274 | +3.02% | 0 | 842.09 |
| RANDOM-01 | 621 | 639 | -2.82% | 0 | 201.28 |
| RANDOM-02 | 1,188 | 1,221 | -2.70% | 0 | 183.45 |
| RANDOM-03 | 1,902 | 2,334 | -18.51% | 0 | 298.41 |
| RANDOM-04 | 1,947 | 2,547 | -23.56% | 0 | 245.83 |
| RANDOM-05 | 2,877 | 3,050 | -5.67% | 0 | 628.73 |

Every selected profile is a **TRICK**, enabled through `--trick INSTANCE`.
These are best individual runs under declared settings, not the mean of a universal
solver. Published NMS scores are historical targets, not matched local comparisons.
WAREHOUSE has exclusive-host qualification and independent action replay. Other rows
use shared GRID hosts with disjoint bound physical cores, no CPU quota, simulator
validation and complete movement/waiting accounting.

[Timestamped best history and source commits](experiments/allmaps-20260920/BEST_HISTORY.md),
[exact settings and evidence](experiments/allmaps-20260920/selected-full-results.json),
[published targets](experiments/allmaps-20260920/TARGETS.md),
[Warehouse history](WAREHOUSE_PROGRESS.md).

GAME reaches **23,977**, 3.02% above the published NMS target. Uniform task admission
to 3,250 robots beats the earlier 2,750-robot profile (21,742) on seed0. All 6,500
robots remain in the movement system; held/started tasks are preserved. This adapts
a Kitty Knight idea and deliberately changes fairness: half the robots get no new
tasks. The independent mask/assignment audit passes. Full 5,000-step runs are needed:
the earlier 2,750-robot variant was worse at 1,000 steps but improved the full run.
[Fleet-size comparison](experiments/allmaps-20260920/results/game-fleet-higher-full-v1/summary.md).

RANDOM-04 reaches **1,947** using KK forward guidance, turn price2 and four repair
rounds. Against turn6, seeds0/2/4 change1801/1803/1935 to1947/1904/1796: two gains
and one loss, +1.95% in the mean. Higher annealing temperature helps the turn6
profile but does not improve turn2. The single-seed record is not a robust large gain.
[Turn comparison](experiments/allmaps-20260920/results/random04-kk-turn-tuning-full-v1/summary.md).

RANDOM-05 gains consistently from eight repair rounds: seeds0/2/4 change
2806/2809/2718 to2877/2865/2836 (+2.94% in the mean). CITY-02 gains from allowing
a finite second pre-pickup reassignment; RANDOM-02 gains from matching every step.
Faster matching loses on RANDOM-01, so its621 profile stays selected.

The geometric known-horizon trick gives small, mixed gains on dense RANDOM.
One unchanged RANDOM-05 control failed at step0; an identical retry completed2806
with the exact prior trajectory. The original failure is preserved and its cause
remains unresolved. No partial result is promoted.

The current solver retains CGAR primary/recovery mechanisms and its documented
NMS-derived temporal PIBT component. Guidance, scheduler and fleet adaptations do
not replace it with either team's complete solver. Generic regional peak retention
is now implemented behind a default-off option: finish fixed work, then choose
the best visited complete plan. Full regressions pass; throughput A/B is next.

Running: finer GAME fleet sizes and seed replication, SORTATION dispatch,
RANDOM-05 round counts. [Next experiments](experiments/allmaps-20260920/TRICK_ROADMAP.md).
The earlier narrative is preserved in the [progress snapshot](experiments/allmaps-20260920/CGAR_PROGRESS_20260921_0404.md).
