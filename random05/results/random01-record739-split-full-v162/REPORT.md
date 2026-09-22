# RANDOM-01 observed-progress correction qualification

All five full600step runs pass independent replay and strict checks.
The739repeat matches all six complete trace fields. Source4fb9498e/build162.

| Planner seed | Progress0 baseline | Progress0.25/span32 | Difference |
|---|---:|---:|---:|
| 0 | 732 | 736 | +4 |
| 1 | 735 | 739 | +4 |
| 2 | 725 | 726 | +1 |
| 3 | 729 | 732 | +3 |
| 4 | 730 | 732 | +2 |

Totals3665versus3651, **+0.3835%**, all five positive.
Largest entry118.656ms; peakRSS390.930MB.
This is a modest paired development-input gain. Selectingseed1is included;
fresh01V1still qualifies only the earlier729profile.

[Audit](audit.json), [exact repeat](control-equivalence.json), [paired evidence](paired-seeds.json).
