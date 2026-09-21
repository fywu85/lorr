# General RANDOM-04 face-cycle qualification

Audited 2026-09-21T23:48:27.685553+00:00. All ten full1000-step cases pass independent replay and strict limits. Source307d5cc9/build159; the original selecting off/on pair used production-equivalent face logic in source158.

| Planner seed | Face cycles off | Maximum length8 | Difference |
|---|---:|---:|---:|
| 1 | 1595 | 1616 | +21 |
| 0 | 1589 | 1565 | -24 |
| 2 | 1574 | 1565 | -9 |
| 3 | 1551 | 1577 | +26 |
| 5 | 1577 | 1555 | -22 |

Total7878 versus7886 (**-0.101%**), two gains/three losses. The selected1616 repeats all six full trace fields, but **there is no positive paired mean gain**. It remains a selected general best, not a robust replacement for the auction-only profile. Against matchedmax2649 it is -39.00%; selected trick2782 remains +5.02%.

Combining face8 with two failed-wait retry passes scores1584 onseed1, below both1616 and the1595 no-face control. No fresh qualification. All full settings and resource/waiting checks are archived. The four failures in the original32-case face screen remain preserved there; this follow-up has no failures.
