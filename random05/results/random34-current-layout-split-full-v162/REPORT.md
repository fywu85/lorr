# Current-profile traffic-layout interaction

All 12 original full runs pass independent replay and strict limits. Both unchanged controls match all six complete trace fields. Every alternative layout loses; the selected 2,646 / 2,782 records remain unchanged.

| Instance | Layout | Tasks | Mean / max ms |
|---|---|---:|---:|
| RANDOM-03 | control | 2646 | 501.334 / 708.593 |
| RANDOM-03 | field15-noflip | 2614 | 498.997 / 714.421 |
| RANDOM-03 | field23 | 2598 | 524.261 / 707.738 |
| RANDOM-03 | field7 | 2514 | 516.020 / 698.071 |
| RANDOM-03 | field31 | 2566 | 524.809 / 747.240 |
| RANDOM-03 | field43 | 2603 | 538.138 / 713.936 |
| RANDOM-04 | control | 2782 | 474.482 / 802.773 |
| RANDOM-04 | field15-noflip | 2743 | 470.713 / 806.200 |
| RANDOM-04 | field105 | 2722 | 471.347 / 783.167 |
| RANDOM-04 | field109 | 2668 | 447.330 / 748.054 |
| RANDOM-04 | field111 | 2688 | 522.929 / 928.603 |
| RANDOM-04 | field115 | 2704 | 477.340 / 764.572 |

These are explicit map-specific tricks on archived inputs, not general improvements or fresh validation. Keep the current field15 with one flip for both densities. Matched max(NMS, KK) is 2,359 / 2,649 respectively.

| RANDOM-04 layout | Step 100 | Step 500 | Step 900 | Final 1,000 |
|---|---:|---:|---:|---:|
| control | 242 | 1377 | 2417 | 2782 |
| field15-noflip | 227 | 1375 | 2391 | 2743 |
| field105 | 223 | 1360 | 2364 | 2722 |
| field109 | 218 | 1324 | 2301 | 2668 |
| field111 | 233 | 1359 | 2322 | 2688 |
| field115 | 207 | 1341 | 2330 | 2704 |

The selected RANDOM-04 control and retained NMS reference both reach 2,417 tasks at step900; their full scores are 2,782 and 2,649. This locates the difference in time and does not isolate its cause.

[Independent audit](audit.json), [exact controls](control-equivalence.json), [phase counts](phases.json).
