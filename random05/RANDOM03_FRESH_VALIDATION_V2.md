# RANDOM-03 frozen fresh-input validation v2

Completed 2026-09-21. The frozen weighted-search candidate finishes **2,617 / 2,609** tasks. Against the previous search on the same two inputs it is **0.115% lower in aggregate**, with one gain and one loss. It does not establish a throughput improvement over that baseline.

The candidate is **+8.852% above matched max(NMS, Kitty Knight)** in aggregate; this misses the ten-percent milestone. Four later unmodified Kitty Knight repetitions now supplement the original protocol: 2,124/2,116 on seed50019 and 2,044/2,058 on seed50020. NMS is stronger on both streams, so the numerical margin is unchanged. Every KK run passes independent replay and strict limits. The supplement was frozen in [8b620cdb](https://github.com/fywu85/lorr/commit/8b620cdbd748f9aed6d232995af7dd8d3bd67a9b); see [its audit](results/random-nms-kk-baselines-v1/audit.json).

The [protocol](experiments/random03-fresh-validation-v2-protocol.json) and [eight-case manifest](experiments/random03-fresh-validation-v2-full.json) were committed in [ce650a6d](https://github.com/fywu85/lorr/commit/ce650a6d43440d30f741ec8a0bc9df6ea3f22ae4) before generation at 2026-09-21T19:34:25.511475+00:00. Seeds50019/50020 remain excluded from tuning at every density. Both PILOT profiles use [source153](https://github.com/fywu85/lorr/commit/1a3076420bc245d5f56839936d7814eb9087b050), planner seed3 and identical settings except `R05_WINDOW_HEURISTIC_WEIGHT=1.2` versus1. This isolates proposal-search priority. The configuration uses explicit `--trick RANDOM-03` guidance and known-horizon triage.

Every run uses800steps,16 physical EPYC9354 cores/32 SMT workers, strict1000ms entries,30s preprocessing and a32 decimal GB process guard. All eight pass independent source, protocol, resource, timing, movement, collision, assignment and task-event checks. Work remains fixed; no partial timed portfolio is returned.

| Input seed | Candidate | Previous | NMS repeat1 | NMS repeat2 | Candidate mean/max ms | Previous mean/max ms |
|---|---:|---:|---:|---:|---:|---:|
| 50019 | 2617 | 2612 | 2413 | 2403 | 455.6/659.5 | 553.1/809.3 |
| 50020 | 2609 | 2620 | 2388 | 2371 | 480.2/649.8 | 545.7/795.9 |

Candidate total5,226 versus previous5,232 and stronger NMS4,801. The clearer observed benefit is runtime: candidate means455.6/480.2ms versus553.1/545.7ms, with maxima659.5/649.8ms versus809.3/795.9ms. These two fresh task/start streams share the archived layout; they are not unseen geometry or the colleague's private instances.

The development record2,620 and its exact repetition remain valid. Five paired development planner seeds give13,016 versus12,974 (+0.324%,three gains/two losses). A new unchanged-weight seed3 attempt failed at the first entry1046.391ms; the earlier successful source132 baseline remains separately identified. See the [five-seed evidence](results/random03-record2620-split-full-v153/paired-comparison.json).

| Run | Longest completed order, steps | Oldest unfinished, censored steps | Initial orders still unfinished |
|---|---:|---:|---:|
| seed50019-ours | 796 | 800 | 37 |
| seed50019-baseline | 781 | 800 | 39 |
| seed50019-nms-repeat1 | 745 | 800 | 43 |
| seed50019-nms-repeat2 | 772 | 800 | 42 |
| seed50020-ours | 764 | 800 | 40 |
| seed50020-baseline | 750 | 800 | 42 |
| seed50020-nms-repeat1 | 776 | 800 | 42 |
| seed50020-nms-repeat2 | 766 | 800 | 43 |

Waiting metrics are secondary and unfinished orders are censored at800steps. Their eventual maximum wait is unknown.

Evidence: [protocol/source/timing audit](results/random03-fresh-validation-v2/audit.json), [replay and waiting audit](results/random03-fresh-validation-v2/replay-audit.json), [generation metadata](results/random03-fresh-validation-v2/generation.json).
