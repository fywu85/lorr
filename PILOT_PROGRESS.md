# PILOT competition progress

Updated 2026-09-21 18:46 UTC. **PILOT** means **Pipelined Look-ahead with Task matching**.
It is the independent planner/scheduler developed from the colleague's log,
with pipelined PIBT and parallel look-ahead for crowded traffic, plus optional
windowed LNS for lighter traffic. Its results are separate from CGAR.

Current development covers RANDOM-01 through RANDOM-05. The other five
competition instances remain placeholders for future work. NMS is the target;
throughput is primary, with order waiting times tracked as a secondary metric.

**Resumed on 2026-09-21:** push throughput across all five RANDOM instances.

**Qualification milestones:** RANDOM-03 at least **2,595** tasks and RANDOM-04 at least
**2,838**, each 10% above matched local NMS, with robust subsecond runtime.
Selected configurations must pass repeated full runs and fresh-input checks.
[Campaign and qualification rules](random05/RANDOM34_CAMPAIGN.md).

Selected scores are complete combined-track runs with enforced **1,000 ms**
entry limits, **30 s** preprocessing, and a **32 decimal GB** process limit.
These rows use **16 physical EPYC9354 cores / 32 SMT workers**, with bound
affinity and no CPU quota on shared GRID hosts. A deadline overrun fails the
run; PILOT completes its declared fixed work instead of returning a partial search.

| Instance | PILOT tasks | Published max(NMS, KK) | Reference | Difference | Profile | Seed | Max step (ms) |
|---|---:|---:|---|---:|---|---:|---:|
| WAREHOUSE | — | 154,795 | NMS | — | Not evaluated | — | — |
| SORTATION | — | 152,714 | NMS | — | Not evaluated | — | — |
| CITY-01 | — | 8,445 | KK | — | Not evaluated | — | — |
| CITY-02 | — | 16,997 | KK | — | Not evaluated | — | — |
| GAME | — | 23,274 | NMS | — | Not evaluated | — | — |
| RANDOM-01 | 729 | 688 | KK | +5.96% | TRICK | 4 | 100.09 |
| RANDOM-02 | 1,408 | 1,260 | KK | +11.75% | TRICK | 2 | 492.79 |
| RANDOM-03 | 2,606 | 2,334 | NMS | +11.65% | TRICK | 5 | 731.22 |
| RANDOM-04 | 2,782 | 2,547 | NMS | +9.23% | TRICK | 4 | 739.55 |
| RANDOM-05 | 4,242 | 3,050 | NMS | +39.08% | TRICK | 0 | 780.49 |

