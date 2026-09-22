# RANDOM-05 declared startup work: complete follow-up

All four full2,000-step runs pass independent replay, bound32-worker allocation, strict1-second entries and32GB. Later work remainsK16320; only FIRST_K changes. Terminal pending credit remains0.5.

| First futures | Seed0 | Seed3 | Paired total |
|---|---:|---:|---:|
| 7968 (separate valid selected/qualification runs) | 4302 | 4224 | 8526 |
| 5952 | 4110 | 4226 | 8336 |
| 4032 | 4062 | 4230 | 8292 |

The two reduced portfolios lose in paired total despite small seed3 gains. Keep the selected4,302 recipe. The separate original disabled-credit seed3 control failed atstep0/1045.313ms; these changed configurations do not replace it or complete that four-pair qualification.

| Case | Step0 ms | Largest later ms | Mean ms |
|---|---:|---:|---:|
| trick-random-05-first-work5952-seed0 | 830.284 | 879.307 | 642.207 |
| trick-random-05-first-work4032-seed0 | 684.406 | 679.374 | 576.010 |
| trick-random-05-first-work5952-seed3 | 731.410 | 701.561 | 585.962 |
| trick-random-05-first-work4032-seed3 | 731.563 | 693.492 | 574.572 |

Startup timings include shared-host variation; no single-cause diagnosis follows from comparing separate runs. Throughput remains primary. No fresh input was used.

[Independent audit](audit.json), [startup/later timing](startup-latency.json).
