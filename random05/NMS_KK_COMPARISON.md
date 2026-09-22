# Matched NMS and Kitty Knight comparison

Updated 2026-09-22 02:23 UTC. Every headline reference is **max(NMS, Kitty Knight)** per instance. These combined-track runs use identical archived input hashes, full horizons,16 physical EPYC9354 cores/32 SMT workers,1s entries,30s preprocessing and the32 decimal GB process guard. Original failures remain in the evidence.

| Instance | PILOT selected best | Strongest valid NMS | Strongest valid KK | max(NMS, KK) | Difference |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 | 745 | 649 | 692 | 692 | +7.66% |
| RANDOM-02 | 1,408 | 1,233 | 1,256 | 1,256 | +12.10% |
| RANDOM-03 | 2,646 | 2,359 | 2,110 | 2,359 | +12.17% |
| RANDOM-04 | 2,799 | 2,649 | 1,472 | 2,649 | +5.66% |
| RANDOM-05 | 4,302 | 3,172 | 2,085 | 3,172 | +35.62% |

Selected bests are development maxima, not seed averages or a universal preset. Retain the strongest previously audited reference when a newer repetition scores lower. Published competition maxima688/1260/2334/2547/3050 use different task/start inputs and hardware and remain historical targets. The local RANDOM-04 reference increases from2,580 to2,649, so a10% lead now requires2,914.

## Baseline protocol and outcomes

The [32-case protocol](experiments/random-nms-kk-baselines-v1-protocol.json) and [manifest](experiments/random-nms-kk-baselines-full-v1.json) were frozen in [8b620cdb](https://github.com/fywu85/lorr/commit/8b620cdbd748f9aed6d232995af7dd8d3bd67a9b). It includes two repetitions of each team on all five archived inputs and12 KK runs supplementing earlier fresh RANDOM-01/02/03 comparisons. Thirty successes pass independent full action, assignment, task-event, waiting, timing and resource audits. Two original KK RANDOM-05 attempts fail and have no score. [Full audit](results/random-nms-kk-baselines-v1/audit.json).

New NMS repetitions are649/641,1232/1233,2352/2346,2649/2609,3150/3166. Original KK repetitions are692/685,1254/1256,2110/2080,1472/1397,and two failures. The earlier valid NMS2,359 and3,172 remain the strongest R03/R05 references.

KK uses its selected combined submission and preserved compiler/linker flags. [Compiled source audit](results/build-kk32-v1/source-audit.json) checks175 files from submissionda7d428b40cc1425b381adefe9709d4accbfdfe1 against repository commit667922b17bc736c7dc3b7a4e802cdf3ca77775f9. The executable SHA256 is `bdc989d3fb76da23356b6db871e0e154d6f136b2f3a052abbd73ce01004fe184`; runtime assets are hashed and verified for every run. NMS SHA256 is `2dfe6f35b3ae532d075aa487de88ec8d436a07ddef97e4dec24e94b550820539`.

## KK RANDOM-05 allocator diagnosis

Both original attempts fail at step35 with thread-creation/resource-deadlock errors. Unchanged monitored repeats on research35 and research36 reproduce that failure. Native VmSize reaches approximately31,998,714,000–31,998,718,000 bytes against the32,000,000,000-byte address-space guard, despite only about90 MB resident RAM and33 threads. Process-count limits exceed6 million. These measurements identify address-space exhaustion rather than establish a CPU-contention problem.

A separately [frozen allocator-only protocol](experiments/kk-random05-arena2-v1-protocol.json), commit[135897e1](https://github.com/fywu85/lorr/commit/135897e18105d625362e9ea5380d630c2efa6520), adds only `MALLOC_ARENA_MAX=2`. Both full2000-step runs then complete2,085 tasks. Binary, source, input,32 workers, memory guard and deadlines are unchanged. This supports excessive allocator reservation as the failure cause; it does not modify the planning/scheduling algorithm. The original failed attempts are retained.

| Host | Tasks | Mean / maximum entry (ms) | Peak RSS (MB) | Whole-run CPU core-equivalents |
|---|---:|---:|---:|---:|
| research35 | 2085 | 279.132 / 476.728 | 206.709 | 2.24 |
| research36 | 2085 | 277.873 / 480.589 | 207.356 | 2.21 |

Evidence: [original monitored35](results/kk-random05-resource-research35-v1/audit.json), [original monitored36](results/kk-random05-resource-research36-v1/audit.json), [allocator35](results/kk-random05-arena2-research35-v1/audit.json), [allocator36](results/kk-random05-arena2-research36-v1/audit.json). Monitoring measures process resources and does not change the solver.

## Fresh task/start supplements

| Frozen comparison | Selected total | General total | Sum of per-input max(NMS, KK) | Selected gain | General gain |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 | 1432 | 1425 | 1372 | +4.37% | +3.86% |
| RANDOM-02 | 2730 | 2714 | 2478 | +10.17% | +9.52% |
| RANDOM-03 V2 | 5226 | — | 4801 | +8.85% | — |
| RANDOM-03 V3 | 5282 | — | 4841 | +9.11% | — |

These supplements preserve the frozen PILOT profiles and all held-out exclusions. RANDOM-03 V2 remains0.115% below its preceding solver in aggregate, although faster. The R01/R02 frozen profiles and R03 V2 use the same geometry with new task/start streams. R03 V1 and R05 V6 did not measure KK on their streams: retain their NMS-only labels.

Machine-readable references: [matched maxima and evidence](references/matched-nms-kk-combined.json).

Fresh RANDOM-03 V3 directly includes two repetitions of both teams in its pre-generation protocol. It qualifies the2646profile:2660/2622 versus per-input maxima2461/2380 (+9.110%aggregate), and +0.590%over the preceding2620recipe on those same inputs. All12original runs pass strict checks and full independent replay. This misses the10%fresh milestone. V2 remains in the machine-readable history; these different streams/configurations must not be pooled as one matched experiment. [V3report](RANDOM03_FRESH_VALIDATION_V3.md).
