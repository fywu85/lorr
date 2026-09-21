# RANDOM-02 frozen fresh-input validation V1

The frozen selected profile beats the stronger NMS repetition by **13.23% in aggregate**, with gains on both fresh task/start inputs. The general profile is **12.57% ahead**. These comparisons use the same layout; they do not establish performance on unseen maps.

| Task/start seed | Selected trick profile | General profile | NMS repeats | Selected gain over stronger NMS |
|---|---:|---:|---|---:|
| 50017 | 1382 | 1376 | 1213 / 1203 | +13.93% |
| 50018 | 1348 | 1338 | 1198 / 1182 | +12.52% |

Total: selected 2730, general 2714, stronger NMS 2411. The selected profile adds 0.590% over the general profile in this comparison. The profiles are independently frozen selections, not a common single-option ablation.

[Protocol f74286e6](https://github.com/fywu85/lorr/commit/f74286e6ba0490b74e1ed975b2289c93c79ae690) was committed before input generation at 2026-09-21T18:14:54.212857+00:00. Seeds50017/50018 are excluded from all subsequent tuning at every density. The exact [configuration](experiments/random02-fresh-validation-v1-full.json) and [protocol](experiments/random02-fresh-validation-v1-protocol.json) remain frozen.

Selected source: [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9). General source: [611aa2a6](https://github.com/fywu85/lorr/commit/611aa2a6). NMS uses the unmodified32-worker executable with SHA256 `2dfe6f35b3ae532d075aa487de88ec8d436a07ddef97e4dec24e94b550820539`.

All eight original runs complete600steps with no planner/scheduler errors or timeouts. Each uses16physical EPYC9354cores,32SMTworkers, verified binding, no CPU quota, a strict1s entry deadline,30s preprocessing deadline and32decimalGB guard. Every successful trace independently passes movement, collision, opened-assignment, waypoint/event and waiting-time replay checks.

| Solver | Mean entry time across runs (ms) | Maximum entry (ms) | Largest RSS (MB) |
|---|---:|---:|---:|
| Selected trick | 340.130–444.593 | 699.828 | 367.534 |
| General | 483.921–634.790 | 945.990 | 349.667 |
| NMS | 950.364–950.457 | 951.237 | 1696.141 |

Throughput is primary. Completed-order latency excludes unfinished orders; the oldest unfinished age is censored at the600step horizon. All secondary statistics are retained in the replay audit. These fresh-input totals do not replace development-input frontier records.

Evidence: [source/input/resource audit](results/random02-fresh-validation-v1/audit.json), [full replay and waiting audit](results/random02-fresh-validation-v1/replay-audit.json), [generation metadata](results/random02-fresh-validation-v1/generation.json), [archived run metadata](results/random02-fresh-validation-v1-split/).
