# PILOT competition progress

Updated 2026-09-21 06:13 UTC. **PILOT** means **Pipelined Look-ahead with Task matching**.
It is the independent planner/scheduler developed from the colleague's log,
with pipelined PIBT and parallel look-ahead for crowded traffic, plus optional
windowed LNS for lighter traffic. Its results are separate from CGAR.

Current development covers RANDOM-01 through RANDOM-05. The other five
competition instances remain placeholders for future work. NMS is the target;
throughput is primary, with order waiting times tracked as a secondary metric.

**Active target:** RANDOM-03 at least **2,595** tasks and RANDOM-04 at least
**2,838**, each 10% above matched local NMS, with robust subsecond runtime.
Selected configurations must pass repeated full runs and fresh-input checks.
[Campaign and qualification rules](random05/RANDOM34_CAMPAIGN.md).

Selected scores are complete combined-track runs with enforced **1,000 ms**
entry limits, **30 s** preprocessing, and a **32 decimal GB** process limit.
These rows use **16 physical EPYC9354 cores / 32 SMT workers**, with bound
affinity and no CPU quota on shared GRID hosts. A deadline overrun fails the
run; PILOT completes its declared fixed work instead of returning a partial search.

| Instance | PILOT tasks | Published NMS | Difference | Profile | Seed | Max step (ms) |
|---|---:|---:|---:|---|---:|---:|
| WAREHOUSE | — | 154,795 | — | Not evaluated | — | — |
| SORTATION | — | 152,714 | — | Not evaluated | — | — |
| CITY-01 | — | 8,420 | — | Not evaluated | — | — |
| CITY-02 | — | 16,787 | — | Not evaluated | — | — |
| GAME | — | 23,274 | — | Not evaluated | — | — |
| RANDOM-01 | 726 | 639 | +13.62% | GENERAL | 4 | 98.23 |
| RANDOM-02 | 1,376 | 1,221 | +12.69% | GENERAL | 5 | 451.06 |
| RANDOM-03 | 2,551 | 2,334 | +9.30% | TRICK | 5 | 698.35 |
| RANDOM-04 | 2,565 | 2,547 | +0.71% | TRICK | 4 | 948.62 |
| RANDOM-05 | 4,011 | 3,050 | +31.51% | TRICK | 0 | 621.75 |

**Published NMS scores are historical targets, not matched local baselines.**
Their reported timeout labels for WAREHOUSE, SORTATION and GAME are preserved
in the [target snapshot](random05/references/published-nms-combined-2024.json).
A dash means no valid PILOT throughput result, not zero completed tasks.
The frozen large-map distance representation was estimated at 95–189 GB,
so those maps are deferred; no large-map throughput is claimed.
[Capacity assessment](random05/GENERALIZATION.md#large-map-limits).

The primary local comparison uses identical archived inputs and matched CPU
allocations. All selected runs pass independent movement, collision,
assignment and task-event replay checks.

| Instance | PILOT | Matched local NMS32 | Difference |
|---|---:|---:|---:|
| RANDOM-01 | 726 | 649 | +11.86% |
| RANDOM-02 | 1,376 | 1,228 | +12.05% |
| RANDOM-03 | 2,551 | 2,359 | +8.14% |
| RANDOM-04 | 2,565 | 2,580 | -0.58% |
| RANDOM-05 | 4,011 | 3,172 | +26.45% |

These are selected individual bests, not an average or one universal preset.
GENERAL means no map-specific guidance or known-horizon rule was enabled;
the general algorithm settings were still selected during development.
TRICK results require the corresponding `--trick INSTANCE` flag.
Selected tuned guidance: RANDOM-03, RANDOM-04, RANDOM-05.
Selected known-horizon rules: RANDOM-03, RANDOM-04, RANDOM-05.
All five RANDOM cases share one layout: this is density transfer,
not unseen-map validation.

| Instance | General best | Explicit-trick best |
|---|---:|---:|
| WAREHOUSE | — | — |
| SORTATION | — | — |
| CITY-01 | — | — |
| CITY-02 | — | — |
| GAME | — | — |
| RANDOM-01 | 726 | 628 |
| RANDOM-02 | 1,376 | 1,122 |
| RANDOM-03 | 1,582 | 2,551 |
| RANDOM-04 | 1,558 | 2,565 |
| RANDOM-05 | 2,226 | 4,011 |

Current selected records are pinned to their completion timestamps and source commits:

| Instance | Completed UTC | Source | Full-run evidence |
|---|---|---|---|
| RANDOM-01 | 2026-09-21T02:53:18.408890+00:00 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Run](random05/results/random-window-followup-split-full-v80/general-random-01-window-selected-seed4/summary.json) |
| RANDOM-02 | 2026-09-21T04:01:36.001191+00:00 | [4b70a80](https://github.com/fywu85/lorr/commit/4b70a80) | [Run](random05/results/random-window2343-followup-split-full-v81/general-random-02-window-h20-i2048-keep10/summary.json) |
| RANDOM-03 | 2026-09-21T06:01:05.809867+00:00 | [dabfcc7](https://github.com/fywu85/lorr/commit/dabfcc7) | [Run](random05/results/random03-cooperative-triage-split-full-v89/trick-random-03-cooperative-triage-triage100/summary.json) |
| RANDOM-04 | 2026-09-21T05:05:35.466739+00:00 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Run](random05/results/random04-depth16-validation-split-full-v80/trick-random-04-depth16-k14400-seed4/summary.json) |
| RANDOM-05 | 2026-09-21T03:50:40.471914+00:00 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Run](random05/results/record3990-coupling-split-full-v80/32-record3990-mix1/summary.json) |

