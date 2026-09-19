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
an absolute deadline overrun still fails the whole decision. Full seed-0 results
are now complete; six-seed validation remains outstanding.


## Full seed-0 results

All four profiles complete 5,000 steps with zero errors/timeouts, every entry
sample below one second, and peak process RSS below 32 decimal GB. They run on
research38, on two disjoint physical EPYC 9354 cores without a CPU quota.
Warm starts and regional repair remain off. The compact 4M control exactly
preserves the original v8 full trajectory.

| Profile | Tasks | Final 1,000 steps | Max entry seconds | Peak RSS bytes | Wall seconds | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|---:|
| No flow, 4M control | 109,244 | 22,456 | 0.899637074 | 11,808,178,176 | 1,193.844 | 850 |
| Flow 1, no margin, freeze 128 | 46,186 | 4,017 | 0.881542455 | 11,787,059,200 | 1,079.368 | 4,234 |
| Flow 1, margin 50, freeze 128 | **122,896** | **25,254** | **0.881397952** | **12,105,699,328** | **1,300.347** | **743** |
| Flow 1, margin 50, freeze 1,024 | 110,649 | 22,655 | 0.873728801 | 12,131,840,000 | 1,268.023 | 850 |

Margin 50 at freeze 128 adds 13,652 tasks (+12.50%) over the paired 4M control
and exceeds the earlier regional best of 112,164 by 10,732 (+9.57%). It remains
30,085 tasks below the 152,981 local leader reference and has only one seed.
Its final-window gain and improved age tail argue against an early burst or
starvation tradeoff in this run. They do not establish repeatability.

The motion change is consistent with more efficient loaded travel: turns fall
from 3,675,264 to 2,695,199, waits from 2,772,123 to 2,700,542, and forward moves
away from the spatial goal from 3,183,258 to 1,970,601. In contrast, the no-margin
field deteriorates throughout the run, ending at 4,017 tasks per 1,000 steps,
with 14,563,439 loaded waits and age p90 4,234. Bounding its repair work prevents
a deadline failure but does not repair its bad routing behavior.

[Complete full matrix](results/flow-margin-full-v20/),
[exact full control equivalence](results/flow-work-full-equivalence.json).
Full independent seeds 1/2 and analysis **8898544/8898545** are complete,
using the exact frozen v20-r1 binary and unchanged margin-50 settings.
They disqualify this profile from promotion; see the robustness results below.


Three one-variable follow-ups pass screen **8898549**: margin 25 (max entry
0.771101494 s), margin 75 (0.776795199 s), and strength 2 at margin 50
(0.794536410 s). Each retains freeze 128, minimum samples 8 and the fixed 4M
candidate-work threshold. These 200-step screens do not rank throughput.
Full **8898554**, analysis **8898555**, compares them against the unchanged
margin-50/strength-1 control after corrected-guide jobs **8898550/8898552**.
It retains the same frozen v20-r1 binary, seed 0, 5,000 steps, exact deadline,
two disjoint physical cores and 24 GiB total reservation.
[Screen evidence](results/flow-margin-neighbors-screen-v20/).


## Independent-seed confirmation: not robust

| Seed | Tasks | Five consecutive 1,000-step windows | Max entry seconds | Peak RSS bytes | Age p90 |
|---|---:|---|---:|---:|---:|
| 0 | 122,896 | 21,931 / 25,125 / 25,226 / 25,360 / 25,254 | 0.881397952 | 12,105,699,328 | 743 |
| 1 | 122,195 | 21,867 / 25,033 / 25,186 / 24,992 / 25,117 | 0.865685336 | 11,944,648,704 | 753 |
| 2 | 70,171 | 21,294 / 23,690 / 14,898 / 6,630 / 3,659 | 0.857074394 | 11,790,622,720 | 3,170 |

All runs are valid full results; the loss is not a timeout or partial-solution
artifact. The mean is **105,087.3**, below the established 50k profile's three-seed
107,413.3. Those profiles differ in fixed work as well as flow guidance, so this
is a comparison of complete policies, not a one-variable causal claim. The
margin-50 policy is **not promoted**. Six-seed testing of this fragile setting
is not a priority while more robust alternatives are being evaluated.

Seed 2 has 10,260,146 loaded waits and 7,196,046 loaded turns, versus seed 1's
2,788,268 waits and 2,745,557 turns. Its final-window rate and task-age tail
collapse. At 2,000 steps its cumulative count is only 4.4% behind seed 0; at
5,000 it is 42.9% behind. Even a 2,000-step prefix would conceal much of this
failure. This supports retaining full horizons for new routing-policy claims.

Two strong seeds and one late collapse establish strong seed sensitivity. The
early frozen field is a plausible contributor, but the seed also changes repair
throughout the run. Matched no-flow 4M controls at seeds 1/2 are still needed to
isolate the policy change from the work setting; specific causal edges have not
been established. Online intended-route guidance is running independently.
[Full confirmation](results/flow-margin-confirm-v20/),
[three-seed summary](results/flow-margin-robustness-v20/summary.json).

## Matched no-flow confirmation

The missing same-work controls now complete at **109,249** (seed 1) and
**109,182** (seed 2), both valid over all 5,000 steps. Together with seed 0's
109,244, their mean is **109,225.0**. The frozen margin-50 field averages
105,087.3, **3.79% below this matched control**, with seed-specific effects
+12.50%, +11.85% and -35.73%. All cases use the same frozen v20-r1 binary and
fixed-work settings. The seed-2 collapse is introduced by the flow policy in
this comparison; the responsible edges/feedback mechanism remain unisolated.
The earlier comparison to a different 50k work policy is no longer the only
available control. No promotion is justified.
[Matched comparison](results/flow-margin-matched-controls-v20.json),
[complete confirmations](results/work4m-confirm-v20/).

An optional cumulative refresh experiment is now built and screen-validated;
its full seed comparisons are pending. See [FLOW_REFRESH.md](FLOW_REFRESH.md).


## Nearby parameters: full seed-0 result

Matrix 8898554 and analysis 8898555 complete successfully. All four cases have
all 5,000 movement/timing samples, no errors or timeouts, and RSS below 32 GB.
The repeated margin50 strength1 control has exactly the earlier full trajectory
fingerprint, not merely the same score.

| Profile | Tasks | Last 1,000 | Max entry (s) | Age p90 |
|---|---:|---:|---:|---:|
| strength1 margin50 control | 122,896 | 25,254 | 0.860278304 | 743 |
| strength1 margin25 | 55,344 | 4,064 | 0.903567274 | 3,826 |
| strength1 margin75 | 119,528 | 24,388 | 0.901415214 | 779 |
| strength2 margin50 | 119,294 | 24,593 | 0.874671928 | 770 |

None improves the control. Margin25 looks only 2.84% worse at step 1,000 but
finishes 54.97% worse, again rejecting short prefixes as quality rankings.
Margin75 and strength2 have stable late seed-0 rates but no multi-seed confirmation.
These settings are not promoted. The independent [flow-field audit](FLOW_FIELD_AUDIT.md)
finds connected neutral-cost graphs and pronounced late movement churn in the
original failing seed; it does not identify a unique causal edge.

[Complete evidence](results/flow-margin-neighbors-full-v20/).
