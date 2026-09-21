# CGAR competition progress

Updated 2026-09-21 04:28 UTC. We are improving CGAR across all ten LoRR2024
instances, using both general mechanisms and explicitly enabled instance tricks.
NMS is the target. Throughput is primary; fairness is measured as a secondary
metric. The independent RANDOM-05 agent's code, jobs and held-out inputs are
outside this campaign's edit scope.

All selected scores below are complete runs with enforced **1000 ms** entry
limits and **32 decimal GB** process memory limits. Timeouts fail the run;
the solver does not return reduced-quality partial work to meet a deadline.
WAREHOUSE and SORTATION use eight physical cores; other rows use four.

| Instance | CGAR tasks | Published NMS | Difference | Seed | Max step (ms) |
|---|---:|---:|---:|---:|---:|
| WAREHOUSE | 155,173 | 154,795 | +0.24% | 0 | 944.77 |
| SORTATION | 150,333 | 152,714 | -1.56% | 0 | 972.92 |
| CITY-01 | 8,427 | 8,420 | +0.08% | 2 | 753.90 |
| CITY-02 | 16,315 | 16,787 | -2.81% | 0 | 821.72 |
| GAME | 21,742 | 23,274 | -6.58% | 0 | 835.91 |
| RANDOM-01 | 621 | 639 | -2.82% | 0 | 201.28 |
| RANDOM-02 | 1,188 | 1,221 | -2.70% | 0 | 183.45 |
| RANDOM-03 | 1,902 | 2,334 | -18.51% | 0 | 298.41 |
| RANDOM-04 | 1,645 | 2,547 | -35.41% | 2 | 139.87 |
| RANDOM-05 | 2,806 | 3,050 | -8.00% | 0 | 343.55 |

Every current selected profile is a **TRICK**, enabled through `--trick INSTANCE`.
These are best individual runs under different declared settings, not the mean
of a universal solver. Published NMS scores are historical targets, not matched
local runs. WAREHOUSE has exclusive-host qualification and independent action
replay. The other rows use shared GRID hosts with disjoint bound physical cores,
no CPU quota, simulator validation and complete movement/waiting accounting.

[Timestamped best history and source commits](experiments/allmaps-20260920/BEST_HISTORY.md),
[exact configurations and evidence for all ten rows](experiments/allmaps-20260920/selected-full-results.json),
[published targets](experiments/allmaps-20260920/TARGETS.md),
[Warehouse history](WAREHOUSE_PROGRESS.md).

The largest new improvement is GAME fleet selection: **15,574 → 21,742 (+39.6%)**.
A fixed subset of 2,750 robots receives tasks, while all 6,500 remain in CGAR's
movement system. No held or started task is dropped. This adapts a Kitty Knight
idea and changes fairness: 3,750 robots receive no new assignments. The unchanged
KK exemption asset scored slightly worse than uniform selection on seed0.
The independent mask/assignment audit passed. The gain appears late; the same
variant was worse after 1,000 steps, so full 5,000-step runs are required.
[Measured fleet ablation](experiments/allmaps-20260920/results/game-fleet-full-v1/summary.md).

General repair and scheduling changes also help on some maps. RANDOM-05 improves
from 2,684 to **2,806** with four regional repair rounds, each with a fixed
8M-candidate limit. Allowing a finite second reassignment before pickup separately
improves CITY-02 from 16,169 to **16,315**. Faster matching with a four-change task
budget improves RANDOM-02 from 1,160 to **1,188**. Started-task ownership, recovery,
primary protection and the robot cooldown remain enforced. The same rematching
experiments lose on RANDOM-04. These mechanisms contain no map lookup, but their
measured combinations include explicitly flagged fields/ranks.
[Full comparisons and unchanged controls](experiments/allmaps-20260920/latest-selected-20260921-0425.json).

CITY-01 reaches 8,427 on seed2, seven above published NMS. Four tested seeds remain
close to that threshold; this is not evidence of a substantial margin. SORTATION's
strict profile scores 150,333; the older 150,353 development run exceeded one second
and remains separately recorded. RANDOM-04 is the largest relative gap.

The current CGAR family retains its primary/recovery framework and the documented
NMS-derived temporal PIBT component. Guidance assets, scheduler adaptations and
fleet selection do not replace it with either team's complete solver. Default-off
controls are checked against prior whole-trajectory fingerprints.

In progress: GAME fleet-size tuning and paired seeds, RANDOM-05 combinations of
regional work and finite rematching, RANDOM-04 region partitions/rounds, and a
RANDOM-02 cadence ablation. [Next experiments](experiments/allmaps-20260920/TRICK_ROADMAP.md).
Historical narrative is preserved in the [earlier progress snapshot](experiments/allmaps-20260920/CGAR_PROGRESS_20260921_0404.md).
