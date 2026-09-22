# Exact SIMD matching startup checks

Runtime diagnostics only:20steps, unchanged full-horizon settings,1,000ms entry limit,16physical cores/32workers. Full throughput qualification remains required.

| Case | Pass | Mean ms | Maximum ms |
|---|---:|---:|---:|
| trick-random-04-simd0-startup-seed0 | True | 489.379 | 681.316 |
| trick-random-04-simd1-startup-seed0 | True | 475.509 | 537.080 |
| trick-random-04-simd0-startup-seed6 | True | 492.117 | 684.445 |
| trick-random-04-simd1-startup-seed6 | True | 476.537 | 536.814 |
| trick-random-05-simd0-startup-seed0 | False | — | — |
| trick-random-05-simd1-startup-seed0 | True | 621.790 | 661.671 |
| trick-random-05-simd0-startup-seed9 | True | 640.065 | 719.353 |
| trick-random-05-simd1-startup-seed9 | True | 622.521 | 661.585 |

All successful compared prefixes are exact across the six action/schedule/event/task fields. The failed05control is preserved, with no partial score. SIMD retains arithmetic order within each reduced-cost expression, the original stable column tie break, all augmentations and complete fixed search. Runtime AVX2 detection preserves a scalar fallback on unsupported CPUs. The feature is opt-in as `R05_MATCH_SIMD=1`.

The compiler/binding-only full v171 batch has additional failures; it is insufficient as a reliability fix. The persistent-worker repair is tested separately.

[Independent audit](audit.json).
