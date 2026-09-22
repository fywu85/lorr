# RANDOM-04 exact matching compression and coupled work prices

All nine full runs pass independent replay, source/input/resource checks, strict one-second entries and 32 GB. Four full-trace comparisons establish exact repetition of 2,799 and score-preserving optional-column compression for the declared controls.

| Compressed setting: length / idle price | Seed 0 | Seed 1 | Total |
|---|---:|---:|---:|
| 0.25 / 32 | 2776 | 2795 | 5571 |
| 0.5 / 32 | 2799 | 2771 | 5570 |
| 0.5 / 64 | 2756 | 2768 | 5524 |
| 1 / 128 | 2779 | 2750 | 5529 |

The selected 2,799 remains the valid maximum, but coefficient 0.5 does not improve the two-seed total over 0.25. The coupled higher idle prices also lose. The new compressed candidate peaks at 848.301 / 859.270 ms on seeds 0 / 1. The exact original noncompressed 2,799 repeat peaks at 902.458 ms; its earlier record peaked at 984.795 ms. The original noncompressed seed-1 failure at 1,046.301 ms remains preserved. Successful compressed runs identify a different runtime configuration and do not erase that failure.

| Case | Step 0 ms | Largest later step ms | Mean ms |
|---|---:|---:|---:|
| trick-random-04-chain-runtime-exact2799-seed0 | 902.458 | 587.522 | 469.232 |
| trick-random-04-chain-runtime-control-seed0 | 855.101 | 750.333 | 526.303 |
| trick-random-04-chain-runtime-current-seed0 | 848.301 | 622.005 | 467.765 |
| trick-random-04-chain-runtime-half-scaled-seed0 | 876.193 | 585.905 | 475.739 |
| trick-random-04-chain-runtime-full-scaled-seed0 | 933.615 | 623.537 | 475.579 |
| trick-random-04-chain-runtime-control-seed1 | 763.114 | 578.561 | 466.900 |
| trick-random-04-chain-runtime-current-seed1 | 859.270 | 665.370 | 483.686 |
| trick-random-04-chain-runtime-half-scaled-seed1 | 855.773 | 578.510 | 474.354 |
| trick-random-04-chain-runtime-full-scaled-seed1 | 948.209 | 622.997 | 475.741 |

The three additional paired seeds (2 / 3 / 4) are complete: three of six attempts fail startup. Across completed pairs 0 / 1 / 3, the candidate totals 8,331 versus 8,347; full five-pair qualification fails. See the [final extension report](../random04-compact-qualification-split-full-v166/REPORT.md). No fresh task/start input was used.

[Independent audit](audit.json), [four exact comparisons](control-equivalence.json), [startup timing](startup-latency.json), [qualification protocol](../../experiments/random04-compact-qualification-v166-protocol.json).
