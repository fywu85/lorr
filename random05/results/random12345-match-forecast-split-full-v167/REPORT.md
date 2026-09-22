# Forecast assignment: complete all-five comparison

All20attempts are complete and independently audited:19valid, one preserved
strict timeout. All eight disabled controls match six complete trace fields.
Sampled profiling confirms the forecast mechanism was exercised. It improves
selectedRANDOM-01at radius8; broad improvement is not established.

| Profile | Disabled | Radius4 | Outcome |
|---|---:|---:|---|
| General RANDOM-01 | 727 | 721 | Lower |
| General RANDOM-02 | 1397 | Timeout | Step2/1016.576ms |
| General RANDOM-03 | 1634 | 1628 | Lower |
| General RANDOM-04 | 1616 | 1510 | Lower |
| General RANDOM-05 | 2314 | 2255 | Lower |
| Selected RANDOM-03 | 2646 | 2625 | Lower |

SelectedRANDOM-01seeds1/0, radius0/2/4/8:739/737/736/739 and736/733/737/740.
Onlyradius8gains in this initial paired total; its subsequent fixed five-seed
qualification gains1.0914%and reaches745. This is an interaction with the selected
profile; the general01follow-up also finds no gain at8/16/24. No freshinput used.

The general02failed step had one forecastrow and102,678,872repair expansions;
these observations do not isolate the timeout's cause. Keep the original failure.
[Audit](audit.json), [eight exact controls](control-equivalence.json),
[01qualification](../random01-record740-split-full-v167/REPORT.md).
