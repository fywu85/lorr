# RANDOM-03 physical-deadline qualification at 2,621

Audited 2026-09-22T00:22:23.176196+00:00. All nine full 800-step runs pass independent replay and strict limits. Source4fb9498e/build162. Exact seed3 repetition matches all six complete trace fields. The selecting off control comes from the audited identical replacement after the original allocation refusal; that refusal is preserved.

| Planner seed | Gate off | Gate on | Difference |
|---|---:|---:|---:|
| 0 | 2614 | 2604 | -10 |
| 1 | 2571 | 2595 | +24 |
| 2 | 2597 | 2600 | +3 |
| 3 | 2620 | 2621 | +1 |
| 4 | 2597 | 2609 | +12 |

Total **13,029 versus 12,999 (+0.231%)**, four gains and one loss. The improvement is modest and not universal. Selected2,621 is **+11.11% versus matched max(NMS,KK)=2,359**. The largest entry across this qualification is 831.136ms.

No new fresh inputs were generated. Fresh03V2 still qualifies the previous2,620 configuration only; its +8.852% two-team aggregate remains below the10% milestone. These planner-seed checks do not change that conclusion.
