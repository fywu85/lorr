# CGAR migration benchmark — MR24

All ten instances completed at their archived horizons with **zero planner errors, scheduling errors, and entry timeouts**. The regression suite also passed, including 1,608 certificate cases and independent action checks.

One run per instance, 1000 ms per decision, 30000 ms preprocessing, and `--jobs 1` under a shared one-CPU quota. Before values come from the existing draft outputs, preserved in [cgar-draft.json](../cgar-draft.json). These are single-run throughput comparisons; elapsed times are not a controlled speed comparison with the earlier draft runs.

| Instance | Robots | Steps | Draft tasks | Fixed tasks | Change | Wall seconds |
|---|---:|---:|---:|---:|---:|---:|
| RANDOM-01 | 100 | 600 | 443 | 457 | +3.2% | 0.76 |
| RANDOM-02 | 200 | 600 | 544 | 574 | +5.5% | 0.96 |
| RANDOM-03 | 400 | 800 | 636 | 645 | +1.4% | 1.80 |
| RANDOM-04 | 700 | 1000 | 655 | 634 | -3.2% | 3.40 |
| RANDOM-05 | 800 | 2000 | 542 | 1032 | +90.4% | 7.01 |
| GAME | 6500 | 5000 | 5159 | 5761 | +11.7% | 285.82 |
| CITY-01 | 1500 | 3000 | 5978 | 6235 | +4.3% | 85.48 |
| CITY-02 | 3000 | 3000 | 6690 | 6768 | +1.2% | 135.80 |
| WAREHOUSE | 10000 | 5000 | 37812 | 26350 | -30.3% | 397.74 |
| SORTATION | 10000 | 5000 | 62703 | 30922 | -50.7% | 440.34 |

7 of ten instances improved. RANDOM-04 and both warehouse-domain results show why these correctness fixes still need throughput tuning; the contributions of individual changes have not been isolated.

**RANDOM-05 uses a restricted operating mode:** 31 robots park outside the core; 769 remain active, and only tasks whose entire remaining chain lies in the core are admitted. Its throughput gain must be interpreted with that restriction.

The fixes cover pocket evacuation, durable recovery witnesses, persistent primary tickets, task-epoch changes, scheduler coverage/fair admission, and bounded distance-table work. Passing the simulator and these regressions does not constitute an end-to-end starvation-freedom proof.

An earlier diagnostic run with three concurrent jobs was stopped after distance-table deadline overruns. These final results use the tested deadline fix and sequential execution.

[Source commit](https://github.com/fywu85/lorr/commit/c2f0e5b0116ebc48cef9ead19c2ea0b6693d588a) · [Machine-readable results](summary.json) · [CSV](summary.csv) · [Source and binary hashes](metadata.json) · [Regression output](../regression-20260917.txt)

Full simulator outputs, logs, and the executable snapshot remain locally in `runs/cgar-fixed-20260917-v2`.
