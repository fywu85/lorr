# Observed-progress correction across RANDOM densities

All12full runs pass independent replay and strict limits. Three disabled01/02/03
controls match all six archived trace fields. The declared same-source04control
is2782 in the guidance-iteration batch and already matches its archived record.
Source4fb9498e/build162; every case requires its explicit instance trick.

| Case | Tasks | Mean / max entry ms | RSS MB |
|---|---:|---:|---:|
| [trick-random-01-progress-transfer-mix0-span32](trick-random-01-progress-transfer-mix0-span32/summary.json) | 735 | 70.810 / 107.480 | 393.183 |
| [trick-random-01-progress-transfer-mixp125-span32](trick-random-01-progress-transfer-mixp125-span32/summary.json) | 739 | 104.697 / 190.818 | 391.033 |
| [trick-random-01-progress-transfer-mixp25-span32](trick-random-01-progress-transfer-mixp25-span32/summary.json) | 739 | 73.552 / 114.527 | 388.555 |
| [trick-random-02-progress-transfer-mix0-span32](trick-random-02-progress-transfer-mix0-span32/summary.json) | 1408 | 349.080 / 543.010 | 369.144 |
| [trick-random-02-progress-transfer-mixp125-span32](trick-random-02-progress-transfer-mixp125-span32/summary.json) | 1405 | 340.507 / 531.421 | 361.308 |
| [trick-random-02-progress-transfer-mixp25-span32](trick-random-02-progress-transfer-mixp25-span32/summary.json) | 1403 | 353.917 / 552.468 | 365.326 |
| [trick-random-03-progress-transfer-mix0-span32](trick-random-03-progress-transfer-mix0-span32/summary.json) | 2621 | 639.414 / 812.020 | 466.280 |
| [trick-random-03-progress-transfer-mixp125-span32](trick-random-03-progress-transfer-mixp125-span32/summary.json) | 2628 | 486.753 / 705.363 | 469.963 |
| [trick-random-03-progress-transfer-mixp25-span32](trick-random-03-progress-transfer-mixp25-span32/summary.json) | 2646 | 484.103 / 676.507 | 469.467 |
| [trick-random-04-progress-transfer-mixp125-span32](trick-random-04-progress-transfer-mixp125-span32/summary.json) | 2766 | 483.072 / 843.422 | 698.319 |
| [trick-random-04-progress-transfer-mixp25-span16](trick-random-04-progress-transfer-mixp25-span16/summary.json) | 2768 | 459.699 / 791.689 | 702.829 |
| [trick-random-04-progress-transfer-mixp25-span128](trick-random-04-progress-transfer-mixp25-span128/summary.json) | 2777 | 486.560 / 806.141 | 695.468 |

RANDOM-01 .125/.25both reach739; the.25run finishes first and is the selected
chronological record. Its exact repetition and five planner seeds pass:
3665versus3651 (+0.3835%, all five positive). RANDOM-03reaches2628/2646;
the latter adds25tasks to2621. Its exact repetition matches all six fields and
five paired seeds total13084versus13029 (+0.4221%, all positive). RANDOM-02and04variants all lose; their settings stay unchanged.

RANDOM-05is covered by the separate field80qualification:4254selected, mean4237,
+2.2134%aggregate over field20on four paired planner seeds. No new fresh input
or unseen-layout claim is made for these development improvements.

[Full audit](audit.json), [three exact controls](control-equivalence.json),
[declared04control](../random345-flow-iterations-split-full-v162/control-equivalence.json),
[01qualification](../random01-record739-split-full-v162/REPORT.md),
[05qualification](../random05-record4254-split-full-v162/REPORT.md),
[03qualification](../random03-record2646-split-full-v162/REPORT.md).
