# Directional evidence and fixed-work follow-up

The original strengths 1/2/4 all fail full-run temporal-repair deadlines, at steps
1,103 / 1,249 / 1,119. They receive no accepted scores. At step 1,000, strength 1
inspects 63.1M candidates despite just 50k attempts.
[All failure records](results/flow-full-v16-failures/).

A reserved-core replay of the unchanged 50k control audits the executed traffic
that the original field observed. It independently reproduces 998,691 moves and
37,474 penalized directions at step 128. Of those directions, 11,468 (30.6%)
have the opposite cumulative majority by step 1,024. Restricting the initial
imbalance to over 50% leaves 17,345 edges, but 23.8% still reverse. At step 1,024,
only 6,611 edges exceed that margin. This shows substantial early/late variation;
it does not prove that noise caused the failed planner runs.
[Read-only audit](results/flow-confidence-audit/). No future trajectory data is
loaded into the planner.

Build v20 adds `CGAR_FLOW_MIN_MARGIN_PERCENT` (0–100, default 0). An edge receives
a penalty only when the opposite count exceeds the forward count by strictly
more than this percentage of their sum. A setting of 50 therefore requires
more than 75% of observed edge crossings to favor the opposite direction. This
is a deterministic evidence margin, not a statistical confidence interval.

All regressions pass, including independently counted 5:3 directional traffic,
the exact 25% boundary, 640 rotated/reflected edge comparisons, and protected
serial/four-thread production episodes. The corrected source patch reconstructs
every tested hash. An incomplete first source-edit attempt was mistakenly
submitted for building; that build was canceled, never benchmarked, and is
recorded separately in the corrected build archive.

Four 200-step screens pass (maximum entry 0.790/0.773/0.793/0.508 seconds). These
cover the 4M-work control, original flow with that work threshold, margin 50 with
that threshold, and the old 50k-flow default for exact trajectory comparison.
The default field preserves every prior trajectory field. The margin-50 field
actually activates, retaining 17,106 penalized directions from the same 961,331
observed moves as the matched 4M/no-margin case (37,308 directions).
[Screen evidence](results/flow-margin-screen-v20/),
[default equivalence](results/flow-margin-default-equivalence.json).

Full job **8898517** and analysis **8898518** follow the temperature matrix. Four
profiles compare 4M-work control, original flow, margin 50, and margin 50 with
freeze delayed to 1,024 observations. The late-freeze profile has no activated
200-step screen; its actual guided phase is tested by the full 5,000-step run.
Each instance uses one physical core; two run concurrently inside 24 GiB total
reserved memory. Warm starts and regional repair are disabled in every case.

The 4M threshold is checked only between complete attempts, includes construction,
and permits the last attempt to overshoot. Construction always completes and
there is a fixed attempt cap. Elapsed time never decides successful stopping;
an absolute deadline overrun still fails the whole decision. Full throughput,
late feasibility and six-seed validation remain outstanding.
