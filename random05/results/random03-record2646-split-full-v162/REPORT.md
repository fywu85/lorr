# RANDOM-03 observed-progress correction qualification

All five full800step runs pass independent replay and strict limits.
The2646repeat matches all six complete trace fields. Source4fb9498e/build162.

| Planner seed | Progress0 baseline | Progress0.25/span32 | Difference |
|---|---:|---:|---:|
| 0 | 2604 | 2611 | +7 |
| 1 | 2595 | 2606 | +11 |
| 2 | 2600 | 2611 | +11 |
| 3 | 2621 | 2646 | +25 |
| 4 | 2609 | 2610 | +1 |

Totals13084versus13029, **+0.4221%**, all five positive.
The candidate mean is2616.8. Largest entry730.523ms; peakRSS482.013MB.
Selectingseed3is included; this is a modest paired development-input gain.
Earlier freshV2qualifies2620only and remains+8.852%above fresh matched max(NMS,KK).

[Full audit](audit.json), [exact repeat](control-equivalence.json), [paired evidence](paired-seeds.json).
