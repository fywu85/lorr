# RANDOM-01 stronger forecast assignment prices

All 9 full 600-step runs pass independent replay and strict source/input/resource/timing checks. Every declared control matches all six trace fields. These changes do not improve either the selected or general solver.

| Future weight | Selected seed 4 | Selected seed 0 | Selected total | General |
|---|---:|---:|---:|---:|
| 1 | 745 | 740 | 1485 | 726 |
| 2 | 727 | 730 | 1457 | 708 |
| 4 | 730 | 730 | 1460 | 702 |

The general solver without forecast rows remains **727**; its radius8/weight1 control gives726. Keep the selected radius8/max32/weight1 configuration and **745** record. Weight0 leaves the hypothetical rows present but removes their priced future-travel contribution; it is a separate ablation from disabling forecasting.

The selected profiles retain their explicit known-horizon trick. General uses no guidance or horizon. No fresh task/start or unseen-map input was used. Higher or lower weights are negative development results, not evidence of an inherent performance ceiling.

[Independent audit](audit.json), [exact controls](control-equivalence.json).
