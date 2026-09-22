# PILOT competition progress

Updated 2026-09-22 01:52 UTC. **PILOT** means **Pipelined Look-ahead with Task matching**.
It is the independent planner/scheduler developed from the colleague's log,
with pipelined PIBT and parallel look-ahead for crowded traffic, plus optional
windowed LNS for lighter traffic. Its results are separate from CGAR.

Current development covers RANDOM-01 through RANDOM-05. The other five
competition instances remain placeholders for future work. The reference is
**max(NMS, Kitty Knight)** for each instance;
throughput is primary, with order waiting times tracked as a secondary metric.

**Active objective:** maximize verified throughput across all five RANDOM instances.
[Full goal and constraints](random05/ACTIVE_GOAL.md). RANDOM-03/04 are milestones within this campaign.

**Current qualification milestones:** RANDOM-03 at least **2,595** tasks and RANDOM-04
at least **2,914**, each 10% above matched max(NMS,KK), with robust subsecond runtime.
Selected configurations must pass repeated full runs and fresh-input checks.
[Campaign and qualification rules](random05/RANDOM34_CAMPAIGN.md).

Selected scores are complete combined-track runs with enforced **1,000 ms**
entry limits, **30 s** preprocessing, and a **32 decimal GB** process limit.
These rows use **16 physical EPYC9354 cores / 32 SMT workers**, with bound
affinity and no CPU quota on shared GRID hosts. A deadline overrun fails the
run; PILOT completes its declared fixed work instead of returning a partial search.

| Instance | PILOT tasks | Matched max(NMS, KK) | Reference | Difference | Profile | Seed | Max step (ms) |
|---|---:|---:|---|---:|---|---:|---:|
| WAREHOUSE | — | — | — | — | Not evaluated | — | — |
| SORTATION | — | — | — | — | Not evaluated | — | — |
| CITY-01 | — | — | — | — | Not evaluated | — | — |
| CITY-02 | — | — | — | — | Not evaluated | — | — |
| GAME | — | — | — | — | Not evaluated | — | — |
| RANDOM-01 | 739 | 692 | KK | +6.79% | TRICK | 1 | 114.53 |
| RANDOM-02 | 1,408 | 1,256 | KK | +12.10% | TRICK | 2 | 492.79 |
| RANDOM-03 | 2,646 | 2,359 | NMS | +12.17% | TRICK | 3 | 676.51 |
| RANDOM-04 | 2,782 | 2,649 | NMS | +5.02% | TRICK | 4 | 739.55 |
| RANDOM-05 | 4,254 | 3,172 | NMS | +34.11% | TRICK | 0 | 874.30 |

