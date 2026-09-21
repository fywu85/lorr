# Full-run bottlenecks across all five RANDOM instances

Updated 2026-09-21T23:03:30.746077+00:00. These are the current audited selected PILOT runs and the strongest retained valid local result from max(NMS, Kitty Knight) on each archived input. Published scores and fresh task streams are separate.

| Instance | PILOT | Local max team | Reference | Lead | PILOT extra loaded steps / all robot steps |
|---|---:|---|---:|---:|---:|
| RANDOM-01 | 729 | KK | 692 | +5.35% | 5.74% |
| RANDOM-02 | 1,408 | KK | 1,256 | +12.10% | 11.47% |
| RANDOM-03 | 2,620 | NMS | 2,359 | +11.06% | 32.96% |
| RANDOM-04 | 2,782 | NMS | 2,649 | +5.02% | 46.40% |
| RANDOM-05 | 4,242 | NMS | 3,172 | +33.73% | 59.56% |

The independent unit-action shortest paths include pickup orientation and all later waypoints, while relaxing collisions and repeated-waypoint timing. The extra loaded steps include unavoidable interaction delays. They are neither recoverable-throughput estimates nor upper bounds on achievable score. RANDOM-01 has much less observed loaded-route slack than RANDOM-03–05, supporting different priorities across densities.

| Instance | Common completed tasks | PILOT loaded steps | Reference loaded steps | PILOT relaxed minimum | Reference relaxed minimum |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 | 684 | 44,676 | 45,718 | 41,495 | 41,594 |
| RANDOM-02 | 1,248 | 86,696 | 94,408 | 74,498 | 74,722 |
| RANDOM-03 | 2,327 | 228,336 | 242,754 | 136,926 | 137,121 |
| RANDOM-04 | 2,587 | 425,939 | 411,436 | 135,008 | 135,417 |
| RANDOM-05 | 3,140 | 840,657 | 1,021,673 | 168,763 | 169,166 |

| Instance | First 100: PILOT / max team | Before final 100: PILOT / max team | Final 100: PILOT / max team |
|---|---:|---:|---:|
| RANDOM-01 | 105 / 106 | 610 / 582 | 119 / 110 |
| RANDOM-02 | 215 / 195 | 1151 / 1050 | 257 / 206 |
| RANDOM-03 | 310 / 324 | 2268 / 2094 | 352 / 265 |
| RANDOM-04 | 242 / 318 | 2417 / 2417 | 365 / 232 |
| RANDOM-05 | 155 / 189 | 3931 / 3041 | 311 / 131 |

RANDOM-04 is tied at2,417 completed tasks after900steps; the selected133-task lead appears in the final100steps (365 versus232). The first100steps instead trail242 versus318. Task release times, chosen chains and traffic diverge, so this is a phase diagnosis, not proof that a horizon rule causes the final advantage. RANDOM-05 trails only the initial phase and then builds a broad lead. RANDOM-01 is already near its relaxed loaded-route bound; matching and arrival/departure decisions merit focused work alongside LNS.

Next bounded experiment: briefly raise the task-admission cap at startup on the two crowded cases, then restore the current cap while protecting already-opened tasks. Earlier source148 startup tests reduced caps; the opposite direction is untested. This is explicitly a `--trick RANDOM-04/05` preset, uses only visible tasks, and cannot be credited before full replay and strict timing checks. Sparse/medium prefix-search comparisons continue separately.

All ten source trajectories were independently replayed before this read-only analysis. Existing distance-table hash and source provenance are preserved. [Oriented bounds and common-task cohorts](oriented.json), [100-step phase counts](phases.json), [input trace manifest](manifest.json), [distance generation](distance-generation.json).
