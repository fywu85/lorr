# RANDOM-03/04 runtime evidence

All listed runs finish their full 800/1000-step horizons under a strict one-second limit. These observations do not establish a worst-case bound for unseen inputs.

| Configuration | Tasks | Mean ms | p95 ms | p99 ms | Max ms | RAM MB | Mean logical CPUs |
|---|---:|---:|---:|---:|---:|---:|---:|
| R03-control2551 | 2551 | 670.4 | 784.4 | 842.2 | 883.0 | 433 | 23.4 |
| R03-seed0 | 2522 | 494.9 | 542.4 | 565.8 | 686.0 | 437 | 31.3 |
| R03-seed3 | 2516 | 499.2 | 548.5 | 566.5 | 684.9 | 426 | 31.2 |
| R03-record2552 | 2552 | 522.9 | 598.6 | 619.0 | 760.8 | 442 | 31.2 |
| R04-record2634 | 2634 | 494.9 | 520.0 | 524.0 | 532.3 | 361 | 31.3 |
| R04-seed0 | 2584 | 474.6 | 501.1 | 517.7 | 641.1 | 364 | 31.3 |
| R04-seed3 | 2530 | 489.0 | 520.7 | 525.5 | 539.4 | 362 | 31.3 |
| R04-shared-cache2048 | 2634 | 446.7 | 466.2 | 468.5 | 484.6 | 1558 | 31.6 |
| R03-merge2580 | 2580 | 534.7 | 584.9 | 604.1 | 736.3 | 431 | 31.1 |
| R03-merge2580-heap4 | 2580 | 471.0 | 513.2 | 534.0 | 672.1 | 434 | 31.1 |
| R04-record2641 | 2641 | 440.7 | 459.9 | 462.9 | 497.7 | 1551 | 31.6 |
| R04-repeat2641 | 2641 | 442.5 | 459.9 | 467.3 | 478.5 | 1549 | 31.6 |
| R04-IPO2641 | 2641 | 427.2 | 450.4 | 456.4 | 501.3 | 1553 | 31.6 |
| R03-merge-seed0 | 2549 | 514.2 | 571.2 | 586.1 | 723.5 | 437 | 30.9 |
| R03-merge-seed1 | 2562 | 519.9 | 570.8 | 598.1 | 732.0 | 426 | 30.9 |
| R03-merge-seed3 | 2558 | 518.2 | 570.7 | 594.1 | 731.1 | 432 | 30.8 |
| R03-merge-seed4 | 2558 | 520.3 | 572.4 | 592.6 | 733.4 | 434 | 30.9 |

The current 2,580-task merged-window recipe has five planner-seed observations: 2,549–2,580 tasks, with all measured maxima below 737 ms. Its exact four-way-heap control averages 471 ms and peaks at 672 ms. Those seeds share the archived input; they are not fresh task/start validation.

The 2,641-task RANDOM-04 recipe has an exact repeat and an exact IPO build control. Additional planner seeds remain needed for this latest scheduler setting. The earlier 2,634-task recipe has three-seed observations. Neither instance has reached the ten-percent throughput target.

CPU averages count occupied logical CPUs over the process lifetime, including initialization and OpenMP waiting. Each allocation contains 16 physical cores / 32 logical CPUs. RAM is peak RSS in decimal MB.

Higher-work and other timed-out configurations remain in the campaign audits and are not represented as successful configurations here. [Machine-readable timings and evidence](timing.json).