**Headline comparisons use the stronger matched local result from NMS and Kitty Knight.**
KK sets the RANDOM-01/02 references; NMS sets RANDOM-03/04/05.
Both teams have valid measurements on all five RANDOM instances.
RANDOM-05 KK uses its unchanged binary with `MALLOC_ARENA_MAX=2`;
both allocator-only repeats score2,085 and pass strict limits and replay.
Original virtual-address exhaustion failures remain in the [baseline audit](random05/NMS_KK_COMPARISON.md).
Retain the strongest historical and new valid baseline; a missing team is never zero.
[Comparison policy](random05/COMPARISON_POLICY.md).
A dash means no valid PILOT throughput result, not zero completed tasks.
The frozen large-map distance representation was estimated at 95–189 GB,
so those maps are deferred; no large-map throughput is claimed.
[Capacity assessment](random05/GENERALIZATION.md#large-map-limits).

The local comparisons use identical archived inputs and matched CPU
allocations. All selected runs pass independent movement, collision,
assignment and task-event replay checks.

| Instance | PILOT | Local NMS32 | Local KK32 | Matched max(NMS, KK) | Difference |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 | 739 | 649 | 692 | 692 | +6.79% |
| RANDOM-02 | 1,408 | 1,233 | 1,256 | 1,256 | +12.10% |
| RANDOM-03 | 2,646 | 2,359 | 2,110 | 2,359 | +12.17% |
| RANDOM-04 | 2,782 | 2,649 | 1,472 | 2,649 | +5.02% |
| RANDOM-05 | 4,254 | 3,172 | 2,085 | 3,172 | +34.11% |

**Published scores below are historical orientation, not matched local comparisons.**
NMS reported timeout labels for WAREHOUSE, SORTATION and GAME are preserved
in the [target snapshot](random05/references/published-nms-kk-combined-2024.json).

| Instance | PILOT | Published max(NMS, KK) | Reference | Difference |
|---|---:|---:|---|---:|
| WAREHOUSE | — | 154,795 | NMS | — |
| SORTATION | — | 152,714 | NMS | — |
| CITY-01 | — | 8,445 | KK | — |
| CITY-02 | — | 16,997 | KK | — |
| GAME | — | 23,274 | NMS | — |
| RANDOM-01 | 739 | 688 | KK | +7.41% |
| RANDOM-02 | 1,408 | 1,260 | KK | +11.75% |
| RANDOM-03 | 2,646 | 2,334 | NMS | +13.37% |
| RANDOM-04 | 2,782 | 2,547 | NMS | +9.23% |
| RANDOM-05 | 4,254 | 3,050 | NMS | +39.48% |

These are selected individual bests, not an average or one universal preset.
GENERAL means no map-specific guidance or known-horizon rule was enabled;
the general algorithm settings were still selected during development.
TRICK results require the corresponding `--trick INSTANCE` flag.
Selected tuned guidance: RANDOM-03, RANDOM-04, RANDOM-05.
Selected known-horizon rules: RANDOM-01, RANDOM-02, RANDOM-03, RANDOM-04, RANDOM-05.
Selected task-admission caps: RANDOM-04=560, RANDOM-05=680. Opened tasks remain protected; all robots remain movable.
All five RANDOM cases share one layout: this is density transfer,
not unseen-map validation.

| Instance | General best | General vs max | Explicit-trick best | Trick vs max | Matched max(NMS, KK) |
|---|---:|---:|---:|---:|---:|
| WAREHOUSE | — | — | — | — | — |
| SORTATION | — | — | — | — | — |
| CITY-01 | — | — | — | — | — |
| CITY-02 | — | — | — | — | — |
| GAME | — | — | — | — | — |
| RANDOM-01 | 727 | +5.06% | 739 | +6.79% | 692 |
| RANDOM-02 | 1,397 | +11.23% | 1,408 | +12.10% | 1,256 |
| RANDOM-03 | 1,634 | -30.73% | 2,646 | +12.17% | 2,359 |
| RANDOM-04 | 1,616 | -39.00% | 2,782 | +5.02% | 2,649 |
| RANDOM-05 | 2,226 | -29.82% | 4,254 | +34.11% | 3,172 |

Current selected records are pinned to their completion timestamps and source commits:

| Instance | Completed UTC | Source | Full-run evidence |
|---|---|---|---|
| RANDOM-01 | 2026-09-22T00:43:24.293402+00:00 | [4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad](https://github.com/fywu85/lorr/commit/4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad) | [Run](random05/results/random1234-progress-transfer-split-full-v162/trick-random-01-progress-transfer-mixp25-span32/summary.json) |
| RANDOM-02 | 2026-09-21T15:54:03.792427+00:00 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Run](random05/results/random12-resume-cutoff-split-full-v132/trick-random-02-resume-cutoff-0p875/summary.json) |
| RANDOM-03 | 2026-09-22T00:53:28.091236+00:00 | [4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad](https://github.com/fywu85/lorr/commit/4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad) | [Run](random05/results/random1234-progress-transfer-split-full-v162/trick-random-03-progress-transfer-mixp25-span32/summary.json) |
| RANDOM-04 | 2026-09-21T17:35:31.545879+00:00 | [88551e69](https://github.com/fywu85/lorr/commit/88551e69) | [Run](random05/results/random45-progress-triage-split-full-v144/trick-random-04-progress-triage-mixp25-span32/summary.json) |
| RANDOM-05 | 2026-09-22T00:21:04.224019+00:00 | [4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad](https://github.com/fywu85/lorr/commit/4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad) | [Run](random05/results/random345-flow-iterations-split-full-v162/trick-random-05-flow-iterations80/summary.json) |

**RANDOM-03 has crossed the archived ten-percent target:** 2,602 versus
2,359 matched max(NMS,KK) (+10.30%). Its original NMS-only fresh comparison gives
2,599/2,557 versus the stronger NMS repetitions 2,327/2,343:
**+10.41% aggregate**, with individual gains +11.69% and +9.13%.
KK was not measured on those V1 streams; this is not a fresh two-team maximum.
All eight fresh runs passed timing/resource checks and independent replay.
The candidate stays below 701 ms on both fresh inputs; its archived exact
repeat and two other planner seeds peak below 675 ms.
The earlier 2,606-task routing-cost refinement is a development result;
the preceding fresh-input validation does not qualify that change.
The 2,606 profile repeats exactly. Paired seeds5/0/3 score2,606/2,572/2,590
versus2,602/2,548/2,566, +0.674% in aggregate and positive on each;
all pass full replay and peak below767ms.
The later heuristic-priority search reaches **2,620** and repeats exactly.
Five paired development seeds total13,016 versus12,974 (+0.324%), with
three gains and two losses. The new same-source seed3 weight1 control
failed at1046.391ms; its earlier valid source132 baseline is separately
identified in the [paired report](random05/results/random03-record2620-split-full-v153/paired-comparison.json).
Frozen fresh V2 finishes2,617/2,609 versus the previous search2,612/2,620:
**-0.115% aggregate versus the previous search**, with improved mean
latency455.6/480.2ms versus553.1/545.7ms. The supplemented fresh
max(NMS,KK) comparison is **+8.852%**, below10%. NMS is stronger on both
streams; the four later unmodified KK repeats also pass replay and timing.
[Fresh V2 report](random05/RANDOM03_FRESH_VALIDATION_V2.md).
RANDOM-01 reaches739 with observed-progress horizon correction0.25/span32,
added to the735profile. The exact repeat passes all six fields. Five paired
planner seeds736/739/726/732/732 versus732/735/725/729/730 give+0.3835%,
all positive. Maximum qualification entry118.656ms. This is a modest
development gain; fresh01V1still qualifies the earlier729profile only.
[Paired evidence](random05/results/random01-record739-split-full-v162/REPORT.md).

The preceding2,621 physical-deadline gate record repeats exactly. Five paired
development seeds total13,029 versus12,999 (+0.231%, four gains/one loss).
The gate alone has not received standalone fresh-input qualification.
[Paired evidence](random05/results/random03-record2621-split-full-v162/paired-seeds.json).

The2,646 selected result adds observed-progress horizon correction
0.25/span32 to the2,621profile. Full replay and strict limits pass:
mean484.103/max676.507ms, RSS469.467MB. The unchanged2,621control is
exact in all six fields, as is the2,646repeat. Five paired planner seeds
2611/2606/2611/2646/2610 total13,084versus13,029 (+0.4221%, all positive).
Maximum qualification entry730.523ms. Selectingseed3is included.
[Paired evidence](random05/results/random03-record2646-split-full-v162/REPORT.md).

**Frozen fresh V3 validates the2,646 profile on two new task/start inputs:**
2,660/2,622 versus the prior2,620 recipe2,639/2,612 (+0.590% aggregate,
positive on both). Both teams were frozen in the original protocol:
NMS2,461/2,444 and2,361/2,380; KK2,084/2,093 and2,094/2,130.
**+9.110% aggregate versus max(NMS,KK)**, with individual gains8.086%/10.168%.
This still misses the10% fresh-input milestone. All12runs pass strict
source/input/runtime/bound-affinity checks and independent full replay.
Candidate mean494.489/478.907ms, max727.685/677.620ms; RSS below465MB.
These are same-layout validation streams, not new archived records.
[Fresh V3 report](random05/RANDOM03_FRESH_VALIDATION_V3.md).

RANDOM-04 currently reaches **2,782** (+5.02% above matched max(NMS,KK)),
**132 tasks short** of2,914. Its record peaks at739.6ms;
The earlier 2,777-task profile repeated exactly. Eight planner seeds score
2,726–2,777; all original, repeat and seed checks peak below 791 ms.
Frozen RANDOM-01/02 task/start checks are supplemented with KK repeats.
The frozen selected profiles beat **max(NMS,KK) by4.37% and10.17%** in
aggregate; their frozen general controls are ahead by3.86% and9.52%.
Every input is positive. The later general R01record727 is not yet fresh-validated.
These two streams per density use the same layout, not unseen maps.
[RANDOM-01](random05/RANDOM01_FRESH_VALIDATION_V1.md),
[RANDOM-02](random05/RANDOM02_FRESH_VALIDATION_V1.md).

The new2,782 record uses task-specific observed progress to adjust the
known-horizon estimate. Exact repetition passes; the new profile improves
the eight-seed aggregate by0.340% (six gains, two losses). Seed11's later
extension fails the strict first-step deadline; it has no valid score.
[Replication](random05/results/random124-seed-extension-split-full/paired-comparison.json).
Fresh RANDOM-04 validation is still pending.
The earlier 2,661-task configuration passed four planner seeds and exact
repetitions below 491 ms; a later equivalent source control peaked at
755.1 ms. That slower control remains in the timing evidence.
[Full timing evidence](random05/results/random34-runtime/REPORT.md).
[Frozen RANDOM-03 comparison](random05/RANDOM03_FRESH_VALIDATION_V1.md).


**The 4,175-task admission profile passes frozen fresh validation V6:**
4,182/4,177 versus the stronger NMS repetitions 3,155/3,178,
**+31.99% in aggregate** and +5.25% over the preceding 4,011 recipe.
All eight runs pass strict timing/resource checks and independent replay;
candidate maxima are 774/799 ms. Its archived exact repeat and four
planner seeds also pass. This validates the frozen 4,175 profile,
not the later cutoff/startup refinements or unseen layouts.
KK was not measured on those V6 streams; the31.99% is NMS-only.
[Frozen V6 comparison](random05/FRESH_VALIDATION_V6.md).
The 4,197 profile now repeats exactly in all six trajectory fields;
planner seeds0/1/2/3 score4,197/4,168/4,179/4,143. All full qualification
runs pass independent replay, with maximum870.036ms and RSS below491MB.
[Qualification](random05/results/random05-record4197-split-full-v132/audit.json).

The preceding4,236 selected best adds mild startup task weighting and repeats exactly.
Seeds0/1/2/3 score4,236/4,051/4,172/4,096: the aggregate is0.791% below
the preceding4,197 profile, so this is a selected-seed record, not a
replicated improvement. Fresh validation remains pending.
[Replication](random05/results/random05-record4236-split-full-v132/paired-comparison.json).

The preceding4,242 record adds observed-progress correction0.125/span32;
mean589.171/max780.487ms, RAM489.562MB. It repeats exactly; four seeds
score4,242/4,028/4,183/4,128 (+0.157% over4,236's profile, three gains
and one loss; still0.635% below4,197's aggregate). Fresh validation is pending.
[Evidence](random05/results/random05-startup-progress-split-full-v144/audit.json).

The new4,254 selected record refines traffic-assignment guidance with80
updates instead of20, keeping the same per-step work: mean579.781ms,
max874.301ms, RSS490.107MB. All2,000steps pass independent replay and
strict limits. The exact repeat matches all six trace fields. Four planner
seeds4254/4189/4252/4253 improve all paired controls, +2.2134%aggregate,
and average4237.0. Maximum qualification entry919.594ms; all pass.
Selectingseed0is included; no fresh-input qualification is claimed.
[Paired evidence](random05/results/random05-record4254-split-full-v162/REPORT.md).

**The earlier RANDOM-05 4,000-task milestone is independently verified.** The archived
4,011-task configuration repeated exactly. Frozen fresh task/start inputs
50009 and 50010 produced 3,912 and 4,047 versus the stronger NMS repetitions
3,095 and 3,190: **+26.63% in aggregate**. The improvement over the immediately
previous configuration is +0.442%, positive on both fresh inputs. The fresh
4,047 belongs to a different input. This validation applies to the frozen4,011
configuration; see the detailed history for qualification of newer records.
All eight original fresh runs passed strict timing, resource and replay checks.
[Frozen comparison](random05/FRESH_VALIDATION_V5.md),
[milestone audit](random05/results/completion-audit-4000/audit.json).

The four-core RANDOM-05 record stays separate: **3,770 versus matched NMS4
2,914 (+29.4%)**. Its earlier frozen fresh comparison was +25.42%.
The current 32-worker archived record averages 580 ms per step, peaks at
874 ms, and uses 490 MB peak RSS. Its longest completed order takes 1,973
steps; some initial orders remain unfinished at 2,000, so the eventual
maximum wait is unknown. Throughput, rather than fairness, selected these runs.
[Completed and censored waits](random05/WAITING_PROGRESS.md),
[fresh-input waits](random05/results/fresh-validation-v5/WAITING.md).

Windowed LNS provides the current general gains on RANDOM-01 and RANDOM-02.
The 1,397 general RANDOM-02 record uses paired repair orders. Seven
development seeds improve +0.36% in aggregate over one order at the same
potential repair work; five improve, one ties, one loses. All pass full
replay below739ms; the selected best repeats exactly. No horizon rule or
guidance is enabled in that general profile.
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

Recent exact controls reduce runtime without changing the selected actions:
the shared task-ranking cache preserves 2,634 tasks on RANDOM-04 at a
measured 447 ms mean / 485 ms maximum. The four-way A-star heap preserves
2,551 and 2,552 on RANDOM-03, with mean reductions of about 4–7%; maxima
vary across those comparisons. These are measured full-run results, not
a universal latency guarantee. Strict timing failures remain in the logs.
[Runtime evidence](random05/results/random34-runtime/REPORT.md),
[heap comparisons](random05/results/random03-heap-runtime-split-full-v97/audit.json).

[Detailed RANDOM-05 throughput history](RANDOM05_PROGRESS.md) remains the
authoritative RANDOM-05 log, as requested. This file is the general PILOT
dashboard and will gain non-RANDOM entries when valid results exist.
[Full density experiment history](random05/RANDOM_PROGRESS.md),
[exact configurations](random05/random-frontiers.json),
[continuation notes](random05/NEXT.md).

Refresh this dashboard after promoting a verified record with
`python3 random05/tools/render_pilot_progress.py`. Evidence and historical
failures stay in the detailed logs; missing map results remain placeholders.
