# Failure-first third repair order: full negative comparison

Updated 2026-09-22 00:32 UTC.

All12full cases pass independent replay and strict checks. All five unchanged controls match all six archived trace fields. Every three-order variant is weaker; leave this optional feature disabled. Nominal order-attempt counts are matched for the work-matched cases, not measured CPU time. Source d323e605, build163; full regression62.13s.

| Case | Tasks | Mean / max entry ms | Peak RSS MB |
|---|---:|---:|---:|
| [general-random-01-failure-first-control](general-random-01-failure-first-control/summary.json) | 727 | 71.989 / 112.551 | 321.950 |
| [general-random-01-failure-first-work-matched](general-random-01-failure-first-work-matched/summary.json) | 720 | 72.987 / 103.382 | 328.708 |
| [general-random-01-failure-first-full-iterations](general-random-01-failure-first-full-iterations/summary.json) | 723 | 191.513 / 289.200 | 329.073 |
| [trick-random-01-failure-first-control](trick-random-01-failure-first-control/summary.json) | 735 | 70.069 / 114.266 | 386.814 |
| [trick-random-01-failure-first-work-matched](trick-random-01-failure-first-work-matched/summary.json) | 731 | 69.548 / 104.689 | 396.534 |
| [trick-random-01-failure-first-full-iterations](trick-random-01-failure-first-full-iterations/summary.json) | 733 | 241.434 / 398.433 | 382.517 |
| [general-random-02-failure-first-control](general-random-02-failure-first-control/summary.json) | 1397 | 512.909 / 712.328 | 347.177 |
| [general-random-02-failure-first-work-matched](general-random-02-failure-first-work-matched/summary.json) | 1389 | 529.163 / 791.622 | 349.618 |
| [trick-random-02-failure-first-control](trick-random-02-failure-first-control/summary.json) | 1408 | 350.512 / 546.144 | 370.049 |
| [trick-random-02-failure-first-work-matched](trick-random-02-failure-first-work-matched/summary.json) | 1383 | 353.441 / 534.206 | 362.947 |
| [trick-random-03-failure-first-control](trick-random-03-failure-first-control/summary.json) | 2621 | 651.935 / 856.132 | 464.171 |
| [trick-random-03-failure-first-work-matched](trick-random-03-failure-first-work-matched/summary.json) | 2582 | 453.917 / 678.010 | 473.485 |

[Independent source/input/replay/waiting audit](audit.json), [full control equivalence](control-equivalence.json).

All original outcomes are retained. These are archived-input development runs, not fresh-layout or fresh-input validation.
