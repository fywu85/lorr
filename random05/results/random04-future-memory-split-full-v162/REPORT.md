# RANDOM-04 forecast memory: no improvement

All six original full runs pass independent replay and strict limits. The seed4 off control is exact in six trace fields. Reusing retained priorities in future mutation values loses on both planner seeds; keep the selected profile disabled.

| Seed | Blend | Step100 | Step500 | Step900 | Final1000 | Max entry ms |
|---|---:|---:|---:|---:|---:|---:|
| 4 | off | 242 | 1377 | 2417 | 2782 | 803.259 |
| 4 | half | 232 | 1376 | 2391 | 2746 | 802.101 |
| 4 | full | 221 | 1325 | 2310 | 2662 | 804.164 |
| 0 | off | 221 | 1362 | 2373 | 2749 | 785.253 |
| 0 | half | 219 | 1382 | 2354 | 2713 | 801.716 |
| 0 | full | 228 | 1350 | 2322 | 2683 | 805.806 |

Paired totals off/half/full are5531/5459/5345. Full and earlier-phase observations do not promote an alternative. The current2,782 record stays+5.02% versus matched max(NMS,KK)=2,649. These are development-input planner seeds, not fresh-input validation.

[Independent audit](audit.json), [exact control](control-equivalence.json), [phase counts](phases.json).
