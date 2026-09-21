# RANDOM-01 frozen fresh-input validation V1

The frozen selected profile is **+4.37% above matched max(NMS, Kitty Knight)** in aggregate; its general control is **+3.86%**. Each task/start input is positive. Four later Kitty Knight repetitions supplement the original NMS-only protocol, without changing PILOT configurations or selecting on these inputs. Both teams use the same inputs and allocation.

| Task/start seed | Selected trick | General | NMS repeats | KK repeats | max(NMS, KK) | Selected gain |
|---|---:|---:|---|---|---:|---:|
| 50017 | 730 | 727 | 645 / 654 | 696 / 694 | 696 | +4.89% |
| 50018 | 702 | 698 | 630 / 630 | 670 / 676 | 676 | +3.85% |

Totals: selected 1432, general 1425, matched stronger-team reference 1372. These fresh task/start streams share one layout; this is not unseen-map validation. The independently selected general/trick profiles are not a single-option ablation.

The supplement was frozen in [8b620cdb](https://github.com/fywu85/lorr/commit/8b620cdbd748f9aed6d232995af7dd8d3bd67a9b). All four KK runs pass full replay, timing and resource checks. See the [supplement audit](results/random-nms-kk-baselines-v1/audit.json) and [stronger-team reference](references/matched-nms-kk-combined.json). The original eight-run audit below remains unchanged.

[Protocol f74286e6](https://github.com/fywu85/lorr/commit/f74286e6ba0490b74e1ed975b2289c93c79ae690) was committed before input generation at 2026-09-21T18:14:49.946479+00:00. Seeds50017/50018 are excluded from all subsequent tuning at every density. The exact [configuration](experiments/random01-fresh-validation-v1-full.json) and [protocol](experiments/random01-fresh-validation-v1-protocol.json) remain frozen.

Selected source: [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9). General source: [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2). NMS uses the unmodified32-worker executable with SHA256 `2dfe6f35b3ae532d075aa487de88ec8d436a07ddef97e4dec24e94b550820539`.

All eight original runs complete600steps with no planner/scheduler errors or timeouts. Each uses16physical EPYC9354cores,32SMTworkers, verified binding, no CPU quota, a strict1s entry deadline,30s preprocessing deadline and32decimalGB guard. Every successful trace independently passes movement, collision, opened-assignment, waypoint/event and waiting-time replay checks.

| Solver | Mean entry time across runs (ms) | Maximum entry (ms) | Largest RSS (MB) |
|---|---:|---:|---:|
| Selected trick | 62.166–62.915 | 109.993 | 363.303 |
| General | 66.671–94.460 | 141.452 | 332.468 |
| NMS | 950.349–950.387 | 953.917 | 1689.706 |

Throughput is primary. Completed-order latency excludes unfinished orders; the oldest unfinished age is censored at the600step horizon. All secondary statistics are retained in the replay audit. These fresh-input totals do not replace development-input frontier records.

Evidence: [source/input/resource audit](results/random01-fresh-validation-v1/audit.json), [full replay and waiting audit](results/random01-fresh-validation-v1/replay-audit.json), [generation metadata](results/random01-fresh-validation-v1/generation.json), [archived run metadata](results/random01-fresh-validation-v1-split/).
