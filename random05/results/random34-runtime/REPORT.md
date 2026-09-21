# RANDOM-03/04 runtime evidence

All listed runs finish their complete800/1000-step horizons under strict1s.
These are archived-input observations, not a worst-case guarantee for all inputs.

| Case | Tasks | Mean ms | p95 ms | p99 ms | Max ms | RSS MB |
|---|---:|---:|---:|---:|---:|---:|
| R03-control2551 | 2551 | 670.4 | 784.4 | 842.2 | 883.0 | 433 |
| R03-seed0 | 2522 | 494.9 | 542.4 | 565.8 | 686.0 | 437 |
| R03-seed3 | 2516 | 499.2 | 548.5 | 566.5 | 684.9 | 426 |
| R03-record2552 | 2552 | 522.9 | 598.6 | 619.0 | 760.8 | 442 |
| R04-record2634 | 2634 | 494.9 | 520.0 | 524.0 | 532.3 | 361 |
| R04-seed0 | 2584 | 474.6 | 501.1 | 517.7 | 641.1 | 364 |
| R04-seed3 | 2530 | 489.0 | 520.7 | 525.5 | 539.4 | 362 |
| R04-shared-cache2048 | 2634 | 446.7 | 466.2 | 468.5 | 484.6 | 1558 |

The2551 configuration has three planner-seed timing observations. Its2552
annealed variant is a separate single-seed record. The2634 configuration also
has three planner-seed observations; its shared-cache variant is an exact full
trajectory control measured separately. Neither density has reached its current
10% throughput target or undergone newly frozen fresh-input qualification.

Higher-work and longer-window attempts that timed out remain in the campaign
audits. They are not represented by the successful configurations above.
[Machine-readable timings and evidence](timing.json).
