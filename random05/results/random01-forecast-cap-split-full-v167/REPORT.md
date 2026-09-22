# RANDOM-01 forecast population limits

All eight full 600-step runs pass independent replay and strict timing/source/input/resource checks. The declared cap32 controls reproduce 745 and 740 in all six trace fields. Smaller populations do not improve throughput.

| Maximum forecast rows | Seed 4 | Seed 0 | Total |
|---|---:|---:|---:|
| 32 | 745 | 740 | 1485 |
| 8 | 737 | 737 | 1474 |
| 16 | 736 | 737 | 1473 |
| 24 | 745 | 740 | 1485 |

Cap24 reproduces both complete cap32 trajectories exactly in a separate post-result comparison. Caps8/16 lose on both seeds. Keep radius8/max32 and the selected745 record. All other matching, search and known-horizon settings were fixed; no fresh input was used.

Next hypothesis: retain the future robots' spatial coverage while reducing the weight of their later assignments relative to currently free robots. This differs from dropping forecast rows or reducing the hop radius. No benefit is assumed.

[Independent audit](audit.json), [declared exact controls](control-equivalence.json), [cap24 equivalence](observed-cap24-equivalence.json).
