# Bounded failed-wait PIBT retries

All original attempts are retained and independently audited against source307d5cc9/build159. Disabled controls match all six complete trace fields exactly.

| Case | Tasks | Mean entry ms | Max entry ms |
|---|---:|---:|---:|
| general-random-01-pibt-revisits0 | 727 | 101.189 | 154.509 |
| general-random-01-pibt-revisits1 | 721 | 71.146 | 118.758 |
| general-random-01-pibt-revisits2 | 721 | 72.039 | 104.529 |
| trick-random-01-pibt-revisits0 | 729 | 66.544 | 113.121 |
| trick-random-01-pibt-revisits1 | 726 | 98.191 | 163.819 |
| trick-random-01-pibt-revisits2 | 726 | 67.436 | 101.948 |
| general-random-02-pibt-revisits0 | 1397 | 507.611 | 703.329 |
| general-random-02-pibt-revisits1 | 1385 | 503.830 | 706.186 |
| general-random-02-pibt-revisits2 | 1385 | 679.300 | 970.004 |
| trick-random-02-pibt-revisits0 | 1408 | 334.678 | 517.489 |
| trick-random-02-pibt-revisits1 | 1402 | 474.617 | 693.106 |
| trick-random-02-pibt-revisits2 | 1393 | 326.584 | 495.906 |
| general-random-03-pibt-revisits0 | 1634 | 264.220 | 286.400 |
| general-random-03-pibt-revisits1 | 1615 | 304.791 | 329.508 |
| general-random-03-pibt-revisits2 | 1596 | 320.302 | 348.972 |
| trick-random-03-pibt-revisits0 | 2620 | 454.246 | 645.532 |
| trick-random-03-pibt-revisits1 | 2588 | 456.774 | 670.926 |
| trick-random-03-pibt-revisits2 | 2588 | 453.307 | 659.667 |
| general-random-04-pibt-revisits0 | 1595 | 627.316 | 836.525 |
| general-random-04-pibt-revisits1 | 1573 | 538.352 | 558.862 |
| general-random-04-pibt-revisits2 | 1614 | 668.812 | 858.244 |
| trick-random-04-pibt-revisits0 | 2782 | 470.379 | 817.773 |
| trick-random-04-pibt-revisits1 | 2750 | 555.947 | 851.100 |
| trick-random-04-pibt-revisits2 | 2733 | 594.218 | 871.388 |
| general-random-05-pibt-revisits0 | 2226 | 608.918 | 868.520 |
| general-random-05-pibt-revisits1 | 2084 | 635.118 | 725.978 |
| general-random-05-pibt-revisits2 | 2111 | 727.549 | 768.801 |
| trick-random-05-pibt-revisits0 | 4242 | 592.377 | 941.173 |
| trick-random-05-pibt-revisits1 | 4180 | 669.113 | 882.372 |
| trick-random-05-pibt-revisits2 | failure | — | — |

All selected trick candidates lose or fail. General04/two passes improves1595→1614, but remains below the independently audited face-cycle record1616. All other general profiles lose. Keep the option disabled; correctness of the two-robot repair did not imply a throughput gain at scale.

[Independent audit](audit.json), [exact controls](control-equivalence.json). These are development results on archived inputs, not fresh validation.
