# RANDOM-04 task transitions and loaded work

This diagnosis freezes PILOT2795, its same-seed offcontrol2773 and matchedNMS2649. Later maxima are in the live dashboard. All three traces were independently replayed in their linked source/run audits. Every700,000robot-step trace is partitioned into approach, loaded work and a censored tail.

| Same2,568completed tasks | PILOT2795 | NMS2649 |
|---|---:|---:|
| Previous finish/start to pickup steps | 108600 | 37945 |
| Physical pickup approach bound | 16236 | 11140 |
| Delay until the chosen order was released | 80963 | 17087 |
| Loaded steps | 420016 | 404002 |
| Relaxed physical loaded bound | 133115 | 133584 |
| Loaded steps above the bound | 286901 | 270418 |
| Pickup-heading remaining-chain excess | 2253 | 2722 |

The transition interval includes waiting for admission and for the chosen order to be revealed. Its relaxed bound is the maximum of physical approach and release delay, not their sum. The much larger total transition interval is therefore not evidence of proportionally longer travel. Completion cohorts also select different prior jobs, robots and traffic histories. No amount in this table is a recoverable-throughput estimate.

On the common cohort, PILOT still uses16,014more loaded steps thanNMS despite a slightly smaller physical bound. This supports investigating traffic execution rather than attributing all remaining room to matching. It does not prove a causal intervention or a performance ceiling.

| Step | PILOT2795 | Offcontrol2773 | NMS2649 |
|---|---:|---:|---:|
| 100 | 222 | 236 | 318 |
| 500 | 1401 | 1389 | 1426 |
| 700 | 1898 | 1895 | 1902 |
| 800 | 2159 | 2127 | 2148 |
| 900 | 2431 | 2419 | 2417 |
| 1000 | 2795 | 2773 | 2649 |

The2795run leads NMS by14at900steps and146at1000;132of its final lead appears in the last100steps. The task-boundary accounting establishes timing of the lead, not its cause. The loaded/transition changes from the whole-chain matcher are modest on the2,690common tasks with its off control.

[Full accounting](audit.json), [input traces](manifest.json), [distance provenance](distance-reference.json), [PILOT replay/source audit](../random04-record2783-split-full-v166/audit.json), [NMS audit](../random-nms-kk-baselines-v1/audit.json).
