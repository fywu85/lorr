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
| R03-record2602 | 2602 | 507.1 | 554.8 | 578.5 | 709.8 | 433 | 31.1 |
| R03-2602-repeat | 2602 | 474.7 | 517.1 | 539.1 | 661.0 | 428 | 31.1 |
| R03-2602-seed0 | 2548 | 472.2 | 524.4 | 545.6 | 655.3 | 437 | 31.1 |
| R03-2602-seed3 | 2566 | 471.7 | 519.3 | 537.8 | 674.1 | 442 | 31.1 |
| R04-2641-seed0 | 2526 | 444.6 | 463.3 | 467.5 | 486.0 | 1536 | 31.6 |
| R04-2641-seed1 | 2601 | 440.3 | 458.4 | 462.5 | 473.7 | 1543 | 31.6 |
| R04-2641-seed3 | 2596 | 443.5 | 461.1 | 465.7 | 480.9 | 1569 | 31.6 |
| R04-2641-seed5 | 2628 | 439.8 | 458.4 | 461.3 | 475.4 | 1539 | 31.6 |
| R04-2641-profile | 2641 | 429.0 | 448.2 | 454.3 | 490.1 | 1570 | 31.6 |
| R03-fresh50011 | 2599 | 507.8 | 563.5 | 576.6 | 700.1 | 433 | 31.1 |
| R03-fresh50012 | 2557 | 471.9 | 513.9 | 533.6 | 675.6 | 429 | 31.1 |
| R04-record2661 | 2661 | 430.4 | 448.5 | 451.8 | 465.9 | 1551 | 31.6 |
| R04-2661-seed0 | 2542 | 428.3 | 447.2 | 453.3 | 469.4 | 1580 | 31.6 |
| R04-2661-seed3 | 2576 | 426.2 | 446.4 | 450.3 | 465.6 | 1561 | 31.6 |
| R04-2661-seed5 | 2605 | 428.0 | 446.3 | 451.1 | 475.1 | 1544 | 31.6 |
| R04-2661-repeat96 | 2661 | 453.5 | 471.3 | 474.6 | 489.4 | 1533 | 31.6 |
| R04-2661-control108 | 2661 | 435.3 | 453.5 | 456.1 | 471.2 | 2058 | 31.6 |
| R04-2661-control109 | 2661 | 450.0 | 468.8 | 473.3 | 487.8 | 2076 | 31.6 |
| R04-2661-control111 | 2661 | 464.6 | 483.5 | 487.9 | 490.8 | 2043 | 31.6 |
| R04-2661-control113 | 2661 | 540.3 | 645.6 | 685.8 | 755.1 | 2104 | 28.3 |
| R04-2661-control114 | 2661 | 445.5 | 469.1 | 471.7 | 483.9 | 2077 | 31.6 |
| R04-2661-control115 | 2661 | 464.9 | 484.3 | 489.5 | 502.2 | 2082 | 31.6 |
| R04-2661-control116 | 2661 | 462.5 | 481.7 | 486.0 | 506.1 | 2084 | 31.6 |
| R04-record2698 | 2698 | 448.0 | 468.7 | 473.9 | 654.1 | 1733 | 30.3 |
| R04-2698-seed0 | 2701 | 469.3 | 490.3 | 499.8 | 668.5 | 1729 | 30.3 |
| R04-2698-seed3 | 2654 | 445.0 | 467.0 | 476.2 | 655.9 | 1707 | 30.3 |
| R04-2698-seed5 | 2705 | 469.0 | 490.0 | 500.2 | 670.6 | 1724 | 30.3 |
| R04-record2738 | 2738 | 462.7 | 478.7 | 489.3 | 670.9 | 1736 | 30.3 |
| R04-2661-v117-32byte | 2661 | 439.7 | 458.5 | 462.2 | 497.6 | 1180 | 31.6 |
| R04-2698-v117-32byte | 2698 | 463.1 | 483.5 | 491.2 | 667.7 | 1020 | 30.3 |
| R04-2661-v118-32byte | 2661 | 453.6 | 471.7 | 475.0 | 483.7 | 1177 | 31.6 |
| R04-2698-v118-32byte | 2698 | 462.8 | 481.7 | 491.1 | 674.7 | 1032 | 30.3 |
| R04-2661-v118-8byte | 2661 | 439.4 | 455.3 | 458.7 | 477.4 | 604 | 31.6 |
| R04-2698-v118-8byte | 2698 | 435.6 | 457.4 | 466.9 | 656.9 | 567 | 30.2 |
| R04-2698-v119-slowmatch | 2698 | 456.2 | 478.9 | 486.2 | 656.0 | 558 | 30.2 |
| R04-2698-v119-fastmatch | 2698 | 427.0 | 445.4 | 451.7 | 618.7 | 553 | 31.1 |
| R04-2661-v119-fastmatch | 2661 | 455.3 | 473.7 | 475.7 | 513.7 | 602 | 31.6 |
| R04-record2741 | 2741 | 450.1 | 464.8 | 479.7 | 649.1 | 1683 | 30.3 |
| R04-2738-repeat116 | 2738 | 482.4 | 500.7 | 508.6 | 665.5 | 1702 | 30.3 |
| R04-2738-seed0 | 2718 | 459.6 | 474.6 | 481.8 | 661.2 | 1752 | 30.3 |
| R04-2738-seed3 | 2689 | 450.4 | 467.6 | 477.2 | 651.9 | 1743 | 30.3 |
| R04-2738-seed5 | 2735 | 452.9 | 469.6 | 487.1 | 654.3 | 1764 | 30.3 |
| R04-2738-fast119 | 2738 | 432.0 | 451.4 | 454.9 | 684.3 | 566 | 31.1 |
| R04-2738-extra-control119 | 2738 | 455.1 | 468.8 | 472.7 | 625.5 | 565 | 31.1 |
| R04-2738-control120 | 2738 | 448.5 | 463.2 | 467.8 | 631.7 | 572 | 31.1 |
| R04-record2762 | 2762 | 447.9 | 460.3 | 465.8 | 629.9 | 554 | 31.2 |
| R04-2741-fast119 | 2741 | 446.9 | 459.3 | 469.4 | 629.7 | 550 | 31.1 |
| R04-2741-seed0 | 2719 | 432.0 | 445.3 | 449.9 | 621.5 | 536 | 31.2 |
| R04-2741-seed3 | 2710 | 432.0 | 446.7 | 450.0 | 618.3 | 567 | 31.2 |
| R04-2741-seed5 | 2759 | 434.9 | 454.7 | 460.7 | 618.0 | 558 | 31.2 |
| R04-2762-seed0 | 2725 | 449.7 | 462.7 | 467.5 | 631.0 | 562 | 31.2 |
| R04-2762-seed3 | 2751 | 445.0 | 456.7 | 463.5 | 635.9 | 534 | 31.1 |
| R04-record2772 | 2772 | 448.3 | 460.1 | 469.6 | 628.8 | 541 | 31.1 |
| R04-record2776 | 2776 | 434.8 | 454.1 | 458.3 | 626.7 | 553 | 31.2 |
| R04-2762-repeat119 | 2762 | 521.2 | 620.3 | 659.0 | 729.5 | 551 | 28.3 |
| R04-2776-seed0 | 2730 | 433.9 | 447.9 | 452.3 | 627.8 | 550 | 31.2 |
| R04-2776-seed3 | 2711 | 433.1 | 446.8 | 455.3 | 622.1 | 535 | 31.1 |
| R04-2776-seed5 | 2776 | 448.6 | 460.7 | 469.9 | 629.5 | 548 | 31.1 |
| R04-2741-control121 | 2741 | 445.8 | 466.3 | 471.5 | 633.0 | 548 | 31.2 |
| R04-2762-control122 | 2762 | 451.6 | 464.9 | 470.1 | 630.1 | 539 | 31.1 |
| R04-2776-control123 | 2776 | 447.6 | 459.9 | 468.5 | 640.6 | 549 | 31.1 |
| R04-record2777 | 2777 | 469.5 | 478.8 | 495.0 | 779.6 | 697 | 30.4 |
| R04-2776-source124-control | 2776 | 449.8 | 463.1 | 471.9 | 646.5 | 536 | 31.0 |
| R04-2776-original-repeat | 2776 | 428.9 | 445.2 | 452.3 | 622.3 | 532 | 31.2 |
| R04-2777-original-repeat | 2777 | 442.6 | 457.9 | 477.4 | 770.0 | 694 | 30.4 |
| R03-2602-source124-control | 2602 | 474.0 | 517.9 | 539.0 | 671.7 | 432 | 31.1 |
| R04-2777-seed0 | 2726 | 469.2 | 478.5 | 485.9 | 780.2 | 694 | 30.4 |
| R04-2777-seed3 | 2742 | 443.1 | 461.4 | 485.0 | 771.9 | 691 | 30.4 |
| R04-2777-seed5 | 2771 | 455.9 | 472.9 | 486.2 | 788.0 | 700 | 30.4 |
| R04-2776-source125-control | 2776 | 462.0 | 475.9 | 481.7 | 655.0 | 563 | 31.0 |

