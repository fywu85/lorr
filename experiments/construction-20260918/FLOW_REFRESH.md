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
an important tradeoff: it removes stale costs but repeatedly makes
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

Full **8898624**, analysis **8898625**, is complete. All six cases pass the
5,000-step, one-second decision and 32 GB process checks. Four physical cores per
case and two concurrent cases use 24 GiB on exclusive research31, with no CPU quota.

| Seed | Frozen | Refresh 512 | Refresh 1,024 |
|---|---:|---:|---:|
| 0 | 122,896 | **134,511** | 133,652 |
| 2 | 70,171 | **134,519** | 131,316 |

Both frozen controls exactly reproduce the prior full trajectories. Refresh512
finishes with 28,215 / 28,214 tasks in the final 1,000 steps, outstanding-task
age p90 653 / 652, max entry 0.765267 / 0.762019 seconds, and peak RSS
11,838,496,768 / 11,888,349,184 bytes. Its two-seed mean is 134,515, 23.17%
above the matched no-flow mean 109,213 from the earlier exact-control study.
This comparison recovers the previously failing seed; it does not establish six-seed
robustness. Refresh1024 final windows are 28,386 / 27,969; all six max entries
are below 0.782 seconds and RSS below 11.925 GB.

Confirmation **8898647 / 8898648** compares refresh512 and no-flow at all six
seeds (1,3,4,5,0,2), using the same frozen v30 binary. Four cases run concurrently
on 16 reserved physical cores under a 48 GiB matrix allocation; each planner
still has its own 32 GB validity limit. It follows the scale study on an exclusive
host. Repeated seeds 0/2 also test exact reproduction in the new allocation.

The mechanism remains partly confounded: changed costs also clear the orientation
cache. The recorded fallback counts spike after refresh. Frozen flow with warm
reuse separately recovers seed2 to 123,251, so the initial field is not inevitably
fatal under every local policy. A fixed-field, cache-only refresh control is next.

[Full results](results/flow-refresh-full-v30/),
[exact control checks](results/flow-recovery-controls-v30-v31.json),
[cache observations](results/flow-refresh-cache-observations-v30.json).

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
12.07% below the local152,981 target. This was the first completed pair; the complete matrix above now confirms seed2 recovery.

The gain does not isolate a mechanism: each changed field also clears old-metric
tables, temporarily increasing use of the fallback heuristic. Future diagnosis
must distinguish improved flow estimates from those cache/fallback effects.
No six-seed or default-promotion claim is made.

[First-pair evidence](results/flow-refresh-first-pair-v30/),
[continuously maintained progress log](../../WAREHOUSE_PROGRESS.md).


## Four-seed confirmation in progress

Completed-subset analysis **8898668** validates seeds1/3 from the ongoing six-seed
matrix, using a symlinked subset that leaves the original matrix unchanged.
Refresh512 reaches134,859 /134,061, final windows28,223 /28,227; controls reach
109,249 /109,110. All four new cases pass. Together with the previously analyzed
seeds0/2, the same frozen v30 refresh policy has a four-seed mean **134,487.5**,
range134,061–134,859. Seeds4/5 and the complete repeated0/2 pairs remain running.
This is four-seed evidence, not the six-seed goal.
[Subset](results/flow-refresh-confirm-first-v30/),
[combined four-seed record](results/flow-refresh-four-seed-v30.json).


## Shorter refresh interval and warm reuse

Full8898655/analysis8898657 completes all six cases, with exact refresh512 controls
under the six-way allocation. Every case passes1-second decisions and32GB RSS.

| Configuration | Seed0 | Seed2 | Two-seed mean |
|---|---:|---:|---:|
| Refresh512 | 134,511 | 134,519 | 134,515 |
| Refresh256 | 134,052 | 134,187 | 134,119.5 |
| Refresh512 + warm reuse | 134,997 | 134,794 | 134,895.5 |

Refreshing twice as often loses0.294% by paired means. Warm reuse adds0.283%,
positive on both seeds, but remains only a small two-seed effect. It has not passed
the broader confirmation applied to ordinary refresh. No new single-run record.
[Full evidence](results/flow-refresh-followup-v30/).


## Six-seed confirmation complete

The full matched v30 matrix8898647 /8898648 passes all twelve cases. Refreshed
flow scores134511 /134859 /134519 /134061 /134626 /134966 on seeds0–5,
mean134,590.333; matched no-flow mean109,173.000.
The refreshed mean is23.282% higher, and every
paired seed improves. Final windows stay between28167 and28368 completed tasks.
All four previously analyzed refreshed trajectories and the three established
no-flow controls reproduce exactly. All complete entries are below one second
and process RSS stays below32GB. This confirms this policy over six seeds; it
does not meet the152981-task leader target.

[Six-seed scores, reproducibility and resource measurements](results/flow-refresh-six-seed-v30.json),
[full independent analysis](results/flow-refresh-confirm-v30/).
