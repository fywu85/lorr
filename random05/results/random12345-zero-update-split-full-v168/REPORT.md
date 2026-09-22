# Exact matching zero-update elision: all-five results

All 18 full runs pass independent replay, strict one-second entries, source/input/allocation checks and 32 GB. All 16 declared historical references and all nine contemporary off/on pairs match six complete trace fields exactly. The optimization changes execution time, not planning decisions or throughput.

| Instance / selected seed | Tasks, both | Matched max(NMS, KK) | Lead | Off max ms | On max ms |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 / 4 | 745 | 692 | +7.66% | 179.680 | 154.724 |
| RANDOM-02 / 2 | 1408 | 1256 | +12.10% | 766.525 | 757.296 |
| RANDOM-03 / 3 | 2646 | 2359 | +12.17% | 706.980 | 693.944 |
| RANDOM-04 / 0 | 2799 | 2649 | +5.66% | 807.399 | 664.721 |
| RANDOM-05 / 0 | 4302 | 3172 | +35.62% | 868.178 | 716.671 |

RANDOM-04 has five paired seeds: 2799/2771/2761/2761/2766 for both implementations. Their optimized maxima are 661.972–669.091 ms versus 805.002–824.746 ms for controls. Startup matching solving falls from478.015–500.912ms to338.328–338.968ms;160752of163770augmentations have zero increments. Mean full-run latency is mixed and not a consistent speedup. The benefit is startup headroom, with fixed search work unchanged.

The separate guidance-layout control repeats the optimized2799 trace again, max657.644ms. Original source166 failures remain invalid attempts; these new valid runs do not substitute for them in prior comparisons. No new throughput record, fresh-input qualification or unseen-map claim is made.

[Independent audit](audit.json), [16 historical references](control-equivalence.json), [nine paired exact comparisons](paired-equivalence.json), [RANDOM-04 runtime detail](RANDOM04_RUNTIME.md).