The 2,602-task RANDOM-03 recipe has an exact repetition and three planner seeds (5/0/3): 2,602/2,548/2,566 tasks. The original, repeat and both other seeds all peak below 710 ms. Frozen fresh inputs give 2,599/2,557 tasks: +11.69%/+9.13% against the stronger NMS repetition, +10.41% aggregate. [Frozen validation](../../RANDOM03_FRESH_VALIDATION_V1.md).

The 2,661-task RANDOM-04 recipe has exact repetitions and four planner seeds (0/3/4/5): 2,542/2,576/2,661/2,605 tasks. Across the listed originals, repetitions and equivalent controls, the maximum is below 756 ms. The original build96 record and its seed checks peak below 490 ms; newer source controls are listed separately. Runtime has measured margin; throughput remains 177 tasks short of the 2,838 target. The older 2,641-task profile and its five-seed qualification are retained above as history.

The 2,698-task admission profile is checked on seeds0/3/4/5:2701/2654/2698/2705. All original-source qualification runs peak below671 ms. Every seed improves over the corresponding uncapped profile; initial unfinished orders increase on the record seed. The exact8byte-cache and fast-matching controls are listed separately.

The 2,738-task profile (cap560, cutoff1) has an exact repetition and four seeds0/3/4/5:2718/2689/2738/2735. Original-source qualification peaks below671 ms; equivalent newer-source controls peak below685 ms. The 2,741-task cutoff0.875 record and its subsequent seed checks remain separately listed.

The contrast2.2, dispersion0.4, cap560, cutoff0.875 profile has four seeds0/3/4/5:2730/2711/2776/2776, all maxima below630 ms. Its original-binary seed4 repetition preserves all six trace fields and peaks at622.264 ms; newer equivalent controls are separately listed. The preceding contrast2.4 profile has an exact repetition at729.537 ms; that slower observation is retained.

The current throughput frontier and remaining target are maintained in [PILOT_PROGRESS.md](../../../PILOT_PROGRESS.md) and [the campaign](../../RANDOM34_CAMPAIGN.md). A higher score does not inherit the timing qualification of a different cutoff or admission profile.

CPU averages count occupied logical CPUs over the process lifetime, including initialization and OpenMP waiting. Each allocation contains 16 physical cores / 32 logical CPUs. RAM is peak RSS in decimal MB.

Higher-work and other timed-out configurations remain in the campaign audits and are not represented as successful configurations here. [Machine-readable timings and evidence](timing.json).