**Published scores are historical targets, not matched local baselines.**
This table uses the stronger published result from NMS and Kitty Knight.
KK sets the RANDOM-01/02 references; NMS sets RANDOM-03/04/05.
NMS reported timeout labels for WAREHOUSE, SORTATION and GAME are preserved
in the [target snapshot](random05/references/published-nms-kk-combined-2024.json).
Matched local Kitty Knight runs are not yet available; the retained qualification
targets below continue to use matched local NMS.
A dash means no valid PILOT throughput result, not zero completed tasks.
The frozen large-map distance representation was estimated at 95–189 GB,
so those maps are deferred; no large-map throughput is claimed.
[Capacity assessment](random05/GENERALIZATION.md#large-map-limits).

The primary local comparison uses identical archived inputs and matched CPU
allocations. All selected runs pass independent movement, collision,
assignment and task-event replay checks.

| Instance | PILOT | Matched local NMS32 | Difference |
|---|---:|---:|---:|
| RANDOM-01 | 729 | 649 | +12.33% |
| RANDOM-02 | 1,408 | 1,228 | +14.66% |
| RANDOM-03 | 2,606 | 2,359 | +10.47% |
| RANDOM-04 | 2,782 | 2,580 | +7.83% |
| RANDOM-05 | 4,242 | 3,172 | +33.73% |

These are selected individual bests, not an average or one universal preset.
GENERAL means no map-specific guidance or known-horizon rule was enabled;
the general algorithm settings were still selected during development.
TRICK results require the corresponding `--trick INSTANCE` flag.
Selected tuned guidance: RANDOM-03, RANDOM-04, RANDOM-05.
Selected known-horizon rules: RANDOM-01, RANDOM-02, RANDOM-03, RANDOM-04, RANDOM-05.
Selected task-admission caps: RANDOM-04=560, RANDOM-05=680. Opened tasks remain protected; all robots remain movable.
All five RANDOM cases share one layout: this is density transfer,
not unseen-map validation.

| Instance | General best | Explicit-trick best |
|---|---:|---:|
| WAREHOUSE | — | — |
| SORTATION | — | — |
| CITY-01 | — | — |
| CITY-02 | — | — |
| GAME | — | — |
| RANDOM-01 | 726 | 729 |
| RANDOM-02 | 1,397 | 1,408 |
| RANDOM-03 | 1,582 | 2,606 |
| RANDOM-04 | 1,558 | 2,782 |
| RANDOM-05 | 2,226 | 4,242 |

Current selected records are pinned to their completion timestamps and source commits:

| Instance | Completed UTC | Source | Full-run evidence |
|---|---|---|---|
| RANDOM-01 | 2026-09-21T15:05:19.829832+00:00 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Run](random05/results/random12-resume-horizon-split-full-v132/trick-random-01-resume-horizon-0p75/summary.json) |
| RANDOM-02 | 2026-09-21T15:54:03.792427+00:00 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Run](random05/results/random12-resume-cutoff-split-full-v132/trick-random-02-resume-cutoff-0p875/summary.json) |
| RANDOM-03 | 2026-09-21T16:52:42.436655+00:00 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Run](random05/results/random03-field-price-split-full-v132/trick-random-03-field-price-physical0p25/summary.json) |
| RANDOM-04 | 2026-09-21T17:35:31.545879+00:00 | [88551e69](https://github.com/fywu85/lorr/commit/88551e69) | [Run](random05/results/random45-progress-triage-split-full-v144/trick-random-04-progress-triage-mixp25-span32/summary.json) |
| RANDOM-05 | 2026-09-21T18:11:42.053845+00:00 | [88551e69df5b6f5ee14600dfe3a7ae8fe586783c](https://github.com/fywu85/lorr/commit/88551e69df5b6f5ee14600dfe3a7ae8fe586783c) | [Run](random05/results/random05-startup-progress-split-full-v144/trick-random-05-startup-progress-mixp125/summary.json) |

**RANDOM-03 has crossed the archived ten-percent target:** 2,602 versus
2,359 matched NMS (+10.30%). The frozen fresh-input comparison gives
2,599/2,557 versus the stronger NMS repetitions 2,327/2,343:
**+10.41% aggregate**, with individual gains +11.69% and +9.13%.
All eight fresh runs passed timing/resource checks and independent replay.
The candidate stays below 701 ms on both fresh inputs; its archived exact
repeat and two other planner seeds peak below 675 ms.
The later 2,606-task routing-cost refinement is a development best;
the preceding fresh-input validation does not qualify that change.
The 2,606 profile repeats exactly. Paired seeds5/0/3 score2,606/2,572/2,590
versus2,602/2,548/2,566, +0.674% in aggregate and positive on each;
all pass full replay and peak below767ms.
RANDOM-04 currently reaches **2,782** (+7.83% above matched NMS),
**56 tasks short** of 2,838. Its record peaks at 739.6 ms;
The earlier 2,777-task profile repeated exactly. Eight planner seeds score
2,726–2,777; all original, repeat and seed checks peak below 791 ms.
New frozen task/start checks on RANDOM-01 and RANDOM-02 also pass:
selected profiles beat the stronger NMS repeats by11.53% and13.23% in
aggregate; general profiles are ahead by10.98% and12.57%. Every individual
input is positive, and all16full runs pass timing/resource/replay checks.
These two streams per density use the same layout, not unseen maps.
[RANDOM-01](random05/RANDOM01_FRESH_VALIDATION_V1.md),
[RANDOM-02](random05/RANDOM02_FRESH_VALIDATION_V1.md).

The new2,782 record uses task-specific observed progress to adjust the
known-horizon estimate. Exact repetition passes; four planner seeds improve
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

The current4,242 record adds observed-progress correction0.125/span32;
mean589.171/max780.487ms, RAM489.562MB. It repeats exactly; four seeds
score4,242/4,028/4,183/4,128 (+0.157% over4,236's profile, three gains
and one loss; still0.635% below4,197's aggregate). Fresh validation is pending.
[Evidence](random05/results/random05-startup-progress-split-full-v144/audit.json).

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
The current 32-worker archived record averages 589 ms per step, peaks at
780 ms, and uses 490 MB peak RSS. Its longest completed order takes 1,983
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
