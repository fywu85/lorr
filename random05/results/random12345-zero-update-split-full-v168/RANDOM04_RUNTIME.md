# RANDOM-04 exact matching runtime qualification

All ten RANDOM-04 full runs (five paired planner seeds) pass independent source/input/resource checks, full action/assignment/task-event replay, the one-second entry limit and 32 GB. Every off/on pair matches all six complete trace fields exactly. The full eighteen-case all-five batch is also complete and independently audited.

| Planner seed | Tasks, both variants | Original maximum ms | Optimized maximum ms |
|---|---:|---:|---:|
| 0 | 2799 | 807.399 | 664.721 |
| 1 | 2771 | 817.693 | 669.091 |
| 2 | 2761 | 824.746 | 666.852 |
| 3 | 2761 | 816.286 | 666.778 |
| 4 | 2766 | 805.002 | 661.972 |

The optimized configuration uses source168 with `R05_MATCH_SKIP_ZERO=1` and optional-column compression. It preserves the selected **2,799** result and all declared search work. Its largest entry across these five seeds is **669.091 ms**; the control maximum is **824.746 ms**. This is a runtime gain, not a throughput gain.

At startup all ten solves perform 163,770 augmenting scans, of which 160,752 (98.157%) have a zero dual increment. The optimization skips those full-column writes while preserving every relaxation, predecessor and tie choice. Startup solving takes 338.328–338.968 ms versus 478.015–500.912 ms in the controls. Mean full-run latency varies with the runs and is not consistently faster; the clear benefit is the first-step spike.

Original noncompressed and compressed source166 startup timeouts remain failures in their original batches. The separate source168 guidance-layout control now repeats 2,799 and all six trace fields exactly, with mean474.599ms/max657.644ms and RSS689.877MB. Its full independent audit is complete. No fresh task/start input or unseen-map claim is made.

[Independent audit](audit.json), [five exact comparisons and phase data](random04-exact-runtime.json), [frozen protocol](../../experiments/random12345-zero-update-v168-protocol.json).
