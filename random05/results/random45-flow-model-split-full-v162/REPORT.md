# Crowded traffic-assignment model comparison

All ten full archived runs pass strict entry/memory checks and independent replay. Both unchanged controls match their prior runs in all six trace fields. None of the changed traffic models improves the selected score.

Source162 (`4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad`), frozen manifest a0d752ea. Only the co-flow cost exponent or coefficient changes; planner seed, output street contrast, scheduler, fixed search work and explicit instance tricks remain fixed. Full1,000/2,000step horizons,32workers on16bound physical EPYC9354cores,1s entries and32GB guard.

| Instance | Model | Tasks | Max entry ms |
|---|---|---:|---:|
| RANDOM-04 | control | 2782 | 789.177 |
| RANDOM-04 | power2 | 2696 | 860.688 |
| RANDOM-04 | power4 | 2558 | 802.420 |
| RANDOM-04 | alpha0p5 | 2714 | 796.755 |
| RANDOM-04 | alpha2 | 2319 | 777.893 |
| RANDOM-05 | control | 4254 | 801.607 |
| RANDOM-05 | power2 | 4059 | 818.832 |
| RANDOM-05 | power4 | 4030 | 820.077 |
| RANDOM-05 | alpha0p5 | 4007 | 919.774 |
| RANDOM-05 | alpha2 | 3518 | 830.352 |

Default exponent3/coefficient1 remains selected. This negative result does not negate the prior RANDOM-05 gain from increasing field-solver iterations20to80; that independent change remains selected and qualified. The development frontiers stay2782/4254, +5.02%/+34.11% versus matched max(NMS,KK)=2649/3172.

[Independent audit](audit.json), [exact controls](control-equivalence.json).