**RANDOM-05's 4,000-task milestone is independently verified.** The archived
4,011-task configuration repeated exactly. Frozen fresh task/start inputs
50009 and 50010 produced 3,912 and 4,047 versus the stronger NMS repetitions
3,095 and 3,190: **+26.63% in aggregate**. The improvement over the immediately
previous configuration is +0.442%, positive on both fresh inputs. The fresh
4,047 belongs to a different input and does not replace the archived 4,011.
All eight original fresh runs passed strict timing, resource and replay checks.
[Frozen comparison](random05/FRESH_VALIDATION_V5.md),
[milestone audit](random05/results/completion-audit-4000/audit.json).

The four-core RANDOM-05 record stays separate: **3,770 versus matched NMS4
2,914 (+29.4%)**. Its earlier frozen fresh comparison was +25.42%.
The current 32-worker archived record averages 534 ms per step, peaks at
622 ms, and uses 559 MB peak RSS. Its longest completed order takes 1,944
steps; some initial orders remain unfinished at 2,000, so the eventual
maximum wait is unknown. Throughput, rather than fairness, selected these runs.
[Completed and censored waits](random05/results/task-waiting-frontiers-20260920T1612/REPORT.md),
[fresh-input waits](random05/results/fresh-validation-v5/WAITING.md).

Windowed LNS provides the current general gains on RANDOM-01 and RANDOM-02.
On guided RANDOM-03, keeping 14 instead of 10 previous plan steps improves
all three paired planner seeds by 3.27% in aggregate. Reactive planning
remains stronger on the two crowded cases; the windowed transfer trials
there were substantially worse. RANDOM-04 is still close to the local NMS
baseline; improving throughput and timing headroom there is the next priority.
An earlier depth16 profile reached 2,565 on planner seed4, while seed6
gives 2,536. Seeds0 and3 fail the strict deadline (1,197 and 1,007 ms);
the original failures remain recorded. Depth18 gives 2,525. An invalid
depth20 work count is rejected at initialization and has no throughput score.
[All six original outcomes](random05/results/random04-depth16-validation-split-full-v80/audit.json).

[Detailed RANDOM-05 throughput history](RANDOM05_PROGRESS.md) remains the
authoritative RANDOM-05 log, as requested. This file is the general PILOT
dashboard and will gain non-RANDOM entries when valid results exist.
[Full density experiment history](random05/RANDOM_PROGRESS.md),
[exact configurations](random05/random-frontiers.json),
[continuation notes](random05/NEXT.md).

Refresh this dashboard after promoting a verified record with
`python3 random05/tools/render_pilot_progress.py`. Evidence and historical
failures stay in the detailed logs; missing map results remain placeholders.
