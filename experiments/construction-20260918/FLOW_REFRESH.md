# Cumulative traffic refresh, v30

The frozen margin-50 field improves two seeds but loses 35.73% in seed 2 versus
its same-binary, same-work no-flow control. The three-seed mean is 105,087.3
versus 109,225.0 without flow (-3.79%). The matching controls are 109,244,
109,249 and 109,182, with final-window counts 22,456 / 22,439 / 22,330. This
isolates the regression to the flow policy in these comparisons; it does not
identify which edges or feedback mechanism causes it. The profile is not promoted.
[Matched evidence](results/flow-margin-matched-controls-v20.json),
[new no-flow confirmations](results/work4m-confirm-v20/).

`CGAR_FLOW_REFRESH_INTERVAL` defaults to 0, retaining the original once-frozen
policy. Values 1–4096 keep cumulative counts of actual adjacent executed moves
and publish a new complete cost field after each prescribed number of additional
consecutive observations. Duplicate observations do no work; a gap never invents
intermediate motion. The field remains fixed between publications. Counts are
cumulative, not a rolling window, and no future tasks or route/map templates enter.

All edge costs remain positive. A changed field invalidates every old-metric
orientation table and speculative prefetch; an identical field preserves them.
The full ordinary solve still completes. Deadline checks reject an overrun;
there is no time-dependent publication or partial-action return. Refreshing has
an important unmeasured tradeoff: it removes stale costs but repeatedly makes
the orientation cache cold. Full validation must measure both throughput and
worst-case decision time, including those cache resets.

Build **8898622** passes all regressions and its source patch reconstructs every
requested hash. New independent checks use hand-counted clockwise/reverse
traffic for five publications, including a neutral field and one unchanged field
that must preserve the cache. Four metric changes discard stale tables, and
hand-computed new goal distances are checked. Another 640 edge comparisons
check rotated/reflected topology, plus skipped/duplicate/reversed observations,
invalid configuration, and default frozen behavior. Protected production tests
check 4,800 identical serial/four-thread actions across six publications, with
warm reuse and compact tables enabled.

Screen **8898623** uses one four-core EPYC 9354 instance, 8 GiB reservation,
200 steps, four preparation workers and exact one-second decisions. All pass:

| Refresh interval | Max entry seconds | Wall seconds | Publications observed |
|---|---:|---:|---:|
| 0 (frozen control) | 0.776158125 | 42.885 | 1 |
| 64 | 0.771582200 | 42.961 | 2 |
| 512 | 0.775961819 | 42.952 | 1 |

The interval-64 case publishes at observed steps 128 and 192, with 17,106 and
19,289 penalized directed edges and a cache reset each time. The control and
interval-512 cases exactly preserve the prior v20 frozen-field screen trajectory.
The latter has not yet refreshed by step 200; its update timing is untested by
this screen. No prefix task counts are used to rank policies.

Full **8898624**, analysis **8898625**, tests intervals 0, 512 and 1,024 at seeds
0 and 2. Each case uses 5,000 steps, four reserved physical cores, a one-second
complete decision limit, and a 32,000,000,000-byte process RSS limit. Two cases
run concurrently inside 24 GiB total reservation. The matrix follows the nearby
frozen-flow settings and precedes the pending v27/v28 exact-policy efficiency
checks; only a verified pending job was reordered. No full refresh outcome or
throughput recovery is established yet.

[Build](build-provenance/v30/), [screens](results/flow-refresh-screen-v30/),
[default equivalence](results/flow-refresh-default-equivalence.json),
[queue change](results/flow-refresh-queue-update.json).


## First completed full pair: seed 0

Completed first-pair analysis **8898634** independently checks all 5,000 decisions
and trajectory records. Frozen control exactly reproduces the v20 trajectory and
122,896 tasks. Interval512 reaches **134,511**, +9.45% versus frozen flow, with
windows22,523 /27,754 /28,022 /27,997 /28,215 and outstanding-task agep90 **653**
(control743). Maximum complete entry is **0.765266707s**, RSS **11,838,496,768bytes**;
there are no errors or timeouts. This is a new single-seed experimental peak,
12.07% below the local152,981 target. Seed2 and the complete matrix remain pending.

The gain does not isolate a mechanism: each changed field also clears old-metric
tables, temporarily increasing use of the fallback heuristic. Future diagnosis
must distinguish improved flow estimates from those cache/fallback effects.
No six-seed or default-promotion claim is made.

[First-pair evidence](results/flow-refresh-first-pair-v30/),
[continuously maintained progress log](../../WAREHOUSE_PROGRESS.md).
