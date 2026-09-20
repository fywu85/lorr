# Saved-state decision audit

The diagnostic capture reproduces all six full trajectory fields of the3,852-task original. All seven snapshots match its positions, headings, task pools, stages and assignments. A full-budget continuation control matches all five cumulative motion/task metrics at every one of128steps.

Each state compares eight distinct fully evaluated decisions under128steps of closed-loop replanning atK1280, with common downstream seeds0/1/2. The live solver usedK16320.

| Real step | Selected mean completions | Best observed mean | Best rank | Score/128-step completions Spearman | Leave-one-seed-out mean gain |
|---:|---:|---:|---:|---:|---:|
| 250 | 259.00 | 259.00 | 0 | 0.262 | +0.00 |
| 500 | 217.67 | 225.00 | 32 | -0.500 | -7.00 |
| 750 | 219.33 | 224.33 | 16 | 0.180 | -0.33 |
| 1000 | 210.67 | 213.00 | 64 | -0.443 | -2.67 |
| 1250 | 200.33 | 209.33 | 5 | -0.429 | +9.00 |
| 1500 | 233.33 | 243.67 | 64 | -0.431 | +5.33 |
| 1750 | 300.00 | 316.67 | 1 | -0.299 | +8.67 |

These small, correlated samples do not establish systematic scoring failure or a causal throughput gain. The best observed mean has selection bias. Leave-one-seed-out selection is an offline diagnostic using information unavailable to the live solver. Full benchmarks determine improvements.

[Machine-readable checks and all candidate outcomes](audit.json).
