# RANDOM-05 combined-track throughput progress

[Current verified result summary](random05/RESULTS.md).

Goal (retargeted 2026-09-20): reach **4,000 completed tasks** on the full archived
RANDOM-05 combined benchmark, then independently validate the selected improvement.
Keep separate four-core and 32-worker records under the existing comparison rules.
The previous target of approaching the colleague's 27–28% matched NMS gain was
met at +25.42% on fresh inputs. Their private absolute counts are not matched
baselines. All prior fresh inputs 50001–50008 remain excluded from tuning.

## Comparison rules

- RANDOM-05: 800 robots, 2,000 simulation steps, combined planner and scheduler.
- Compare both solvers on the same input hashes, horizon and CPU allocation.
- The 32-worker allocation means 16 physical cores with two SMT threads each,
  matching the topology in the archived 2024 evaluation specification. Local
  NMS and recent four-core frontiers use AMD EPYC9354 hosts; the historical
  server used EPYC7R32, so comparisons to published scores remain indirect.
- Record valid completion, planner/scheduler errors, entry timeouts, latency,
  CPU use and peak process RAM. Our implementation must fit within 32 GB.
- Map-specific guidance and known-horizon triage require `--trick RANDOM-05`.
  Reassignment of unopened tasks is a legitimate combined-track scheduling rule.
- Throughput is the primary objective. Track release-to-completion latency and
  unfinished-order ages as secondary metrics in [the waiting log](random05/WAITING_PROGRESS.md).
- Fixed-work screens accelerate development. Full runs determine the frontier.
- Record every new best, including a seed-specific or trick-enabled result,
  alongside its source commit/hash and UTC timestamp. Distinguish that best from
  replicated improvements across seeds.

## Verified local frontier

Updated: 2026-09-21 02:16 UTC.

**Best single run on the archived input: 3,978 tasks on 32 workers / 16 physical cores**,
or **+25.4% versus matched NMS32=3,172**. Source
[acdbfd7](https://github.com/fywu85/lorr/commit/acdbfd7), planner seed0,
firstK7968 thenK16320/B18/s2/q4/G4/E8/P8, move-proposal bias3.
Guidance and the directional horizon cutoff remain explicit tricks;
startup weighting and finalist rescoring are off.
Mean617ms, maximum786ms, RSS560MB; all2,000steps valid and independently replayed.
This is37tasks above3,941. Bias3 averages2.60% higher across four paired development
planner seeds (three positive), including the seed used to select the setting.
This is a selected maximum, not independent task/start validation. The4,000
target remains22tasks away.
[Full evidence](random05/results/move-bias-refinement-split-full-v77/32-move-bias3-b18-seed0/summary.json),
[independent replay](random05/results/move-bias-refinement-split-full-v77/action_audit-3978.json).

The3,933 intermediate record used move bias2; the same source's zero-bias
control exactly reproduces3,928 in all six trajectory fields.

The previous3,928 record used the same B18 search without move-proposal bias.
Across seven paired planner seeds, B18's aggregate is0.27% lower than B14,
with four positive pairs. That earlier selected record did not establish a
mean improvement. [Paired outcomes](random05/results/branch-seeds-split-full-v69/paired-outcomes.json).

The previous3,852 record used scale1.5. A new diagnostic build reproduces its
entire trajectory while recording exact pre-decision snapshots. Four of that
configuration's five original seeds completed; the timed-out seed completed a
declared repeat. Preserve the original failure and the distinction between
selected development records and independent-input validation.

The same configuration previously scored 3,794 on planner seed 3 (mean473ms,
maximum525ms). Two worker-placement controls reproduce that full trajectory;
pinning changes mean time by less than1% and has a larger maximum in this pair.
[Placement controls](random05/results/worker-affinity-32-full-v65/equivalence-and-affinity.json).

The same source first reached3,755 atK8192/B10, then3,776 atK16384/B10.
Both intermediate records remain in the timestamped history. The five-seed
K8192/B10 check scores3,730/3,642/3,611/3,755/3,715, mean3,690.6 versus3,636.6
for ordinaryK8192/B8 (+1.5%, three of five positive pairs). These are planner
seeds on one input. The larger configuration's original five attempts include one timeout. With its
declared strict repeat, completed outcomes average3,771.4 (+2.19%, all five pairs
positive); that does not establish five-for-five first-attempt deadline reliability.
[Completed outcomes with the original failure retained](random05/results/staged-record-seeds-32-split-full-v65/k16320-completed-outcomes-with-repeat.json).
[Paired evidence](random05/results/staged-seeds-and-budget-32-split-full-v65/paired-seeds.json).

The previous best was3,743 with ordinaryK8192/B8 on plannerseed0,
source[d6a3e0f](https://github.com/fywu85/lorr/commit/d6a3e0f). The staged method
scores3,730 onseed0 (-13), and3,755 onseed3 (+50 versus the oldseed3=3,705).
The completed five-pair check above supports a modest mean improvement on this
input; it does not establish improvement across independent inputs.

For the previous ordinaryK8192/B8 configuration, the five-seed mean is
3,636.6; blending future offsets with retained vectors at weight0.5 averages
3,652.8 (+0.45%, three of five positive pairs), but its best is3,724. These are
planner seeds on one development input, not five independent task/start inputs.
The mean difference is small relative to variation between seeds.
[Paired seed evidence](random05/results/elite-futures-seeds-split-full-v59/summary.json).

A separate K16384/B16 run scored3,741 on planner seed3 (mean452ms, maximum563ms).
It tests more continuations per root at the same1,024 root candidates asK8192/B8.
Its five planner-seed scores are3,705/3,617/3,715/3,741/3,567, mean3,669.0,
versus3,636.6 forK8192/B8 (+0.89%, three of five positive pairs). These are
planner seeds on one input; its highest score remains below3,743.
[Budget comparison](random05/results/persistent-budget-split-full-v55/summary.json).

**Best confirmed four-core run: 3,770 tasks**, or **+29.4% versus the strongest
matched NMS4 repeat = 2,914**. Source
[5f81613](https://github.com/fywu85/lorr/commit/5f81613), planner seed 3,
first K4608 then K5760/B12, screen2/keep4, four generations/E8/P8, blend0,
cache512 and kinematic mask enabled. Mean788ms, maximum845ms, RSS485MB;
all 2,000 steps are valid under a strict one-second deadline. Two placement
controls on disjoint four-core sets reproduce every action, assignment, event
and task from the faster 32-worker run. Worker pinning gives effectively equal
mean time in this pair; it is not a measured optimization.
[Full evidence](random05/results/worker-affinity-four-full-v65/summary.json),
[trajectory and affinity audit](random05/results/worker-affinity-four-full-v65/equivalence-and-affinity.json).

The five planner seeds at this configuration score 3,663/3,669/3,732/3,770/3,654,
mean **3,697.6**. Compared with the earlier ordinary K5120/B8/E1/P1 configuration,
all five pairs improve (+2.54% mean). This compares complete configurations on
one development input, not an isolated effect of screening or fresh inputs.
The new frozen V4 protocol tests two untouched task/start streams.
[Seed evidence](random05/results/staged-record-seeds-32-split-full-v65/k5760-paired-seeds.json),
[V4 protocol](random05/FRESH_VALIDATION_V4.md).

The immediately preceding four-core record was 3,718 at K4608/B12: mean628ms,
maximum756ms, RSS474MB. That cheaper configuration and its exact 32-worker
trajectory confirmation remain in the timestamped history.
[Previous evidence](random05/results/staged-b12-four-split-full-v65/four-screen2-keep4-k4608-b12-roots1024-seed3/summary.json).

The former four-core best was3,709, sourcef81b760, firstK5120 thenK6144/B8,
seed3, future blend0.5. It completed strictly (mean820ms, maximum889ms), with
the same whole trace as the earlier relaxed-deadline result. Earlier deadline
failures remain archived; the successful result is not an isolated cycle-mask
speedup measurement.
[Previous record](random05/results/cycle-mask-strict-candidate-split-full-v61/four-first5120-k6144-blend0.5-cycle-mask1-strict/summary.json).

The previous four-core best was3,655 atK5120/B8/blend0. The optimized source
60c5f9b averaged702ms (maximum826ms), reproducing its original source05559b7
trajectory exactly. A later unchanged-search control fromf81b760 also reproduces
3,655 (mean670ms, maximum793ms).

For that previous3,655 configuration, planner seeds0–4
score 3,583 / 3,495 / 3,614 / 3,655 / 3,602, mean **3,589.8**, versus **3,606.0**
for one parent and one retained vector (3,611 / 3,526 / 3,648 / 3,637 / 3,608).
Only one of five pairs improves. The fast 32-worker seed3 run reproduces every
action, schedule, task and event of the four-core record. These are planner seeds
on one development input. Fresh validation V3 retains its frozen one-parent
candidate, planner seed3; no later record is substituted.
[Seed evidence](random05/results/persistent-parents-seeds-split-full-v55/summary.json).

The 3,501 configuration remains the frozen candidate for fresh validation V2:
K2048/B8/start2, source e896201, mean 459 ms, maximum 579 ms, RSS 285 MB. Its
complete trajectory repeats across four cores and 32 workers.
[Equivalence](random05/results/hotpaths-four-split-full-v45/3501-equivalence.json).
Later development records never replace a candidate inside a frozen validation.
Earlier K4096 implementations exceeded the strict four-core first-step deadline.
Exact sorting and candidate-ranking caches subsequently made K4096 and K5120
feasible on four cores; retain the earlier failures as implementation-specific evidence.

The separate relaxed-deadline3,709 measurement remains archived (source60c5f9b,
mean923ms, maximum1,495ms, declared5s deadline). It was never entered into the
strict frontier; the later strict run above provides that confirmation.
[Relaxed evidence](random05/results/kinematic-larger-four-relaxed-split-full-v60/first5120-k6144-elites8-persist8-blend0.5-relaxed5s/summary.json).

**Repeated improvement on the development input:** across planner seeds 0–4,
four-continuation search averages 3,392.6 tasks and eight-continuation search
averages 3,417.6, versus 3,338.4 for the previous configuration. All five pairs
improve in both comparisons (+1.6% and +2.4% on average). These are different
planner seeds on one fixed input, not independent task/start instances.
[Four continuations](random05/results/continuations-validation-split-full-v42/summary.json),
[eight continuations](random05/results/continuations8-validation-split-full-v42/summary.json).

All these records use explicit `--trick RANDOM-05`: generated directional field
15 with one direction pair reversed (flip seed 5), plus known-horizon triage
at 2,000 steps with scale 1.5. Other shared settings are depth 8, noise 200,
dispersion 0.8, wait cost 0.5, turn cost 0.6, exact guided matching with keep
bonus 0.5 and length weight 0.25, and directional penalty 2.4. Continuation
search is a general algorithmic change; its best runs still use these tricks.
Machine-readable settings are frozen in `random05/best*.json`.

**Fresh-input validation V4 of the frozen 3,770 configuration is complete:**
3,680 versus NMS repeats2,907/2,870 and3,641 versus2,930/2,918.
Against the stronger repetition: **+26.59% / +24.27%, aggregate +25.42%**.
All six original attempts pass the full2,000-step, strict1s, four-core and32GB
checks. Protocola7bad0c was committed before generating inputs50007/50008;
source5f81613/seed3 was retained. Candidate means796/791ms, maxima859/852ms.
This is close to the colleague's reported27–28% improvement range on different
private inputs; it does not exactly reproduce their experiment. All50001–50008
inputs remain excluded from tuning.
[Matched V4 audit](random05/results/fresh-validation-v4/audit.json),
[protocol and full results](random05/FRESH_VALIDATION_V4.md).

**Fresh-input validation of the frozen 3,637 configuration is complete:**
it scores 3,698 versus NMS repeats 2,926/2,900 and 3,619 versus 2,906/2,831.
Against the stronger NMS repeat per input: **+26.38% / +24.54%, aggregate +25.46%**.
All six full runs pass the four-core / strict 1s / 32GB checks. Protocol81bdfbd
froze source6ce9312 and plannerseed3 before input generation. The later
3,648 development seed and elite-parent changes were not substituted.
[Matched audit](random05/results/fresh-validation-v3/audit.json).

Previous fresh validation of sourcee896201 / the3,501 configuration scored
3,494 versus NMS2,984 and3,387 versus2,892, **+17.1% aggregate**, allsixvalid.
V2 andV3 use different task/start inputs, so this percentage difference is not
an isolated paired effect. Allsix generated inputs remain outside tuning.
[Previous matched audit](random05/results/fresh-validation-v2/audit.json).

**Without known-horizon triage**, current configurations complete **3,503 tasks
on four cores (+20.2% versus matched NMS4)** and **3,632 on32 workers
(+14.5% versus matched NMS32)**. Both source5f81613 full runs pass strict1s.
Their matching cutoff-enabled counts are3,770/3,852, so the cutoff contributes
267/220 tasks (+7.6%/+6.1% relative to cutoff-off) on these selected seeds.
The generated guidance trick remains enabled; these are not results without
map tuning. Each paired setting differs only in R05_HORIZON, with identical
binary and input hashes. Both compared allocations use EPYC9354 CPUs.
[All full results](random05/results/staged-no-horizon-split-full-v65/summary.json),
[paired-setting audit](random05/results/staged-no-horizon-split-full-v65/horizon-ablation.json).

**Waiting-time audit of the current throughput records:** the longest completed
order takes1,941 steps for our four-core run versus1,997 forNMS; the32-worker
pair is1,940 versus1,976. Both solvers still have step-zero orders unfinished at
step2,000, so eventual maximum latency is unknown and at least2,000 steps.
Initial orders unfinished:137 versus219 on four cores,138 versus206 on32workers,
out of1,200 initially revealed. Initial orders never opened:96 versus102 and
96 versus91 respectively. Higher throughput does not establish a waiting bound.
[Matched audit](random05/results/task-waiting-frontiers-20260920T1612/REPORT.md),
[latency history for every frontier](random05/WAITING_PROGRESS.md).

[NMS four-worker evidence](random05/results/nms4-full-v1/summary.json),
[NMS 32-worker evidence](random05/results/nms-original-full-v1/summary.json),
[NMS four-worker repeats](random05/results/nms4-repeats-full-v3/summary.json).
Historical rows on EPYC 7532 or two workers are exploratory comparisons.
Recent frontier runs and their NMS references use the same EPYC 9354 model and
allocation. The four-worker NMS build changes only its worker constant; both
local NMS builds retain the previously documented constructor-initialization
fix. Neither removes combined-track features.

| Completed UTC | Source commit | Configuration / seed | Tasks / 2,000 | Workers / physical cores / CPU | NMS reference | Gain vs reference | Evidence |
|---|---|---|---:|---|---:|---:|---|
| 2026-09-20T07:55:37.615086+00:00 | [215fd6d](https://github.com/fywu85/lorr/commit/215fd6d167c96695e9ac6db6f8517791f2dab077) | Generated flow seed 1; turn cost 0.6; K=16; planner seed 0; `--trick RANDOM-05` | 54 | 4 / 4 / EPYC 9354 | 2903 (4 workers) | -98.1% | [Full evidence](random05/results/flow-first-v4/summary.json) |
| 2026-09-20T07:58:41.020438+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | lanes-intent; K=16; seed 0; `--trick RANDOM-05` | 2113 | 4 / 4 / EPYC 7532 | 2903 (4 workers) | -27.2% | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T07:58:41.152623+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | flow-intent; K=16; seed 0; `--trick RANDOM-05` | 2230 | 4 / 4 / EPYC 7532 | 2903 (4 workers) | -23.2% | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T07:58:41.330442+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | flow06-disp08; K=16; seed 0; `--trick RANDOM-05` | 2346 | 4 / 4 / EPYC 7532 | 2903 (4 workers) | -19.2% | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T07:58:42.118225+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | upstream-intent; K=16; seed 0; `--trick RANDOM-05` | 2431 | 4 / 4 / EPYC 7532 | 2903 (4 workers) | -16.3% | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T08:02:13.099706+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | file-k64-disp0-sched0; seed 0; `--trick RANDOM-05` | 2488 | 4 / 4 / EPYC 9354 | 2903 (4 workers) | -14.3% | [Full evidence](random05/results/scale-full-v5/summary.json) |
| 2026-09-20T08:05:44.745252+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | file-k256-disp0.8-sched1; seed 0; `--trick RANDOM-05` | 2562 | 4 / 4 / EPYC 9354 | 2903 (4 workers) | -11.7% | [Full evidence](random05/results/scale-full-v5/summary.json) |
| 2026-09-20T08:05:50.770563+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | file-k256-disp0.8-sched0; seed 0; `--trick RANDOM-05` | 2594 | 4 / 4 / EPYC 9354 | 2903 (4 workers) | -10.6% | [Full evidence](random05/results/scale-full-v5/summary.json) |
| 2026-09-20T08:10:24.936747+00:00 | [8993a43](https://github.com/fywu85/lorr/commit/8993a43) | K=64; local5; equal; dispersion 0.8; horizon 2000; seed 0; `--trick RANDOM-05` | 2658 | 4 / 4 / EPYC 9354 | 2903 (4 workers) | -8.4% | [Full evidence](random05/results/refinements-full-v6/summary.json) |
| 2026-09-20T08:18:53.857208+00:00 | [ca80563](https://github.com/fywu85/lorr/commit/ca80563) | predict-match; K=64; local5/equal/horizon2000; seed 0; `--trick RANDOM-05` | 2669 | 4 / 4 / EPYC 7532 | 2903 (4 workers) | -8.1% | [Full evidence](random05/results/motion-guidance-full-v8/summary.json) |
| 2026-09-20T08:19:08.147723+00:00 | [ca80563](https://github.com/fywu85/lorr/commit/ca80563) | loop4; K=64; local5/equal/horizon2000; seed 0; `--trick RANDOM-05` | 2677 | 4 / 4 / EPYC 7532 | 2903 (4 workers) | -7.8% | [Full evidence](random05/results/motion-guidance-full-v8/summary.json) |
| 2026-09-20T08:32:56.449253+00:00 | [6fb222e](https://github.com/fywu85/lorr/commit/6fb222e) | Wait0.5; cached K=64; local5/equal/horizon2000; seed 0; `--trick RANDOM-05` | 2729 | 4 / 4 / EPYC 7532 | 2903 (4 workers) | -6.0% | [Full evidence](random05/results/wait-cost-full-v9/summary.json) |
| 2026-09-20T08:41:54.502028+00:00 | [f7ca98c](https://github.com/fywu85/lorr/commit/f7ca98c) | exact160-guided; wait0.5; K64/local5/equal/horizon2000; seed0; `--trick RANDOM-05` | 2766 | 4 / 4 / EPYC 7532 | 2903 (4 workers) | -4.7% | [Full evidence](random05/results/matching-wait-full-v10/summary.json) |
| 2026-09-20T08:41:56.906339+00:00 | [f7ca98c](https://github.com/fywu85/lorr/commit/f7ca98c) | triage09; wait0.5; K64/local5/equal/horizon2000; seed0; `--trick RANDOM-05` | 2767 | 4 / 4 / EPYC 7532 | 2903 (4 workers) | -4.7% | [Full evidence](random05/results/matching-wait-full-v10/summary.json) |
| 2026-09-20T08:48:21.604864+00:00 | [8619b95](https://github.com/fywu85/lorr/commit/8619b95) | exact800-guide; K64/wait0.5/local5/equal/horizon2000 scale0.9; seed0; `--trick RANDOM-05` | 2841 | 4 / 4 / EPYC 9354 | 2903 (4 workers) | -2.1% | [Full evidence](random05/results/task-cost-full-v11/summary.json) |
| 2026-09-20T08:55:52.615079+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | avg-p3-cb0.75-s5-exact-guide; K64/triage0.9; planner seed0; `--trick RANDOM-05` | 2842 | 4 / 4 / EPYC 9354 | 2903 (4 workers) | -2.1% | [Full evidence](random05/results/traffic-field-validation-full-v12/summary.json) |
| 2026-09-20T08:55:55.708615+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | avg-p3-cb0.75-s5-greedy; K64/triage0.9; planner seed0; `--trick RANDOM-05` | 2868 | 4 / 4 / EPYC 9354 | 2903 (4 workers) | -1.2% | [Full evidence](random05/results/traffic-field-validation-full-v12/summary.json) |
| 2026-09-20T08:59:55.016021+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | Public field; exact/oriented matching; triage1.5; K64; seed0; `--trick RANDOM-05` | 2872 | 4 / 4 / EPYC 7532 | 2903 (4 workers) | -1.1% | [Full evidence](random05/results/triage-scale-full-v12-bound/summary.json) |
| 2026-09-20T09:01:43.509210+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | flow-seed15; generated field; K64; planner seed0; `--trick RANDOM-05` | 2910 | 2 / 2 / EPYC 7532 | 2903 (4 workers) | +0.2% | [Full evidence](random05/results/traffic-seeds-full-v12/summary.json) |
| 2026-09-20T09:05:15.036957+00:00 | [b79a218](https://github.com/fywu85/lorr/commit/b79a218) | noise200; generated field; K64; planner seed0; `--trick RANDOM-05` | 2916 | 4 / 4 / EPYC 9354 | 2903 (4 workers) | +0.4% | [Full evidence](random05/results/priority-search-full-v13/summary.json) |
| 2026-09-20T09:14:27.889620+00:00 | [134faa8](https://github.com/fywu85/lorr/commit/134faa8) | field15-triage15; K64/noise200; planner seed0; `--trick RANDOM-05` | 2997 | 4 / 4 / EPYC 7532 | 2903 (4 workers) | +3.2% | [Full evidence](random05/results/pockets-combinations-full-v15/summary.json) |
| 2026-09-20T09:19:02.607264+00:00 | [6aed8ba](https://github.com/fywu85/lorr/commit/6aed8ba) | blocked-extent2; K64; planner seed0; `--trick RANDOM-05` | 3127 | 4 / 4 / EPYC 9354 | 2903 (4 workers) | +7.7% | [Full evidence](random05/results/cycle-portfolio-full-v16/summary.json) |
| 2026-09-20T09:21:46.636885+00:00 | [134faa8](https://github.com/fywu85/lorr/commit/134faa8) | K1024, 32 workers, field15/noise200/triage1.5; seed0; `--trick RANDOM-05` | 3231 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +1.9% | [Full evidence](random05/results/frontier-compute-full-v15/summary.json) |
| 2026-09-20T09:40:30.891971+00:00 | [6aed8ba](https://github.com/fywu85/lorr/commit/6aed8ba) | Exact/oriented matching, keep0.5; K1024/32 workers; field15/noise200/triage1.5; seed0; `--trick RANDOM-05` | 3299 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +4.0% | [Full evidence](random05/results/compound-search-full-v16/summary.json) |
| 2026-09-20T10:03:44.348412+00:00 | [79d0e79](https://github.com/fywu85/lorr/commit/79d0e79) | Shared candidate sorting; exact/oriented keep0.5; K1024; field15/noise200/triage1.5; seed0; `--trick RANDOM-05` | 3299 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +13.2% | [Full evidence](random05/results/shared-sort-full-v18/summary.json) |
| 2026-09-20T10:19:47.676357+00:00 | [2ead4f4](https://github.com/fywu85/lorr/commit/2ead4f4) | Same score; stable insertion sorting lowers mean latency to335ms; K1024, field15/seed0; `--trick RANDOM-05` | 3299 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +13.2% | [Full evidence](random05/results/candidate-sort-full-v20/summary.json) |
| 2026-09-20T10:23:12.419038+00:00 | [2ead4f4](https://github.com/fywu85/lorr/commit/2ead4f4) | contrast3.2; K1024, field15/seed0; `--trick RANDOM-05` | 3351 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +15.0% | [Full evidence](random05/results/contrast-validation-full-v20/summary.json) |
| 2026-09-20T10:23:13.900548+00:00 | [2ead4f4](https://github.com/fywu85/lorr/commit/2ead4f4) | contrast2.4; K1024, field15/seed0; `--trick RANDOM-05` | 3363 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +15.4% | [Full evidence](random05/results/contrast-validation-full-v20/summary.json) |
| 2026-09-20T10:57:32.632101+00:00 | [3228b9c](https://github.com/fywu85/lorr/commit/3228b9c) | Independent per-step random streams; K1024, contrast2.4, field15/seed0; `--trick RANDOM-05` | 3374 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +15.8% | [Full evidence](random05/results/load-depth-split-full-v22/step-rng-k1024/summary.json) |
| 2026-09-20T11:09:45.903706+00:00 | [3228b9c](https://github.com/fywu85/lorr/commit/3228b9c) | Same3,374 trajectory; independent per-step RNG; K1024, contrast2.4, field15/seed0; `--trick RANDOM-05` | 3374 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +6.4% | [Full evidence](random05/results/step-rng-validation-split-full-v22/step-rng-workers32/summary.json) |
| 2026-09-20T11:17:50.924764+00:00 | [3228b9c](https://github.com/fywu85/lorr/commit/3228b9c) | Independent per-step RNG; K1024, contrast2.4, field15; planner seed3; `--trick RANDOM-05` | 3379 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +16.0% | [Full evidence](random05/results/step-rng-validation-split-full-v22/step-rng-seed3/summary.json) |
| 2026-09-20T11:25:19.439602+00:00 | [32b333a](https://github.com/fywu85/lorr/commit/32b333a) | Same seed3 trajectory on32 workers; active cost rows; K1024, contrast2.4, field15; `--trick RANDOM-05` | 3379 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +6.5% | [Full evidence](random05/results/active-cost-split-full-v28/seed3-workers32/summary.json) |
| 2026-09-20T11:51:07.120923+00:00 | [b824f5d](https://github.com/fywu85/lorr/commit/b824f5d) | One directional pair reversed, flip seed5; K1024, contrast2.4, field15; planner seed3; `--trick RANDOM-05` | 3395 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +7.0% | [Full evidence](random05/results/guidance-local-split-full-v31/flips1-seed5/summary.json) |
| 2026-09-20T12:08:42.946296+00:00 | [b824f5d](https://github.com/fywu85/lorr/commit/b824f5d) | Four-core repeat of the same actions/assignments/events; one pair reversed, flip seed5; field15/planner seed3; `--trick RANDOM-05` | 3395 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +16.5% | [Full evidence](random05/results/guidance-local-validation-split-full-v31/flips1-seed5-four/summary.json) |
| 2026-09-20T12:21:57.244760+00:00 | [b824f5d](https://github.com/fywu85/lorr/commit/b824f5d) | Separate record without horizon cutoff; K1024, field15/one flip seed5, planner seed3; `--trick RANDOM-05` | 3197 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +0.8% | [Full evidence](random05/results/frontier-triage-split-full-v31/no-horizon/summary.json) |
| 2026-09-20T12:37:00.784376+00:00 | [b824f5d](https://github.com/fywu85/lorr/commit/b824f5d) | Separate record without horizon cutoff; K1024, field15/one flip seed5, planner seed3; `--trick RANDOM-05` | 3197 | 4 / 4 / EPYC 9354 | 2914 (4 workers) | +9.7% | [Full evidence](random05/results/no-horizon-four-split-full-v31/no-horizon-four/summary.json) |
| 2026-09-20T13:32:10.140710+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | Mean of4 continuations; K1024 total/local0; field15/one flip seed5/planner seed3; `--trick RANDOM-05` | 3400 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +7.2% | [Full evidence](random05/results/continuations-split-full-v42/mean4-k1024/summary.json) |
| 2026-09-20T13:33:08.542811+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | Mean of8 continuations; K1024 total/local0; field15/one flip seed5/planner seed3; `--trick RANDOM-05` | 3422 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +7.9% | [Full evidence](random05/results/continuations-split-full-v42/mean8-k1024/summary.json) |
| 2026-09-20T13:34:31.273508+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | Mean of4 continuations; K1024 total/local0; continuation starts after2 steps; field15/one flip seed5/planner seed3; `--trick RANDOM-05` | 3450 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +8.8% | [Full evidence](random05/results/continuations-split-full-v42/mean4-start2/summary.json) |
| 2026-09-20T13:36:34.649056+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | Mean of4 continuations; K2048 total/local0; continuation starts after1 steps; field15/one flip seed5/planner seed3; `--trick RANDOM-05` | 3492 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +10.1% | [Full evidence](random05/results/continuations-split-full-v42/mean4-k2048/summary.json) |
| 2026-09-20T13:44:31.351693+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | Four-core reproduction, exact actions/schedules/events; mean of4 continuations; K1024/local0/planner seed3; `--trick RANDOM-05` | 3400 | 4 / 4 / EPYC 9354 | 2914 (4 workers) | +16.7% | [Full evidence](random05/results/continuations-validation-split-full-v42/mean4-four/summary.json) |
| 2026-09-20T13:46:39.608755+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | Four-core reproduction, exact actions/schedules/events; mean of8 continuations; K1024/local0/planner seed3; `--trick RANDOM-05` | 3422 | 4 / 4 / EPYC 9354 | 2914 (4 workers) | +17.4% | [Full evidence](random05/results/continuations8-validation-split-full-v42/mean8-four/summary.json) |
| 2026-09-20T13:52:47.072835+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | Mean of 8 continuations; K2048 total/local0; mutation starts after 2 steps; field15/flip5/planner seed3; `--trick RANDOM-05` | 3501 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +10.4% | [Full evidence](random05/results/continuations-scale-split-full-v42/k2048-b8-start2/summary.json) |
| 2026-09-20T13:57:09.026640+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | Mean of 4 continuations; K4096 total/local0; mutation starts after 2 steps; field15/flip5/planner seed3; `--trick RANDOM-05` | 3555 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +12.1% | [Full evidence](random05/results/continuations-scale-split-full-v42/k4096-b4-start2/summary.json) |
| 2026-09-20T14:00:11.259204+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | Four-core reproduction of all actions/schedules/events; B4/K2048/start1/local0/planner seed3; `--trick RANDOM-05` | 3492 | 4 / 4 / EPYC 9354 | 2914 (4 workers) | +19.8% | [Full evidence](random05/results/continuations-scale-split-full-v42/mean4-k2048-four/summary.json) |
| 2026-09-20T14:04:45.937007+00:00 | [da00823](https://github.com/fywu85/lorr/commit/da00823) | Same full trajectory with exact prefix reuse; B4/K2048/start1/local0/planner seed3; `--trick RANDOM-05` | 3492 | 4 / 4 / EPYC 9354 | 2914 (4 workers) | +19.8% | [Full evidence](random05/results/shared-prefix-split-full-v44/shared-k2048-b4-four/summary.json) |
| 2026-09-20T14:15:04.122374+00:00 | [e896201](https://github.com/fywu85/lorr/commit/e896201) | Same 3,555 full trajectory with prefix reuse, packed priorities and sparse dispersion; B4/K4096/start2/seed3; `--trick RANDOM-05` | 3555 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +12.1% | [Full evidence](random05/results/hotpaths-split-full-v45/both/summary.json) |
| 2026-09-20T14:24:41.462668+00:00 | [e896201](https://github.com/fywu85/lorr/commit/e896201) | Exact four-core reproduction of the 3,501 trajectory; B8/K2048/start2/local0/planner seed3; prefix/packed/sparse optimizations; `--trick RANDOM-05` | 3501 | 4 / 4 / EPYC 9354 | 2914 (4 workers) | +20.1% | [Full evidence](random05/results/hotpaths-four-split-full-v45/retry3501-four/summary.json) |
| 2026-09-20T14:29:52.858068+00:00 | [d933023](https://github.com/fywu85/lorr/commit/d933023) | Mean of 8 continuations; K8192 total/start2/local0/planner seed3; exact implementation optimizations; `--trick RANDOM-05` | 3596 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +13.4% | [Full evidence](random05/results/scratch-reuse-split-full-v46/k8192-b8/summary.json) |
| 2026-09-20T14:45:45.463667+00:00 | [d933023](https://github.com/fywu85/lorr/commit/d933023) | B8/K3584/start2/local0/planner seed3 with exact implementation optimizations; `--trick RANDOM-05` | 3509 | 4 / 4 / EPYC 9354 | 2914 (4 workers) | +20.4% | [Full evidence](random05/results/scratch-four-split-full-v46/k3584-b8-four/summary.json) |
| 2026-09-20T14:52:24.059199+00:00 | [f9b1143](https://github.com/fywu85/lorr/commit/f9b1143) | Four generations; K2048/B8/start2/local0; exact optimizations; planner seed3; `--trick RANDOM-05` | 3520 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +20.8% | [Full evidence](random05/results/generation-four-split-full-v47/generations4-four/summary.json) |
| 2026-09-20T14:57:15.935581+00:00 | [d933023](https://github.com/fywu85/lorr/commit/d933023) | K16384/B8/start2/local0; exact optimizations; planner seed3; `--trick RANDOM-05` | 3657 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +15.3% | [Full evidence](random05/results/continuation-larger-split-full-v46/k16384-b8/summary.json) |
| 2026-09-20T14:57:35.059037+00:00 | [d933023](https://github.com/fywu85/lorr/commit/d933023) | No known horizon; K8192/B8/start2/local0; planner seed3; guidance still `--trick RANDOM-05` | 3408 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +7.4% | [Full evidence](random05/results/continuation-no-horizon-split-full/no-horizon-32/summary.json) |
| 2026-09-20T15:02:34.822809+00:00 | [e896201](https://github.com/fywu85/lorr/commit/e896201) | No known horizon; K2048/B8/start2/local0; planner seed3; guidance still `--trick RANDOM-05` | 3285 | 4 / 4 / EPYC 9354 | 2914 (4 workers) | +12.7% | [Full evidence](random05/results/continuation-no-horizon-split-full/no-horizon-four/summary.json) |
| 2026-09-20T15:29:14.931221+00:00 | [f9b1143](https://github.com/fywu85/lorr/commit/f9b1143) | Four generations; K2048/B8/start2/local0; planner seed4; `--trick RANDOM-05` | 3562 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +22.2% | [Full evidence](random05/results/generation-confirm-four-split-full-v47/generations4-seed4-four/summary.json) |
| 2026-09-20T15:29:21.178706+00:00 | [f9b1143](https://github.com/fywu85/lorr/commit/f9b1143) | Four generations; K2048/B8/start2/local0; planner seed0; `--trick RANDOM-05` | 3562 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +22.2% | [Full evidence](random05/results/generation-confirm-four-split-full-v47/generations4-seed0-four/summary.json) |
| 2026-09-20T15:50:14.795609+00:00 | [6ce9312](https://github.com/fywu85/lorr/commit/6ce9312) | K5120/B8/start2/local0; four generations; exact CPU optimizations; planner seed3; `--trick RANDOM-05` | 3637 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +24.8% | [Full evidence](random05/results/ranking-cache-split-full-v52/ranking-k5120-b8-generations4/summary.json) |
| 2026-09-20T16:15:24.583747+00:00 | [8eb59d3](https://github.com/fywu85/lorr/commit/8eb59d3) | K8192/B8/start2/local0; four generations / eight elite parents; planner seed3; exact caches/radix; `--trick RANDOM-05` | 3689 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +16.3% | [Full evidence](random05/results/elite-scaling-32-split-full-v54/k8192-elites8-workers32/summary.json) |
| 2026-09-20T16:25:39.768782+00:00 | [6ce9312](https://github.com/fywu85/lorr/commit/6ce9312) | K5120/B8/start2/local0; four generations; exact CPU optimizations; planner seed2; `--trick RANDOM-05` | 3648 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +25.2% | [Full evidence](random05/results/k5120-seeds-four-split-full-v52/k5120-generations4-seed2/summary.json) |
| 2026-09-20T16:34:48.526472+00:00 | [05559b7](https://github.com/fywu85/lorr/commit/05559b7) | K8192/B8/start2/local0; four generations; eight parents / eight persistent vectors; planner seed3; `--trick RANDOM-05` | 3705 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +16.8% | [Full evidence](random05/results/persistent-elites-32-split-full-v55/k8192-elites8-persist8/summary.json) |
| 2026-09-20T17:09:29.991140+00:00 | [05559b7](https://github.com/fywu85/lorr/commit/05559b7) | K5120/B8/start2/local0, generations4/elites8/persist8; exact CPU optimizations; seed3; `--trick RANDOM-05` | 3655 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +25.4% | [Full evidence](random05/results/persistent-parents-four-split-full-v55/k5120-elites8-persist8/summary.json) |
| 2026-09-20T17:38:53.567749+00:00 | [d6a3e0f](https://github.com/fywu85/lorr/commit/d6a3e0f) | K8192/B8/start2/local0, generations4/elites8/persist8; future elite blend0.5; exact CPU optimizations; seed3; `--trick RANDOM-05` | 3724 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +17.4% | [Full evidence](random05/results/elite-futures-split-full-v59/k8192-elite-future0.5/summary.json) |
| 2026-09-20T17:45:06.759124+00:00 | [05559b7](https://github.com/fywu85/lorr/commit/05559b7) | K16384/B16/start2/local0, generations4/elites8/persist8; exact CPU optimizations; seed3; `--trick RANDOM-05` | 3741 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +17.9% | [Full evidence](random05/results/persistent-budget-split-full-v55/k16384-b16-elites8-persist8/summary.json) |
| 2026-09-20T17:52:23.244327+00:00 | [d6a3e0f](https://github.com/fywu85/lorr/commit/d6a3e0f) | K8192/B8/start2/local0, generations4/elites8/persist8; future elite blend0; exact CPU optimizations; seed0; `--trick RANDOM-05` | 3743 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +18.0% | [Full evidence](random05/results/elite-futures-seeds-split-full-v59/k8192-elite-future0-seed0/summary.json) |
| 2026-09-20T18:07:16.977439+00:00 | [60c5f9b](https://github.com/fywu85/lorr/commit/60c5f9b) | Same3655 trajectory; cache512/kinematic mask; K5120/B8/start2/local0, generations4/elites8/persist8; seed3; `--trick RANDOM-05` | 3655 | 4 / 4 / EPYC 9354 | 2914 (4 workers, strongest repeat) | +25.4% | [Full evidence](random05/results/kinematic-mask-split-full-v60/k5120-persist8-kinematic-mask1/summary.json) |
| 2026-09-20T18:59:20.474586+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | Staged continuations: K8192/B10/s2/keep4, generations4/E8/P8, seed3, blend0; **trick** guidance+known horizon | 3755 | 32 / 16 / EPYC 9354 | 3172 NMS32 | +18.4% | [full run](random05/results/staged-continuations-32-split-full-v65/32-screen2-keep4-k8192-b10-seed3-blend0/summary.json) |
| 2026-09-20T18:59:34.727026+00:00 | [f81b760](https://github.com/fywu85/lorr/commit/f81b760) | FirstK5120 thenK6144/B8, generations4/E8/P8, seed3, futureblend0.5, cache512, kinematic+cycle masks; **trick** guidance+known horizon | 3709 | 4 / 4 / EPYC 9354 | 2914 NMS4 | +27.3% | [full run](random05/results/cycle-mask-strict-candidate-split-full-v61/four-first5120-k6144-blend0.5-cycle-mask1-strict/summary.json) |
| 2026-09-20T19:21:39.535205+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | K16384/B10, firstK0, screen2/keep4, generations4/E8/P8, seed3, blend0; **trick** guidance+known horizon | 3776 | 32 / 16 / EPYC 9354 | 3172 NMS32 | +19.0% | [full run](random05/results/staged-seeds-and-budget-32-split-full-v65/32-screen2-keep4-k16384-b10-seed3-blend0/summary.json) |
| 2026-09-20T19:23:22.219359+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | FirstK8000 thenK16320/B14, screen2/keep4, generations4/E8/P8, seed3, blend0; **trick** guidance+known horizon | 3794 | 32 / 16 / EPYC 9354 | 3172 NMS32 | +19.6% | [full run](random05/results/staged-initial-budget-32-split-full-v65/32-screen2-keep4-k16320-b14-seed3-blend0-first8000/summary.json) |
| 2026-09-20T19:47:51.315402+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | K4608/B12, screen2/keep4, generations4/E8/P8, seed3, cache512; **trick** guidance+known horizon | 3718 | 4 / 4 / EPYC 9354 | 2914 NMS4 | +27.6% | [full run](random05/results/staged-b12-four-split-full-v65/four-screen2-keep4-k4608-b12-roots1024-seed3/summary.json) |
| 2026-09-20T20:02:29.833542+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | FirstK4608 thenK5760/B12, screen2/keep4, generations4/E8/P8, seed3, cache512, worker binding false; **trick** guidance+known horizon | 3770 | 4 / 4 / EPYC 9354 | 2914 NMS4 | +29.4% | [full run](random05/results/worker-affinity-four-full-v65/summary.json) |
| 2026-09-20T20:07:08.133068+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | FirstK8000 thenK16320/B14, screen2/keep4, generations4/E8/P8, seed4, blend0; **trick** guidance+known horizon | 3852 | 32 / 16 / EPYC 9354 | 3172 NMS32 | +21.4% | [full run](random05/results/staged-record-seeds-32-split-full-v65/32-staged-k16320-b14-first8000-seed4/summary.json) |
| 2026-09-20T20:36:40.537684+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | Without horizon cutoff: firstK8000 thenK16320/B14, screen2/keep4, generations4/E8/P8, seed4; **trick** guidance only | 3632 | 32 / 16 / EPYC 9354 | 3172 NMS32 | +14.5% | [full run](random05/results/staged-no-horizon-split-full-v65/32-k16320-b14-first8000-seed4-no-horizon/summary.json) |
| 2026-09-20T20:47:29.818820+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | Without horizon cutoff: firstK4608 thenK5760/B12, screen2/keep4, generations4/E8/P8, seed3; **trick** guidance only | 3503 | 4 / 4 / EPYC 9354 | 2914 NMS4 | +20.2% | [full run](random05/results/staged-no-horizon-split-full-v65/four-k5760-b12-first4608-seed3-no-horizon/summary.json) |
| 2026-09-20T22:04:50.502045+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | K16320/B14/s2/q4/G4/E8/P8; first8000; seed4; triage1.25; `--trick RANDOM-05` | 3857 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +21.6% | [Full evidence](random05/results/record-triage-split-full-v65/32-record-triage1.25-seed4/summary.json) |
| 2026-09-20T22:49:18.963497+00:00 | [233f5bf](https://github.com/fywu85/lorr/commit/233f5bf) | K16320/B14/s2/q4/G4/E8/P8; first8000; seed4; triage1.25/directional mix0.5; `--trick RANDOM-05` | 3872 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +22.1% | [Full evidence](random05/results/directed-triage-split-full-v69/32-directed-triage-mix0.5-scale1.25-seed4/summary.json) |
| 2026-09-20T23:58:38.632599+00:00 | [233f5bf](https://github.com/fywu85/lorr/commit/233f5bf) | K16320/B18/s2/q4/G4/E8/P8; first7968; seed4; triage1.25/directional mix0.5; `--trick RANDOM-05` | 3877 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +22.2% | [Full evidence](random05/results/branch-allocation-split-full-v69/32-branch-allocation-b18-fixedwork-k16320-seed4/summary.json) |
| 2026-09-21T00:05:35.106669+00:00 | [1e266b0](https://github.com/fywu85/lorr/commit/1e266b0) | Startup rank power0.25 for250steps; K16320/B14/s2/q4/G4/E8/P8; first8000; seed4; `--trick RANDOM-05` | 3917 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +23.5% | [Full evidence](random05/results/startup-rank-split-full-v74/32-startup-rank-power.25-steps250-seed4/summary.json) |
| 2026-09-21T00:19:49.212073+00:00 | [233f5bf](https://github.com/fywu85/lorr/commit/233f5bf) | K16320/B18/s2/q4/G4/E8/P8; first7968; planner seed5; triage1.25/directional mix0.5; `--trick RANDOM-05` | 3928 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +23.8% | [Full evidence](random05/results/branch-seeds-split-full-v69/32-branch-seed-b18-seed5/summary.json) |

| 2026-09-21T01:28:12.433277+00:00 | [acdbfd7](https://github.com/fywu85/lorr/commit/acdbfd7) | K16320/B18/s2/q4/G4/E8/P8; first7968; planner seed5; move bias2; triage1.25/directional mix0.5; `--trick RANDOM-05` | 3933 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +24.0% | [Full evidence](random05/results/move-proposal-bias-split-full-v77/32-move-bias2-b18-seed5/summary.json) |

| 2026-09-21T01:50:46.795715+00:00 | [acdbfd7](https://github.com/fywu85/lorr/commit/acdbfd7) | K16320/B18/s2/q4/G4/E8/P8; first7968; planner seed5; move bias3; triage1.25/directional mix0.5; `--trick RANDOM-05` | 3941 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +24.2% | [Full evidence](random05/results/move-bias-followup-split-full-v77/32-move-bias3-b18-seed5/summary.json) |

| 2026-09-21T02:16:03.845916+00:00 | [acdbfd7](https://github.com/fywu85/lorr/commit/acdbfd7) | K16320/B18/s2/q4/G4/E8/P8; first7968; planner seed0; move bias3; triage1.25/directional mix0.5; `--trick RANDOM-05` | 3978 | 32 / 16 / EPYC 9354 | 3172 (32 workers) | +25.4% | [Full evidence](random05/results/move-bias-refinement-split-full-v77/32-move-bias3-b18-seed0/summary.json) |

## Reference evidence supplied by the user

| Colleague instance seed | Their combined solver | NMS better local build | Gain |
|---|---:|---:|---:|
| 41001 | 3,715 | 2,932 | 26.7% |
| 41002 | 3,531 | 2,762 | 27.8% |

Both are reported 4-core / one-second runs with a known 2,000-step horizon.
The published NMS score of 3,050 used different instances and hardware.

## Fresh input validation

The configuration was frozen at [b824f5d](https://github.com/fywu85/lorr/commit/b824f5d)
before generating these task/start instances; see the
[predeclared protocol](random05/FRESH_VALIDATION.md). Same map, four physical
EPYC9354 cores, 2,000 steps, strict 1s entry budget. All six runs are valid,
with zero errors/timeouts and measured RAM below 32 decimal GB. The candidate
retains its explicit guidance and known-horizon tricks.

| Instance seed | Candidate completed UTC | Candidate tasks | NMS repeats | Gain over stronger NMS | Evidence |
|---|---|---:|---|---:|---|
| 50001 | 2026-09-20T12:47:36.996239+00:00 | 3386 | 2920 / 2957 | +14.5% | [Matched audit](random05/results/fresh-validation-v1/audit.json) |
| 50002 | 2026-09-20T12:50:09.079437+00:00 | 3178 | 2898 / 2915 | +9.0% | [Matched audit](random05/results/fresh-validation-v1/audit.json) |

Aggregate: 6,564 versus 5,872 tasks (**+11.8%**), using the stronger NMS repeat
on each input. This supports a gain on new inputs, while falling short of the
colleague's reported 27–28%. Two instances on one map do not establish broader
generalization. These are not the colleague's private inputs, and no 32-worker
fresh-instance result is implied. The frozen validation candidate scored 3,395 on its original development input. Keep these inputs out of further configuration selection.


The second validation freezes source [e896201](https://github.com/fywu85/lorr/commit/e896201)
at protocol [e6b2dbe](https://github.com/fywu85/lorr/commit/e6b2dbe), before generating
seeds 50003/50004. It tests the 3,501 development configuration with the same
four-core / 1s / 32GB rules. All six runs are valid; both NMS repeats are retained.

| Instance seed | Candidate completed UTC | Candidate tasks | NMS repeats | Gain over stronger NMS | Evidence |
|---|---|---:|---|---:|---|
| 50003 | 2026-09-20T14:50:00.584389+00:00 | 3494 | 2946 / 2984 | +17.1% | [Matched audit](random05/results/fresh-validation-v2/audit.json) |
| 50004 | 2026-09-20T14:50:17.990772+00:00 | 3387 | 2892 / 2875 | +17.1% | [Matched audit](random05/results/fresh-validation-v2/audit.json) |

Aggregate: **6,881 versus 5,876 tasks (+17.1%)**. This supports an improvement
on two new task/start inputs on the same map. It remains below the colleague's
reported 27–28%; the two validation versions use different inputs, so their
11.8% and 17.1% figures are not a paired effect estimate. Keep all four validation
inputs out of configuration selection. Full per-run latency, CPU time, RSS,
input/binary hashes and allocation are linked in the audits.

## Development record

- 2026-09-20: started an isolated `random05/` backend from the supplied log.
  Snapshot the existing simulator while replacing the planner and scheduler.
  Initial implementation: collision-certified pipelining, exact oriented task
  chain costs, joint greedy matching, deterministic rollout portfolios, generic
  dead-end evacuation and coordinated cycle moves. Benchmark evidence pending.

- Initial v1 runs exposed exponential backtracking in the first PIBT executor.
  Progress froze and rollouts exhausted their expansion caps. These are failed
  implementation runs, not a performance frontier. Replaced rollback search
  with standard PIBT failure propagation and committed blocking waits.

- Corrected failure propagation passes the dense regression. The first
  full-size gridlock persists despite changing guidance. Idle-turn preparation
  is the next diagnosed mechanism to investigate. All four v4 runs complete
  2,000 steps without planner errors, schedule errors, or entry timeouts.

- v5: spatial-intention pre-rotation removes the persistent dense-map gridlock.
  The selected current move and following promised move are independently
  certified. Full runs now take about 35 seconds with K=16. Next: more futures,
  guidance-aware matching and dispersion ablations, followed by seed repeats.

- Scaling: K=64 gives 2,488; K=256 gives 2,522; adding dispersion 0.8
  gives 2,594. Guidance-aware matching helps the generated field (2,419 to
  2,548), but lowers the public-field result (2,594 to 2,562). Keep these as
  separate configurations instead of assuming every borrowed feature helps.

- Refinements at K=64: control 2,485; local search 2,531; local + equal-score
  acceptance 2,574; horizon triage alone 2,581; combined 2,658. Equal-score
  acceptance alone gives 2,485. Triage is recomputed after every task change
  and never illegally reassigns an opened task.

- v8: control repeats exactly at 2,658. Larger cycles: extent3=2,634,
  extent4=2,677. Predicted-position matching=2,669. Push price2=2,452.
  Turn-aware generated fields=2,287–2,461, below the public-field control.
  Keep the public field while investigating cost-cache efficiency and priority
  age consistency in simulated futures.

- v9: precomputed task-chain costs reproduce the complete control trajectory
  exactly (2,658 tasks) and lower mean latency from 71.8 to 64.2 ms on the
  same allocation. Priority-age simulation gives 2,558; pocket-component
  handling 2,652; together 2,622. Keep these experimental options off.
- Compute scaling with local refinements: K64=2,658, K256=2,630,
  K1024 on 32 logical CPUs=2,712. The gain is too small to explain the remaining
  gap. Wait-cost ablation: 0.05=2,691, 0.25=2,648, 0.5=2,729, 1=2,513,
  2=2,658, 4=2,559. The 0.5 setting is a new single-seed frontier and requires
  replication; it is not yet a demonstrated average gain.

- v10: wait0.5 control repeats at 2,729. Exact matching for <=160 eligible
  robots gives 2,684; with oriented guidance gives 2,766. Anticipating turns
  in the wait score gives 2,653 (wait2: 2,693), so leave it off. Larger cycles
  now give 2,684. Triage scale0.6=2,753, scale0.9=2,767.
- Trace diagnosis: the 2,729 run makes 459,441 forward moves versus NMS32's
  520,780. Tasks in successive 500-step blocks: ours 639/719/620/751,
  NMS32 975/750/754/693. Much of the gap opens early; test initial/global
  assignment and oriented task-chain cost as well as planner flow.
- Allocation audit: the borrowed-guidance v10 batch was granted CPU slots but
  GRID did not apply its requested physical-core binding (64 cores visible
  versus 24 requested). Cases still have disjoint taskset masks and fixed work,
  but latency is contended and cannot be treated as isolated. Future submissions
  fail immediately if GRID does not honor binding. Earlier frontier and NMS
  reference allocations have the expected physical-core count.

- Borrowed guidance, same fixed K64 wait0.5 control: public field=2,729,
  NMS bitmap=2,446, public weights power0.5=2,332, power2=2,701,
  half turn cost=2,638, double turn cost=2,534. All valid. The unchanged
  control repeats exactly despite the documented CPU contention.

- v11 task-cost ablation: control=2,767; exact matching including the initial
  800-agent assignment=2,763; with oriented approach cost=2,841. Oriented
  task-chain matching cost gives 2,803 with greedy matching, but 2,631 with
  exact matching. Length weights0/0.5/1 give 2,557/2,733/2,733. Do not combine
  favorable individual changes without testing their interaction.

- Public-field frontier repeats at 2,841. Planner seeds1/2/3/4 give
  2,736/2,736/2,685/2,562; the five-seed average including seed0 is 2,712.
  K128=2,818, K256=2,741, depth6=2,631, depth4=2,562. The maximum is not a
  demonstrated average gain. Keep seed and single-run labels explicit.
- Traffic-field screen (K8, full horizon): public control2,617; best generated
  field2,666. Full K64 validation: public2,841; generated seed5 with greedy
  matching2,868 and exact/guided matching2,842; seed2 gives 2,771/2,720.
  Low-K rankings do not reliably predict full-search rankings.
- GRID now reserves both SMT slots for each requested physical core, requests
  enough free cores through the scheduler's m_topology_inuse attribute, and
  checks the resulting affinity before any benchmark starts. Jobs rejected by
  the binding check are allocation failures, not solver failures.

- Public-field triage scales0.9/1.2/1.5/2/3 yield
  2,841/2,840/2,872/2,811/2,571. Earlier abandonment quickly becomes harmful;
  retain its explicit known-horizon trick label. NMS4 and both recent frontier
  batches ran on AMD EPYC9354 hosts, with four physical cores per process.

- Sixteen full K64 traffic-field seeds give 2,698–2,910; field seed15 is best.
  Field seed5 repeats exactly at 2,868 on two physical cores instead of four.
  Its earlier K8 score was lower than seed2, reinforcing the need for full
  production-search validation.
- Priority search on field5: noise100=2,860; noise200=2,916; age caps50/100/200
  give 2,818/2,859/2,841. Independent per-step random streams give 2,839;
  combining those streams with cap100/noise100 gives 2,909. Keep these as
  separate measured configurations; no claim that capped aging is generally
  better. Regression tests verify identical fixed-work trajectories with one
  and two worker threads under the new random-stream mode.

- Forcing cycle proposals before PIBT lowers throughput substantially:
  1,920–2,674 versus 2,916 control. Keep it off. Follow-up tests restrict
  preparation to robots PIBT already leaves waiting, or include baseline
  policies in the rollout portfolio so search can reject cycle proposals.
- Goal-less pocket eviction passes the targeted regression but lowers full-run
  throughput (eviction2=2,757; eviction4/8=2,846; with distinct pocket components
  2,855). Keep it off in the frontier. Combining field15 with noise200 gives
  2,960; triage1.5 gives 2,997. Field5 with triage1.5 gives 2,926.

- Six-seed validation of the 2,997 configuration: seeds0–5 give
  2,997/3,050/3,093/2,991/3,075/2,999 (mean 3,034). Without known horizon,
  seed0 gives 2,796: triage contributes 201 tasks (+7.2%) in that pair.
- General cycle preparation restricted to already-waiting robots works:
  extents2/3/4 give 3,127/3,109/3,063 versus 2,997 control. Baseline/forced-cycle
  portfolios give 2,720–2,954 and remain off. The improvement comes from
  preparing blocked groups while preserving PIBT's useful existing moves.

- Larger portfolios on the competition CPU topology: K1024=3,231 and
  K2048=3,205, versus 3,172 NMS. All valid; respective mean latencies134/230ms,
  maxima239/340ms. Save the overall best in `random05/best.json` and the
  four-core best in `random05/best-four-cores.json`.
- Six-seed blocked-cycle results: 3,127/3,025/2,974/2,974/2,961/3,063;
  mean 3,021 versus 3,034 without preparation. This improves the best seed,
  not the six-seed mean. Without horizon triage the blocked-cycle run gives
  2,914, compared with2,796 without preparation.
- Scheduler settings on the preceding four-core control (2,997): keep bonuses
  0/0.25/0.5/1/4 give 3,042/3,050/3,092/3,010/3,084. Exact/oriented matching
  with keep0.5 gives 3,109; length0.1/0.5 gives 3,018/3,067.
- Policy settings on that control: dispersion0/0.4/1.6/3.2 gives
  3,023/3,034/2,995/2,838; local20/50 gives 3,007/2,968;
  noise400/800 gives 3,064/3,051. Compound settings require their own validation.
- Two NMS repeat attempts failed before solver launch because the harness
  requested a 32GB process limit above the smaller inherited GRID limit.
  The runner now reserves at least 32GiB total and never raises an inherited
  hard limit. These are harness failures; the repeats are resubmitted.

- Temporal-score and turn-field variants: control3,127; progress discounts
  0.95/0.85/0.7/0.5 give 3,041/3,087/3,006/2,809. Static load-weighted turn
  costs (mean preserved) give 2,973–3,031. Leave both options off.
- The progress audit checks every frontier row against its exact result
  timestamp, full-run validity, binary hash, and compiled planner sources at
  the linked Git commit. See `random05/results/progress-audit.json`.

- Blocked-cycle preparation with larger portfolios gives 3,151 at K1024 and
  3,217 at K2048, versus 3,231/3,205 without it. It is an optional candidate
  policy, not a reliable improvement across budgets and seeds.

- Combined scheduler/search validation: K1024 with exact/oriented matching and
  keep bonus0.5 reaches **3,299**; greedy keep0.5 gives3,212 and adding noise400
  gives3,225. A baseline/blocked-cycle portfolio at K2048 gives3,289. All use
  32 workers and complete with zero errors/timeouts. Record the3,299 maximum
  without treating it as a replicated average gain.
- Four-core cycle thresholds0/0.5/2/4/8 give3,071/3,052/3,043/2,968/2,931;
  gain penalties0.25/0.5 give3,010/3,057. The K128 policy mixture gives3,122.
  None improves the four-core maximum3,127.
- Implementation efficiency: share candidate-cost sorting and priority order
  between spatial-intention PIBT and executable PIBT. Full trajectory identity
  and before/after latency will be checked on GRID before claiming a speedup.

- Shared-sort optimization: the K64 exact/guided case repeats all2,000 steps
  byte-for-byte in trajectory (3,109 tasks). Mean latency drops from47.4ms to
  35.1ms on the same EPYC9354 allocation, a25.8% reduction. The blocked-cycle
  control also repeats3,127 with the exact previous trajectory. K1024 is still
  running on four physical cores; no score or deadline claim yet.
- K4096 with the preceding greedy scheduler gives3,263; mutation0.05/0.1 gives
  3,218/3,230 and noise400 gives3,223. Those runs used EPYC7532, so their timing
  is not comparable to the EPYC9354 latency figures. None beats3,299.

- Thirty-two more generated fields (seeds16–47), screened with full2,000-step
  K128 exact/guided matching runs:2,698–3,038 versus3,073 for field15 control.
  All complete without errors/timeouts. Keep field15; validate four leading
  alternatives at K1024 because smaller-portfolio rankings have been unreliable.
- Added an independent final directional penalty so guidance contrast can be
  changed while preserving the traffic-assignment street layout. Default
  behavior is unchanged; cost-field variants remain explicit RANDOM-05 tricks.

- Four-core K1024 completes at3,299 with the exact same trajectory as the
  32-worker frontier. Mean378ms, maximum486ms, so this stronger search fits
  the strict one-second budget with substantial room. NMS four-worker repeats
  finish at2,914 and2,902 versus2,903 initially. Promote3,299 to both `best.json`
  and `best-four-cores.json`; retain `best-32-workers.json` separately.
- Action diagnosis for that trajectory:484,180 forward moves and481,329 turns,
  versus NMS32's520,780 forward moves and574,518 turns. Tasks per500 steps are
  767/831/734/967 versus975/750/754/693. Early throughput remains behind; the
  known-horizon result benefits heavily near the end. Do not infer that moving
  more robots alone is the right optimization target.

- Completed K1024 timing pair: both original and shared-sort builds score3,299
  with identical trajectories. Mean latency falls500.4→377.8ms (24.5%); maximum
  falls605.7→486.3ms on matching four-core EPYC9354 allocations.
- Fixed-layout contrast at K128: control3,073; penalties0.4/0.8/1.2/2.4/3.2/4.8
  give2,636/2,796/2,859/3,199/3,155/3,063. Turn cost0.3 gives3,148 and1.2 gives
  2,968. The2.4 contrast is a4.1% gain in this controlled pair; validate it at
  the production search budget before promoting it.
- Next general search experiment: split the same fixed rollout budget into
  generations that mutate the current best priorities. One generation preserves
  the original algorithm. It remains disabled until measured; dense regression
  checks worker-count determinism for multiple generations.

- Stable insertion sorting repeats the exact 3,299 trajectory at mean 334.5 ms,
  maximum 437.8 ms. The preceding shared-sort implementation took 377.8 ms
  (11.5% reduction). Planner seeds 0–4 score 3,299/3,185/3,284/3,214/3,061,
  mean 3,208.6. K3072 exceeds the strict deadline at step 0 (1,084.8 ms) and
  is excluded from valid throughput results; K2048 remains in progress.
- Alternative fields at K1024: seeds17/22/31/47 give3,187/3,180/3,052/3,084.
  Keep field15. Two generations at total K128 give3,182 versus3,073 control
  (+3.5%); four/eight/sixteen generations give3,076/3,074/3,065. Larger-K
  generation tests are still running, so there is no promotion yet.
- Added one-job-per-case submission (`tools/split_grid.py`) for mixed-duration
  batches. Each solver retains its full physical-core allocation; completed
  cases release their resources immediately instead of waiting for the batch.

- Production-budget contrast validation: K1024 with final penalty2.4 scores
  **3,363**, versus3,299 control (+1.9%), a15.4% lead over the strongest NMS4
  repeat. Penalty3.2 gives3,351; turn0.3 gives3,330; penalty2.4 plus turn0.3
  gives3,315. All valid. Keep the single stronger-contrast change; combining
  separately promising changes was worse than either alone.

- The larger K2048 portfolio gives 3,262 with contrast1.6 and 3,343 with
  contrast2.4, below each corresponding K1024 best. Multi-generation search
  also loses at K1024: G2=3,181 and G4=3,234 versus3,299 control. Keep it off;
  the promising smaller-budget result did not transfer.
- With contrast2.4 at K128, static load multipliers0/0.25/0.5/0.75/1/1.5 give
  3,013/3,101/3,190/3,199/3,217/3,221. The last two need production-budget
  validation; no frontier promotion from this small gain.
- Next objective test separates policy guidance from evaluation: retain lane
  preferences for selecting moves, but blend the rollout score with exact
  unit-action distances over all remaining task stops. Extra cost tables fit
  comfortably in the32GB allowance. Regression tests check that identical
  metrics reproduce the same trajectory and that blends are worker-invariant.
  Runtime and throughput validation are pending; the feature defaults to off.

- Contrast2.4 across five planner seeds: 3,363/3,276/3,344/3,329/3,312, mean
  3,324.8 versus3,208.6 for contrast1.6 (+3.6%, all five pairs positive).
  Against the strongest NMS4 repeat, the best is+15.4% and the mean is+14.1%.
  The direct-recursion build reproduces the seed0 trajectory at mean322.5ms,
  maximum431.3ms. Keep the original producing commit linked to the frontier.
- The plain-action evaluation blend loses at K128 and K1024. At K128, blends
  0/0.25/0.5/0.75/1 give3,199/3,161/3,123/3,111/3,156. At K1024, blends0.25/0.5
  give3,326/3,296 versus3,363 control. Keep the weighted evaluation unchanged.
- New optional coordination policies decompose the spatial intent into complete
  movement chains and cycles. Mode1 executes only fully ready components;
  mode2 preserves ready components and fills the rest with kinematic PIBT.
  Dense turnover and worker-determinism regressions pass. Full-map throughput
  validation is pending; mode0 retains the current solver.

- Independent per-step random streams reach **3,374** at K1024 versus3,363
  with the preceding stream. This11-task maximum is not evidence of a mean
  improvement yet. The mode keeps global candidate prefixes and local-refinement
  random draws independent of the rollout count, improving budget comparisons.
- Production-budget load multipliers1/1.5 give3,281/3,307, below3,363 control;
  depth6 at K1024 gives3,234. Again, small gains at K128 did not transfer.
- Component-policy screens at K128: baseline3,199; ready-components-only1,663;
  ready components plus kinematic PIBT3,101. Full-budget checks remain running.
- Added a fixed reference for guidance normalization. Changing counterflow
  penalties can now preserve preferred-direction, turn, wait and matching cost
  scales. Unit tests verify that an equal reference reproduces old weights
  exactly and that stronger counterflow prices leave preferred edges unchanged.
  Full-map fixed-scale contrast validation is pending.

- Prefix audit: among 26 distinct valid K1024 four-core trajectories above2,800
  tasks, rank correlation with final throughput is0.22 at250 steps,0.75 at500,
  and0.81 at750. Selecting the top third at500 steps retains only7 of10 runs
  within2% of the best. This retrospective sample does not justify replacing
  full2000-step validation. Evidence: `random05/results/prefix-audit-v25.json`.
- Intent-component policies lose: executing only fully ready spatial components
  gives1,694 atK1024; pinning ready components before ordinary PIBT gives3,266,
  versus3,363 for the unchanged policy. Both remain disabled.
- Fixed-scale guidance separates counterflow price from preferred-edge scale.
  AtK128, penalties2.4/3.2/4.8/8/16 give3,199/3,213/3,239/3,146/3,027.
  FullK1024 validation is required; earlier low-K rankings often did not transfer.
- New diagnostic experiment: a nonnegative score penalty for consecutive
  opposite turns. The old3,299 trace contains58,071 such reversals. The penalty
  includes the previous real action at the rollout boundary; the default is0.
  This is a hypothesis about wasted rotations, not yet a measured improvement.

- Fixed preferred-edge scale, output penalty4.8: fullK1024 gives3,362 versus
  the matched control3,363. The low-K gain does not carry through. Keep the
  existing contrast2.4 configuration.
- Experimental regional priority search: keep25% of candidates fully global,
  but optionally limit other mutations to a local rectangle. This targets the
  hypothesis that changing240 unrelated priorities at once loses useful local
  repairs. Radius0 preserves the existing algorithm. Full comparisons pending.
  `random05/RESEARCH.md` records this and the inspected EPIBT follow-up.
- Efficiency candidate: resolve each robot's active task-cost row once per
  simulated step, removing repeated stage/cache checks from neighbor ranking.
  The arithmetic and candidate ordering are unchanged. Full trajectory identity
  and measured latency are required before claiming a speed improvement.
- Local/rotation tests (K1024, four cores, per-step RNG): control repeats3,374;
  reversal penalties0.1/0.3/1 give3,335/3,252/3,123. More local refinement
  (25 trials) gives3,294; mutation fractions0.1/0.03 give3,310/3,148. These
  changes remain off. Penalizing this signal did not improve completed-task throughput.
- Regional mutation on32 workers: unchanged control repeats3,374; radius2/4/8
  with mutation0.3 gives3,195/3,217/3,348; radius2/4 with mutation1 gives
  3,224/3,222. All complete without errors or timeouts. Limiting most changes
  to one region does not improve this configuration. Evidence is in
  `random05/results/regional-mutation-split-full-v27/`.

- Local100 finishes at3,294, the same task count as local25 and below control
  3,374. More local priority trials do not recover the gap. All8 local/rotation
  cases are complete and valid.

- Initial500-step diagnosis: ours3,379 visits3,393 stops and finishes779 tasks;
  strongest NMS4 visits3,346 and finishes879. Forward actions are nearly equal
  (123,634 versus124,058). Ours completes407 two-stop tasks versus NMS486.
  This motivates a startup task-length preference, not a claim that task choice
  explains the whole gap. The new phase-specific experiment is guarded by
  `--trick RANDOM-05`; opened tasks remain locked. Default behavior is unchanged.
  The full3,379 run actually finishes more five-stop tasks (522 versus346).

- Active-cost-row validation: the complete control stays3,374 with identical
  actions, schedules and task events. On four cores, measured mean latency is
  309.6ms versus325.6ms for the preceding implementation; maxima424.5/434.9ms.
  This is one fixed-work pair on shared EPYC9354 hosts. The cached/uncached
  regression across task changes also passes. Evidence:
  `random05/results/active-cost-split-full-v28/control-equivalence.json`.

- Startup scheduling screen at fullK1024/32 workers, planner seed3: control3,379;
  constant length weights0.5/1/2 give3,342/3,294/3,139. Initial weight1 for250
  steps gives3,330. Thus the early task-count deficit alone does not establish
  that a stronger length preference improves total throughput. The remaining
  phase settings also lose: initial2 for250 steps=3316, initial1 for500 steps=3367.
  All7 full runs are valid; the original configuration remains best.

- New guidance experiment: reverse a small, seeded subset of directional cost
  pairs after generating field15. This preserves every physical edge, positive
  costs, pair cost sums and turn costs. It requires flow guidance and therefore
  `--trick RANDOM-05`; count0 preserves the control. Evaluate fullK1024/full
  horizon because lower-work rankings have repeatedly failed to transfer.

- Added an optional three-step operation policy (`R05_OPERATIONS=3`) beneath
  the existing search and scheduler. It can revise inherited multi-action plans
  with bounded revisits while retaining a certified fallback. Regressions cover
  suffix inheritance, half-turns followed by a fully occupied cycle, prohibited
  edge swaps, dense task replacement and worker-count determinism. Full-map
  performance is pending; the existing pipeline remains selected by default.

- Operation-policy review found that a generic rotation-before-wait tie rule
  could prefer a turn immediately undone before any movement. Candidate
  generation now excludes opposite turns without intervening forward motion
  and redundant three-quarter-turn sequences. An immovable two-cell regression
  requires waiting without swaps or futile rotations. Geometry retains all
  action encodings so shifted inherited plans remain representable.

- One-visit operation ablation stalls in the dense toy case and is rejected as
  a performance candidate. The selected four-visit policy passes the same
  mobility test, including deterministic trajectories across worker counts.
  Its full-map performance is still unmeasured.

- Local guidance search, fullK1024/32 workers, planner seed3: control3,379.
  Reversing1/4/16 directional pairs gives maxima3,395/3,365/3,306 across
  eight flip seeds each. Only one candidate beats control, by16 tasks (+0.47%).
  It uses one flip with seed5. Record the maximum, while keeping replication
  separate from selection. Four-core confirmation and seeds0/1/2/4 are queued.
  All25 full runs finish without errors or timeouts. Evidence:
  `random05/results/guidance-local-split-full-v31/`.

- Initial operation-policy full runs: K1=152, K8=1,018. Both are valid but far
  below the current pipeline. Larger portfolios and policy ablations are still
  running. The three-step policy remains experimental and disabled by default.

- Guidance replication: seed3 repeats3,395 on four cores, with identical actions,
  assignments and events to32 workers. Mean312ms/max425ms, peak RSS294MB, valid
  full2000 steps. Other planner seeds0/1/2/4 give3,356/3,311/3,325/3,305;
  five-seed mean3,338.4 versus3,339.8 before. The selected best improves by16
  tasks; this field does not improve the replicated average. The frontier JSONs
  retain the selected maximum as requested.

- All operation-policy v34 cases are complete and valid. K1/8/32/128 score
  152/1,018/1,019/1,246. AtK32, travel-cost weight1 gives823, no inheritance624,
  and revisit limit16 gives1,886. The unchanged pipeline control repeats3,379.
  Increased revisits help this prototype, but it is still far from the frontier.
  Source b72a51e; results in `random05/results/operation-policy-split-full-v34/`.
  v33's added one-visit mobility ablation failed; v34 rejects that ablation and
  retains the original collision and four-visit/no-inheritance mobility tests.

- v35/source0754ed8 adds optional moving-footprint search, inspired by the
  reference's exclusion of all-wait from active candidates. The inherited plan
  remains available as fallback. Dense regressions pass; full comparisons are
  running. Separately, generated field seeds1–16 are being compared at fullK1024
  with planner seed3; the original field selection used much less search work.

- Partial v35 results (five of six full runs): moving-footprint K1/8/32 gives
  343/817/1,118; K32 cost1=743 and no inheritance=577. These do not approach
  the pipeline. K128 remains queued/running. The generic operation prototype
  has not reproduced the reference algorithm's reported performance.

- Full-budget field revalidation is complete: field15 remains best at3,379
  without the local flip; the other15 fields range3,012–3,270. The existing
  field choice survives the stronger K1024/planner-seed3 comparison. All16 runs
  are valid. The moving-footprint operation K128 case finishes at711; all6
  operation ablations are complete, valid, and below the pipeline frontier.

- Source369e2df/v36 adds an optional whole-task completion bonus in the rollout
  score. The contested-goal regression passes; full bonus0/2/4/8/16/32 runs are
  queued. Separately, the frontier is being measured with no horizon cutoff and
  cutoff scales0.75–3. These are full-horizon comparisons, with unchanged tasks,
  guidance, planner seed and allocation.

- Optional known-pool task turnover in rollouts tests the same completion
  diagnosis more directly. Finished simulated agents can take currently visible
  free tasks; a per-rollout mask prevents duplicate use. Newly assigned cost is
  rebased, and actual schedules remain unchanged. This forecast does not see
  hidden future tasks or rematch all unopened assignments. No gain is assumed;
  regression and full-run evidence are required.

- Full cutoff comparison, unchanged best field/planner seed3/32 workers:
  no cutoff3,197; scales0.75/1/1.25/1.5/1.75/2/2.5/3 give
  3,372/3,380/3,367/3,395/3,367/3,335/3,236/3,117. All9 are valid. Scale1.5
  remains selected. This is a198-task (6.2%) contribution on this seed, separate
  from the method's performance without knowing the run length. Sourceb824f5d.

- The cutoff-free3,197 trajectory repeats exactly on four physical cores,
  mean316ms/max421ms and peak RSS293MB. This is+9.7% over matched NMS4=2,914,
  and+0.8% over matched NMS32=3,172. Guidance remains a declared map trick.
  Worker-equivalence evidence is in `random05/results/no-horizon-four-split-full-v31/`.

- Completion reward full results, bonus0/2/4/8/16/32:
  3,395/3,281/3,327/3,289/3,253/3,054. All6 runs valid, every bonus loses.
  Keep the option off. Source369e2df, results under
  `random05/results/completion-reward-split-full-v36/`.

- Visible-task turnover full results, K256/512/1024:3,254/3,225/3,335. Adding
  completion bonus8 atK1024 gives3,379; without the horizon cutoff it gives3,122
  versus the no-cutoff control3,197. All6 runs including control are valid.
  Keep turnover off. The default3,395 control has identical actions, schedules
  and events acrossv31/v36/v37. Source5db6827, evidence under
  `random05/results/future-tasks-split-full-v37/`.

- Fresh validation is declared before results in `random05/FRESH_VALIDATION.md`.
  Freeze sourceb824f5d and its3,395 configuration, then test new generator seeds
  50001/50002 with two NMS4 repetitions each. All six jobs use full2000 steps,
  checked EPYC9354 four-core allocations,1s deadlines and32GB limits. These are
  new task/start instances, not extra planner seeds on the archived input.

- Larger operation revisit budgets remain below the pipeline: revisit64/K32
  gives1,973, revisit128/K32 gives1,929, moving-only/revisit64/K32 gives1,890,
  and revisit64/K128 gives2,106. All4 full runs are valid; this prototype is not
  competitive. Longer pipeline look-ahead10/12/16 gives3,302/3,284/3,199, all
  below3,395 atdepth8. Full priority-noise100/400/800 gives3,349/3,335/3,348;
  retain200. These comparisons are now complete and archived.

- Independent replay of four full trajectories verifies every move, goal event,
  task lock and robot step. On3,049 task IDs completed by ours and NMS32, ours
  spends35.26 more waiting steps per task; turns are3.53 fewer and forward
  steps4.66 more. Against NMS4 on2,860 common tasks, waits are25.77 more but
  turns15.50 fewer. Assignments and congestion histories still differ; this is
  diagnosis, not a recoverable-throughput estimate. See
  `random05/results/action-audit-v39/REPORT.md` and its hashed replay evidence.

- Source56b0970/v40 adds optional early forward augmentation. Already aligned
  idle chains/cycles may move immediately while all previous forward promises
  remain fixed. Moves must have positive aggregate potential gain and obey
  single-step rotation rules; swaps are rejected. Ready-cycle, swap and dense
  worker-determinism regressions pass. The option remains off pending full
  results; it changes the motion policy rather than adding a score bonus.

- Early forward augmentation full results are complete: thresholds 0/1/4 give
  3,369/3,355/3,381 versus the unchanged 3,395 control. K256 gives 3,332;
  without the horizon cutoff, K1024 gives 3,101 versus 3,197. All six runs are
  valid. Keep the feature off. The default actions, schedules and events match
  the original frontier exactly. Source [56b0970](https://github.com/fywu85/lorr/commit/56b0970);
  [full evidence](random05/results/early-fill-split-full-v40/summary.json).

- At 2026-09-20 13:02 UTC, two fresh-instance candidate runs have finished:
  seed 50001 = 3,386 and seed 50002 = 3,178, both valid. The four matched NMS
  repetitions are still running, so no fresh-instance gain is claimed yet.
  Separately, expand the development guidance search to field seeds 17–32 at
  full K1024, and recheck six priority/push policies previously screened only
  at K64. These jobs use the archived development input; the fresh candidate
  and its validation inputs remain frozen.

- Fresh validation is complete: candidate 3,386/3,178 on seeds50001/50002;
  NMS repeats 2,920/2,957 and 2,898/2,915. Gains versus each stronger reference
  are14.5%/9.0%; aggregate11.8%. The audit checks frozen settings, binary hashes,
  generated input hashes, physical cores, CPU model, deadline validity and RAM.
  See the separate validation table above. The development frontier is unchanged.

- Full-budget field seeds17–32 give
  3,283/3,139/2,677/2,825/3,222/3,244/3,225/3,098/
  3,137/3,107/3,082/3,068/2,987/3,080/3,176/2,875.
  All16 are valid; none improves field15. Some fields were previously screened
  at smaller budgets or with older settings; these are the current seed3/K1024
  comparisons. Evidence: `random05/results/field-expansion-split-full-v31/`.

- A new declared guidance trick tests confidence-weighted opposing-edge costs:
  soften a preferred direction when aggregate demand is nearly balanced, while
  preserving physical edges and direction ordering. Exponent0 preserves the
  old field. Direction/cost invariants and the full engine regression pass in
  build-v41. Full production-budget comparisons are required before promotion.

- The full-budget priority/push recheck also loses: rollout aging3,333,
  age caps100/200 give3,337/3,352, push prices1/2 give3,117/3,121, and prospective
  idle-turn costing gives3,331. All6 full runs are valid; keep the options off.
  [Evidence](random05/results/frontier-policy-split-full-v31/summary.json).
  The confidence-scaling control at exponent0 reproduces the3,395-task run's
  actions, schedules and events exactly. The four nonzero exponents are still running.

- Confidence scaling is complete and rejected: exponents0/0.25/0.5/1/2 give
  3,395/3,295/3,200/3,153/2,899, all valid. Source
  [ffe75c7](https://github.com/fywu85/lorr/commit/ffe75c7),
  [full evidence](random05/results/flow-confidence-split-full-v41/summary.json).
  Exponent0 reproduces the prior actions, schedules and events exactly. Keep0;
  none of this round's policy or field experiments changes the verified frontier.

- Sourcea6ad284/v42 tests repeated continuation evaluation: each first decision
  is held fixed while later priorities vary; scores are averaged. Default1
  preserves the earlier method. Dense turnover, common-root equivalence and
  worker-count checks pass. At K1024/local0, one/two/four/eight continuations
  give3,254/3,271/3,400/3,422. The full frontier control repeats3,395. Larger
  budgets and delayed/milder mutations remain in progress. Four-core and seed
  checks are queued; no replicated improvement is claimed yet.

- All ten initial continuation runs are complete and valid. Keeping the root
  priorities for two simulated steps gives3,450 atK1024/B4. Milder future
  mutation0.1 gives3,388. AtK2048/local0, single-continuation search gives3,240
  while four-continuation averaging gives3,492. This is+7.8% at equal global
  rollout work and+97 tasks over the previous selected maximum3,395. Four-core
  confirmation is queued. Additional seeds and scale comparisons are underway;
  this new regime is not yet validated on fresh task/start instances.

- Five planner-seed results atK1024/local0: B4 gives
  3,379/3,393/3,383/3,400/3,408 (mean3,392.6); B8 gives
  3,451/3,447/3,328/3,422/3,440 (mean3,417.6). The preceding configuration gives
  3,356/3,311/3,325/3,395/3,305 (mean3,338.4). Both new configurations improve
  all five pairs, by1.6% and2.4% on average. These remain planner seeds on the
  same development input, not fresh-instance validation. B4 local rollout
  allowances4/16/64 give3,419/3,366/3,370; additional local work does not help
  consistently. The overall best remains the single-seedK2048 result3,492.

- Shared-prefix reuse passes dense regressions that preserve task turnover,
  score accounting and worker-count determinism. Full controls will compare
  against the3,492 and3,450 trajectories, plus four-core latency. No timing or
  throughput improvement is claimed from the optimization before those runs.

- Four-core confirmation: B4/K1024 reproduces3,400 and B8/K1024 reproduces3,422,
  with every action, assignment and event equal to the corresponding32-worker
  run. Mean317/322ms, maxima431/435ms, peak RSS283/282MB; all valid. The four-core
  frontier is now3,422 (+17.4% versusNMS4). K2048 confirmation is still running.

- Continuation scaling reaches **3,501** at K2048/B8 with mutation starting after
  two steps; K2048/B4/start2 gives 3,427 and K2048/B16/start1 gives 3,485.
  All are valid 32-worker runs. Larger runs remain in progress. The four-core
  shared-prefix K4096/B8/start2 experiment exits with code 124 at step 0 after
  1,133 ms, so it is excluded from the frontier. Fixed-work search did not return
  a partial solution. Its failed-run metadata is preserved.

- The three-step operation-policy comparison is complete: control 1,973,
  successful-path protection 2,099, terminal-turn exclusion 1,742, both 1,820,
  and both with moving-only repair 2,008. All five runs are valid, but remain
  far below the pipelined solver. No operation variant is promoted.
  [Evidence](random05/results/operation-reference-rules-split-full-v43/summary.json).

- K4096/B4/start2 reaches **3,555 tasks** on 32 workers, a new selected maximum
  (+12.1% versus matched NMS32). K4096/B4/start1 gives 3,477. The corresponding
  four-core feasibility remains unverified. Shared-prefix reuse reproduces every
  action, schedule and event of the 3,492 and 3,450 controls on full runs.
  [Optimization equivalence](random05/results/shared-prefix-split-full-v44/shared-k2048-b4-equivalence.json).

- The 3,492 trajectory is now reproduced on four cores, with and without exact
  prefix reuse. Every action, assignment and event is identical across both
  implementations and worker allocations. The four-core frontier rises to
  **+19.8% versus matched NMS4**; timing details and linked evidence are above.

- K2048/B8/start2 across planner seeds 0–4 gives 3,478 / 3,443 / 3,522 / 3,501 /
  3,472, mean **3,483.2**, versus 3,338.4 for the preceding configuration (+4.3%;
  all five pairs positive). Same development input, not fresh-input validation.
  Start3/4 gives 3,449/3,389; future mutation 0.1/0.6/1.0 gives 3,314/3,460/3,484;
  B4/start2/mutation0.6 gives 3,523; depth6/10 gives 3,284/3,503. None exceeds
  the overall 3,555 record. The optimized 3,501 control is exactly equivalent.
  Its four-core run on research58 exits 124 at step 3 (1,068 ms), so it is not
  counted as a four-core success.
  [Full comparison](random05/results/continuation-refinement-split-full-v44/summary.json).

- Two exact optimizations pass the full regression suite in build-v45: packed
  priority ordering preserves ties and signed-zero behavior; sparse/hole-based
  dispersion counting preserves the original geometric pair count. Both default
  off until full-map timing and trajectory comparisons. These change evaluation
  cost, not the search objective or map policy.

- The new continuation method preserves the main guidance settings: turn costs
  0.4/0.8/1.0 give 3,480/3,451/3,394; noise 100/400 gives 3,452/3,460;
  contrast 1.6/3.2/4.0 gives 3,395/3,482/3,499; no directional flip gives 3,500.
  Fields 5/17 give 3,236/3,332, and the imported NMS field gives 2,961. All 12
  full runs are valid, none beats the 3,501 control at the same K2048/B8 budget.
  [Evidence](random05/results/continuation-coupling-split-full-v44/summary.json).

- The independent event-checkpoint audit finds the 3,492 and 3,555 trajectories
  already ahead of the preceding solver at step 1,000 (1,680/1,705 versus 1,645)
  and step 1,500 (2,480/2,505 versus 2,402). This describes when gains appear;
  it does not identify their cause or validate short prefixes for selecting runs.
  [Checkpoint evidence](random05/results/continuation-progress-audit-v45/checkpoints.json).

- The progress audit now explicitly verifies matching planner/preprocessing
  budgets, maximum entry latency <=1s and peak RSS <=32 decimal GB, in addition
  to full-run validity, input hashes, CPU allocation and producing source.
  All 46 recorded frontier rows pass. Failed deadline runs remain separate.

- All four v45 optimization cases reproduce the complete 3,555 trajectory,
  including actions, assignments and events. Mean 32-worker latency is 194 ms
  for the control, 269 ms for packed sorting, 192 ms for sparse dispersion and
  173 ms for both. Packed-only uses less CPU time despite greater wall time;
  these shared-host timings are noisy, so do not attribute every difference to
  the code. Combined CPU time is about 11% lower in this pair.
  [Full equivalence and timing](random05/results/hotpaths-split-full-v45/equivalence.json).

- Reused policy buffers pass full regressions, but K4096/B4, K4096/B8 and
  K3840/B4 still exceed the first-step four-core deadline at 1,084/1,042/1,019 ms.
  Those failures remain excluded. K3584/B8 is running. A new optional search
  experiment penalizes variation among continuation scores; zero preserves the
  mean. Regression checks pass; no throughput gain is yet claimed.

- The optimized K2048/B8/start2 configuration completes the full strict four-core
  run with **3,501 tasks**, exactly reproducing all actions, schedules and events
  from the 32-worker run. The four-core record is now **+20.1% versus NMS4**.
  Its earlier failed four-core attempt used the preceding implementation and a
  different shared host; retain both records rather than attributing the whole
  timing difference to the optimization.

- Full v46 controls with and without reusable policy arrays both reproduce all
  3,555 actions, schedules and events. This establishes semantic equivalence;
  small shared-host timing differences do not establish a large extra speedup.
  [Equivalence](random05/results/scratch-reuse-split-full-v46/equivalence.json).

- K8192/B8/start2 reaches **3,596 tasks**, a new 32-worker record (+13.4% versus
  matched NMS32). K8192/B4 gives 3,439, so increasing candidate count alone does
  not reliably help. The four-core K3072/B4 intermediate gives 3,454; retain the
  smaller, higher-scoring 3,501 four-core configuration. All these full runs are
  valid. The 3,596 result has not been reproduced on four cores.

- Continuation variance penalties all lose: at K2048/B8, weights 0/0.25/0.5/1/2
  give 3,501/3,332/3,390/3,305/3,386; at K4096/B4 they give
  3,555/3,454/3,499/3,392/3,369. Both zero controls preserve every action,
  assignment and event. Keep risk zero. Two/four/eight search generations at
  K2048/B8 give 3,512/3,520/3,482; the small four-generation gain is awaiting
  four-core confirmation and is not a replicated improvement.
  [Evidence](random05/results/continuation-risk-split-full-v47/summary.json).

- A second fresh-input validation is frozen at commit
  [e6b2dbe](https://github.com/fywu85/lorr/commit/e6b2dbe), before generating seeds
  50003/50004. It tests the confirmed 3,501 four-core configuration from e896201
  against two NMS repetitions per input. All six jobs have been submitted under the
  existing 1s/32GB rules. The earlier 50001/50002 inputs remain excluded from
  tuning, and development continues on the original archived input.
  [Predeclared protocol](random05/FRESH_VALIDATION_V2.md).

- The K3584/B8 four-core run completes with **3,509 tasks** (+20.4% versus NMS4),
  eight more than the preceding record, at mean 785 ms and maximum 930 ms.
  The smaller 3,501 configuration remains frozen for fresh-input validation V2.

- K8192/B8 across planner seeds 0–4 gives 3,528/3,579/3,457/3,596/3,580,
  mean **3,548**, versus 3,483.2 at K2048/B8 (+1.9%; four of five pairs improve).
  These are 32-worker runs on the same input; four-core feasibility is not
  implied. K8192/B16 gives 3,399. Larger K16384 runs are still in progress.

- Build-v48 adds optional phase timing for assignment, task-cost preparation,
  candidate generation, look-ahead and final checks. Full regression tests pass,
  including identical decisions with profiling on/off. Short diagnostic runs
  will use a relaxed deadline and will never enter the full-throughput frontier.

- K16384/B8 completes **3,657 tasks**, a new 32-worker record (+15.3% versus
  matched NMS32), at mean 607 ms / maximum 726 ms. K16384/B16 and B32 give
  3,551 and 3,594. Wider branching is not uniformly better. All three are valid
  full runs; none has four-core feasibility established.
  [Evidence](random05/results/continuation-larger-split-full-v46/summary.json).

- Four generations at K2048/B8 reproduce **3,520 tasks on four cores**, with
  exactly the same complete trajectory as 32 workers. Mean 456 ms / maximum
  571 ms and RSS 287 MB. This is a small selected gain, not a replicated claim.
  The predeclared fresh-input candidate remains the older 3,501 configuration.

- Timing diagnosis shows K4096/B4 spends about 963 ms in first-step look-ahead,
  93 ms in matching and 12 ms building task costs. Subsequent matching is about
  1 ms. Greedy startup matching is slower (115 ms), so it is not adopted.
  These are 120-step profiles with a 10s guard, excluded from the throughput
  table. A tested optional map-only distance cache now shares final-errand cost
  rows and guided approach distances; full timing/equivalence checks are next.
  [Profile evidence](random05/results/phase-profile-split-prefix-v48/timings.json).

- The user approved the exact prepared Fable payload. The CLI invocation used
  Fable 5.1 / max effort with tools disabled and the reserved persistent session,
  but the provider returned an out-of-usage-credits error (zero billed usage).
  No review was produced. Keep the approved payload/session for a later retry;
  local development and benchmark evidence do not depend on that review.

- Fresh validation V2 completes: 3,494 versus NMS 2,946/2,984 on seed50003;
  3,387 versus NMS 2,892/2,875 on seed50004. Both matched gains are 17.1%.
  All six full runs pass source, input, CPU allocation, latency and RAM audits.
  No later candidate was substituted into the predeclared comparison.

- Shared goal-distance caching preserves every action, assignment and event
  in the controlled 120-step profiles. First-step matching matrix construction
  falls from 28.6 to 11.6 ms; assignment solve time remains 62.8 ms, and the
  approximately 970 ms look-ahead dominates K4096/B4 on four cores. Full
  semantic/timing controls are running. Build-v50 adds sampled internal policy
  timing so the next CPU change targets a measured cost; regressions pass.
  [Cache profile](random05/results/goal-cache-profile-split-prefix-v49/timings-equivalence.json).

- K2048/B8 with four generations across planner seeds 0–4 gives
  3,562/3,499/3,514/3,520/3,562, mean **3,531.4**, versus 3,483.2 with one
  generation (+1.4%; four of five pairs improve). All are full 32-worker runs
  on the development input. The two 3,562 trajectories and K3584/generation2
  (3,552 tasks) are now undergoing strict four-core confirmation.
  [Evidence](random05/results/generation-scaling-split-full-v47/summary.json).

- The shared-goal cache controls both complete 3,520 tasks on 32 workers;
  every action, schedule and event matches the preceding implementation.
  Four-core validation remains in progress. Sampled policy timing locates
  approximately 24% of elapsed policy work in candidate ranking and 17% in
  priority sorting; these samples guide optimization, not causal CPU estimates.
  Build-v51 adds optional stable radix ordering with exact signed-zero/tie
  preservation and finite-value fallback. Ordering and dense task-turnover
  regressions pass; controlled timing profiles are next.
  [Full cache equivalence](random05/results/goal-cache-split-full-v49/equivalence.json),
  [policy profile](random05/results/policy-profile-split-prefix-v50/timings.json).

- Exact radix ordering preserves all actions, assignments and events in the
  controlled prefix. Sampled ordering time drops about 70%, and look-ahead
  wall time about 12% (971 to 850 ms initially, 952 to 830 ms at step100).
  These shared-host prefix timings motivate full strict-deadline checks, now
  running; no full four-core feasibility claim is made yet.
  [Evidence](random05/results/radix-profile-split-prefix-v51/timings-equivalence.json).

- Build-v52 adds an optional bounded per-worker candidate-ranking cache.
  Its key includes the current real-step epoch, robot, task-chain identity,
  task stage, position, heading and moving/idle state. Occupancy-dependent
  push costs bypass it. Priorities and collision resolution remain dynamic.
  Dense task turnover, virtual-task assignment, alternative wait/rotation
  policies, push-cost fallback and worker-count equivalence regressions pass.
  The cache adds approximately 157 MB at 800 robots / 32 workers. Defaults
  remain off pending full timing and trajectory checks; profiles are next.

- All three full goal-cache controls finish with 3,520 tasks and identical
  actions, assignments and events across cache settings and worker counts.
  K8192/four generations gives 3,582 and K16384/four generations gives 3,495,
  below the corresponding one-generation scores 3,596 and 3,657. More search
  generations help the smaller portfolio but do not scale uniformly.

- The ranking-cache prefix preserves the exact trajectory. With radix sorting
  enabled in both cases, look-ahead drops from 889 to 712 ms initially and from
  878 to 677 ms at step100. The three K5120/B8 four-core full runs have passed
  the first-step deadline; full validity/throughput are still pending. The
  radix-only 32-worker full control finishes 3,555 with an unchanged trajectory.
  [Cache timing evidence](random05/results/ranking-cache-profile-split-prefix-v52/timings-equivalence.json).

- **Trick: RANDOM-05 guidance search.** Extend the 32 previously sampled flow
  layouts with seeds33–48, plus the existing field15 control, at the full
  K2048/B8/four-generation budget. All 17 runs use explicit `--trick RANDOM-05`
  and the archived development input. This is map-tuned layout selection,
  separate from the general exact caching/sorting changes. Fresh-validation
  inputs remain excluded. No new layout is promoted before full results.

- Both selected four-generation planner seeds (0 and 4) reproduce **3,562
  tasks on four cores**, matching their complete 32-worker trajectories.
  The confirmed record rises to **+22.2% versus NMS4**. Both are valid full
  strict-deadline runs. This does not replace or extend the frozen fresh-input
  result of +17.1%; the predeclared candidate remains source e896201 / seed3.

- All 16 newly sampled guidance layouts (seeds33–48) lose to the existing
  field15 control at the same full K2048/B8/four-generation budget. The best
  new field is47 at 3,444 versus 3,520 for field15. All17 runs are valid.
  The field15 control preserves every action, assignment and task event through
  the goal cache, radix ordering and ranking cache. Keep field15.
  [Full results](random05/results/flow-expansion-33-48-split-full-v52/summary.json),
  [control equivalence](random05/results/flow-expansion-33-48-split-full-v52/control-equivalence.json).

- Build-v53 permits a finite signed continuation coefficient: positive values
  retain the previous variance penalty, zero retains the exact mean, and
  negative values test a bonus for promising variable futures. This tests
  whether mean random continuation outcomes undervalue the next real step's
  re-optimization. Every configured rollout still runs to completion; this
  changes the scoring rule, not deadline handling. Zero-variance and worker
  equivalence regressions pass. Eight full four-core A/B cases are prepared:
  coefficients0/-0.25/-0.5/-1 with one/four generations, seed3, same K2048/B8.
  No throughput benefit is assumed before these results.

- K3584/B8/two generations also completes on four cores with 3,552 tasks,
  exactly matching its 32-worker trajectory. All three generation confirmations
  are now valid and complete; the smaller K2048/four-generation seed0/4
  configuration remains the four-core record at 3,562. Larger optimized search
  and signed-continuation full comparisons remain pending at this checkpoint.

- K4096/B4 now completes all2,000 steps under the strict four-core deadline,
  both with radix sorting alone and with the candidate-ranking cache. Both
  finish3,555 tasks, exactly matching every32-worker action, assignment and
  event. The previous kernel exceeded1s at the first step for this search size.
  This establishes full feasibility of the exact speedups; it does not yet beat
  the3,562 four-core task record. Full K5120 throughput remains pending.
  [Four-core equivalence and timings](random05/results/ranking-cache-split-full-v52/four-core-equivalence.json).

- The signed-score comparison completes all8 strict four-core runs. At one
  generation, coefficients0/-0.25/-0.5/-1 give3,501/3,403/3,384/3,471;
  at four generations they give3,520/3,543/3,521/3,505. Both zero controls
  preserve every previous action, assignment and task event. The small gain
  for -0.25 with four generations needs planner-seed checks before adoption;
  the four-core record remains3,562.
  [Full results](random05/results/optimistic-continuations-four-split-full-v53/summary.json),
  [zero-control equivalence](random05/results/optimistic-continuations-four-split-full-v53/control-equivalence.json).

- Build-v54 adds optional multiple elite parents between search generations,
  retaining distinct evaluated priority vectors and the existing incumbent.
  Fully randomized candidates do not consume an elite's mutation slot. Total
  rollout work remains fixed; one elite preserves the previous algorithm.
  Single-generation equivalence, dense task turnover, duplicate-parent handling
  and worker-count regressions pass. A bounded full four-core comparison tests
  2/4/8/16 parents and one interaction with the small optimism bonus. No gain
  is assumed before full results. This is a general search change.

- All three K5120/B8 full four-core runs pass the strict1s limit. One/two/four
  generations give3,537/3,591/**3,637**, with mean latencies792/793/791ms and
  maxima961/927/928ms. Four generations sets the new confirmed four-core record,
  **+24.8% versus NMS4**. RSS309MB. The existing V2 validation remains tied to
  its older3,501 candidate; a third protocol is declared before generating new
  inputs to test this larger search honestly.

- Four elite parents at K2048/B8/four generations finish **3,618 tasks** on
  four cores, versus3,520 for one parent (+2.8% at the same rollout count).
  Two/eight/sixteen parents give3,537/3,465/3,497. At eight generations,
  one/four/eight parents give3,482/3,428/3,535. Adding optimism0.25 to the
  four-parent/four-generation case gives3,581. All9 full runs are valid;
  the one-parent controls exactly preserve their complete earlier trajectories.
  This selected3,618 result needs seed checks and is below the3,637 record.
  [Evidence](random05/results/elite-parents-four-split-full-v54/summary.json).

- The optimism0.25 gain does not replicate: planner seeds0–4 give
  3,476/3,432/3,528/3,543/3,485, mean3,492.8 versus3,531.4 for zero coefficient
  (-1.1%, only2/5 pairs positive). All five optimism runs use four cores and
  finish validly. Leave the coefficient at zero.
  [Seed evidence](random05/results/optimism-seeds-four-split-full-v53/summary.json).

- Fresh validation V3 was frozen at
  [81bdfbd](https://github.com/fywu85/lorr/commit/81bdfbd) before generating
  seeds50005/50006. The candidate remains source6ce9312 / K5120/B8/four
  generations / seed3 / one parent / mean score, the3,637 four-core configuration.
  All six predeclared candidate/NMS4 runs are submitted. Additional development
  seeds and later elite-parent changes do not replace this frozen candidate.
  [Protocol](random05/FRESH_VALIDATION_V3.md).

- Eight elite parents at K8192/B8/four generations reach **3,689 tasks** on
  32 workers, versus 3,596 with one generation and 3,582 with four generations
  and one parent at the same K. This is a new selected maximum, not a replicated
  average gain. All 2,000 steps pass; mean238ms/max359ms/RSS590MB.
- The K2048/four-parent planner-seed check gives
  3,553/3,610/3,484/3,618/3,394 for seeds0–4, mean **3,531.8**. The single-parent
  control mean is3,531.4. Two of five pairs improve, so retain the option for its
  selected maxima without claiming a general mean improvement.
- Per user steering, throughput remains the primary objective. The progress audit
  now also records completed-order mean/p95/max, the oldest unfinished age and
  initial-cohort unfinished/unopened counts for all historical frontier rows.
  Completed-only latency statistics are explicitly censored by unfinished orders.

- K5120/B8/four-generation planner-seed validation is complete: seeds0–4 give
  3,611/3,526/3,648/3,637/3,608, mean3,606.0. All five improve against the
  K2048/four-generation controls, whose mean is3,531.4 (+2.1%). These are
  planner seeds on the archived development input, not independent input seeds.
  Each new full four-core run has max latency under928ms and no errors/timeouts.
- The new persistent-candidate implementation is source05559b7/build-v55.
  Dense regressions pass in5.99s. Full controls and 2/4/8-vector comparisons are
  running; no performance claim yet. Reused vectors are re-evaluated against
  current state, and default carry count1 preserves the previous algorithm.
- The larger K16384/eight-parent run gives3,673, below K8192/eight-parent3,689.
  More search work is not guaranteed to improve the selected trajectory.

- Third fresh-input validation is complete and audited: frozen source6ce9312,
  plannerseed3,3698 vsNMS2926/2900 and3619 vs2906/2831. Against each stronger
  NMS repeat, gains are26.38%/24.54%, aggregate25.46%. Allsixattempts pass full
  horizon, strict1s, source/input/binary checks, four physical cores and32GB.
  Protocol81bdfbd predates input generation. No newer candidate was substituted.
- Predictive matching loses on the current solver: K2048 gives3450 vs3520;
  K5120 gives3541 vs3637. The latter full trajectory exactly repeats between
  four cores and32workers. Leave the option off.
- Extra parents at four-core K4096/K5120 give3604/3565 (four parents) and
  K5120/eight parents3610. The selected four-core record remains3648 with one
  parent, although32-worker K8192 benefits on seed3. Allcasesvalid.

- Persistent candidates, source05559b7/build-v55: K2048/E1 with carry2/4/8
  gives3508/3540/3540 versus3520 control; E4/carry4 gives3562 versus3618.
  K5120/E1 carry2/4/8 gives3560/3596/3613 versus3637. K8192/E8 carry4/8 gives
  3671/**3705** versus3689. All full runs are valid. Retain the selected new
  32-worker maximum; persistence has no universal benefit and its default stays1.
- Default-carry controls reproduce every action, assignment, event and task of
  the original3637 and3689 runs. The elite-selection refactor preserves behavior.
- Build-v56/source0bd0a41 adds optional mutation decay between generations.
  Defaults preserve existing search; regressions pass6.80s. Full controls and
  decay0.25/0.5/0.75 comparisons are underway. No throughput gain claimed yet.

- Four-core latency headroom is concentrated after startup: all five K5120
  planner seeds peak at step0 (924–928ms), whereas maxima after step4 are839–855ms.
  Build-v57/source0295ae9 declares a separate first-step rollout count, leaving
  regular fixed work unchanged thereafter. No time-based cutoff or partial result
  is introduced. Full strict timing/throughput checks are pending.

- Mutation decay results: atK2048/E1, rates0.25/0.5/0.75 give3503/3557/3513
  versus3520. AtK5120/E1 they give3462/3526/3530 versus3637; atK8192/E8/carry8,
  3628/3627/3702 versus3705. All valid. The smaller-budget0.5 gain is not
  replicated and no configuration improves the main frontiers. Leave decay1.
  Both full default controls preserve all actions, assignments, events and tasks.
- Startup-budget testing: the equal-K5120 control is fully identical to3637.
  FirstK5120/regularK6144 exits124 at timestep2,1000.533ms; this failed strict
  attempt is preserved and excluded. FirstK5120 with regular5632/5888 is pending.
- Build-v58/sourcebdc051f adds configurable bounded ranking-cache capacity,
  powers of two8–1024/default64. Exact dense tests with frequent eviction and
  larger128/256 tables pass in7.35s. Full64/128/256/512 comparisons and larger-K
  timing checks are underway. Memory capacity changes must preserve trajectories.

- Larger-cache32-worker controls with128 and512 slots both reproduce3,637,
  with every action, assignment, event and task identical to the prior64-slot
  run. Four-core full timing checks and K6144 attempts remain in progress.
  [Exact controls](random05/results/ranking-capacity-control-split-full-v58/control-equivalence.json).


- The retained-parent configuration establishes a new four-core selected best
  of3,655, but its five planner seeds average3,589.8 versus3,606.0 for the
  one-parent policy. Preserve both the maximum and the negative mean result.
  Stronger root mutations0.5/1 also lose at both tested budgets.
  [Seed comparison](random05/results/persistent-parents-seeds-split-full-v55/summary.json),
  [mutation comparison](random05/results/stronger-root-mutation-split-full-v55/summary.json).
- Increasing ranking-cache capacity64->512 preserves the entire3,637 trajectory
  while reducing four-core mean latency791->772ms. The larger caches also make
  firstK5120/regularK6144 finish below1s, but its3,545 tasks do not beat the
  frontier. Keep the earlier cache64 timeout in the evidence.
  [Timing](random05/results/ranking-capacity-four-split-full-v58/summary.json),
  [full trajectory equivalence](random05/results/ranking-capacity-four-split-full-v58/6144-equivalence.json).
- Replaying the latest records separates loaded waits with active goals from
  waits after declared horizon triage: about13% of our loaded waits have a
  suppressed goal. This is descriptive accounting, not recoverable throughput.
  [Current action and suppression report](random05/results/action-audit-current-v55/REPORT.md).


- Staged search, source5f81613: the fixed1,024-root comparison gives3,718 at
  B12/K4608,3,670 at B14/K5120,3,695 at B16/K5632 and3,664 with a four-branch
  screen at B16/K7168. All full32-worker runs are valid. Direct four-core B12
  andB16 confirmations reproduce every trajectory field, with means628/755ms
  and maxima756/898ms. The3,718 result becomes the four-core frontier.
- FirstK4608 thenK5760/B12 gives3,770 on32workers;K6120 gives3,660.
  FirstK8000 thenK24000/B14 finishes3,702, below3,794 atK16320. More work
  does not guarantee more throughput. Worker-pinning controls and five-seed
  checks of the leading configurations are running; results are not assumed.
- Sampled rollout profiling attributes22.6% of worker time to setup in the
  former3,709 four-core record. The next source combines three independent
  per-robot setup passes and removes redundant pointer/flag clears, while
  preserving neighbor-dependent work after the full pass. Build-v66 regression
  and complete-trajectory controls must pass before any speedup claim.


- Full worker-placement controls reproduce3,770 on four cores and3,794 on
  32 workers, including every action, assignment, event and task. Every observed
  worker CPU mask remains inside its case's allocated cores. Four-core means
  are788/788ms and maxima845/843ms for binding false/close. The32-worker means
  are503/500ms, maxima585/874ms. No useful pinning speedup is established.
- The firstK4608/K5760/B12 configuration averages3,697.6 across planner seeds
  0–4:3,663/3,669/3,732/3,770/3,654. All five improve versus the earlier
  ordinaryK5120/B8/E1/P1 configuration (mean3,606, +2.54%), but this compares
  complete configurations and search budgets, not an isolated screening effect.
- The higher-budget firstK8000/K16320/B14 configuration scores3,785/3,721/
  failed/3,794/**3,852** for seeds0–4. Seed2 exits124 at timestep1850 with
  elapsed1422.350ms; no five-seed strict mean is claimed. The new3,852 seed4
  best is fully valid (mean478ms/max538ms). A declared strict repeat and a
  relaxed5s diagnostic preserve the original failure and use identical fixed
  work. The relaxed result cannot enter the strict frontier.
- Screening alternatives at approximately K5760/B12: two branches/keep-half
  gives3,675; two/keep-eighth3,509; one/keep-quarter3,639; one/keep-eighth3,737.
  All four full runs are valid and below3,770. Exact K and startup work vary
  slightly to fit whole generation groups; manifests retain those counts.
- V4 froze source5f81613 and the3,770 four-core candidate in protocol
  [a7bad0c](https://github.com/fywu85/lorr/commit/a7bad0c) before generating
  inputs50007/50008. All six candidate/NMS runs have verified four-core EPYC9354
  allocations and are underway. Later records do not replace that candidate.
- Setup fusion, sourcefde9172/build-v66, passes all regressions in13.61s.
  Paired full controls remain underway. Early matched-step profiling is slower
  in the setup phase; do not claim an optimization or use it in frozenV4.


- Reject setup fusion after its full32-worker paired control. Both source65 and
  sourcefde9172 reproduce every field of the3,794 trajectory, but fusion increases
  mean latency505.76->518.56ms (+2.53%); maxima576.62/576.89ms. The working
  engine is restored byte-for-byte to the already validated source5f81613.
  The full four-core comparison continues as separate evidence; no four-core
  timing conclusion is inferred from partial output.
  [Exact trajectory and timing audit](random05/results/setup-fusion-32-full-v66/equivalence-and-timing.json).


- Independent full replay of the current3,770/3,852 records and matched NMS
  confirms collision safety, unique assignments, every waypoint event, and
  locked-task handling. Our loaded rotations are421,149/416,358 versus
  NMS561,393/543,475. Total work on final-unfinished orders is12.7%/13.6%
  of robot-steps versus31.0%/30.0%, while our loaded waits are higher.
  Shared completed orders require6.62%/1.15% less loaded work. These are
  observational accounting results; task sets and congestion histories differ.
  Full paired-setting ablations of known-horizon triage are now running.
  [Replay report](random05/results/action-audit-frontiers-v65/REPORT.md).


- The complete four-core setup-fusion control corroborates the rejection:
  both full traces exactly reproduce3,770, but mean time rises785.94->798.83ms
  (+1.64%). Maxima854.99/853.75ms. Source was already restored to5f81613;
  all experimental evidence remains archived.
  [Four-core exactness and timing](random05/results/setup-fusion-four-full-v66/equivalence-and-timing.json).


- High-budget seed2 completes both declared timeout follow-ups with **3,705**
  tasks. Their full trajectories are identical under1s and5s entry limits;
  means506.42/507.34ms, maxima564.12/561.46ms. The original step1850 spike
  (1422.35ms, exit124) did not recur. This is consistent with transient runtime
  variation; its cause is not proved and the original failure remains recorded.
  [Complete trajectory/timing audit](random05/results/staged-seed2-timeout-check-full-v65/equivalence-and-timeout.json).
- Completed deterministic outcomes at firstK8000/K16320/B14 now average3,771.4
  (3,785/3,721/3,705/3,794/3,852), versus3,690.6 atK8192/B10: +2.19%, all five
  planner-seed pairs positive. This uses the declared seed2 strict repeat;
  the initial five attempts contained one timeout, so this is not a claim of
  five-for-five first-attempt deadline reliability or fresh-input validation.
  [All outcomes and failure caveat](random05/results/staged-record-seeds-32-split-full-v65/k16320-completed-outcomes-with-repeat.json).
- K7056/B12/first4608 scores3,788 on32 workers, but both direct four-core
  attempts fail at timestep1: cycle-mask0=1073.008ms, mask1=1040.146ms.
  Neither receives a four-core throughput score or promotion. K6480/K6768
  give3,733/3,740 on32 workers, below the existing four-core candidate's3,770.


- Final larger-budget check K28800/B14/first8000/seed4 completes3,706,
  below3,852 atK16320 (mean809ms/max916ms). All four headroom experiments
  are terminal and archived; none changes the strict frontiers.
- The final no-cutoff four-core ablation completes3,503 tasks, mean806ms,
  maximum866ms. Its32-worker counterpart scores3,632, mean482ms/max531ms.
  Both are new no-cutoff frontiers, with guidance still explicitly marked as
  a trick. The horizon switch contributes7.6%/6.1% on these selected seeds.
- All submitted Random05 jobs are terminal. The final audit verifies74
  timestamped frontier rows, all six original freshV4 runs, six independent
  fresh trajectory replays, and29 compiled engine/simulator source/header files.
  Fresh25.42% versus the stronger NMS repeats, alongside the selected four-core
  29.4% gain, establishes the requested reasonably-close performance level.
  It does not reproduce the colleague's private experiment exactly.
  [Completion evidence](random05/results/completion-audit.json),
  [concise results](random05/RESULTS.md).


- New 4,000-task campaign begins from verified 3,770/four-core and 3,852/32-worker
  records. First investigate candidate-score quality using saved pre-decision
  states and fully evaluated alternative roots. Offline continuations will
  measure completions with repeated replanning; they are diagnostics, not
  benchmark throughput or an optimality bound. New algorithmic candidates
  still require complete 2,000-step runs and independent replay.

- Saved-state build9c7a78b passes regression tests in15.05s. Three completed
  counterfactual states (250/500/750), eight decisions each, three shared
  downstream seeds, do not establish systematic scoring failure. The selected
  decision has the best mean at250; alternatives can do better at500/750.
  A full-budget continuation control matches original cumulative completions,
  waypoint visits, forwards, turns and waits at all128 subsequent steps.
- New optional motion-component recombination passes25.69s regressions. Unlike
  fixed geometric regions, groups include every robot linked by either plan's
  moves and every shared destination, including holes. Each proposed hybrid
  completes all continuations and is collision-certified. Full32-worker
  control and three variants are declared in components-full-v68.json; no
  throughput gain is assumed.
- Fable was resumed in the same session with a6,664-byte research summary
  and no new source payload. Provider returned out-of-usage-credits again,
  zero usage/cost, so no review or recommendations were received.

- The4k campaign's first selected increment is **3,857** on32workers, only
  five tasks above3,852. The sole setting change is horizon-triage scale1.5->1.25;
  this remains a declared trick. The1.75 alternative gives3,798. Both full runs
  pass strict1s; no robust across-seed gain is claimed from this small record.
- Snapshot capture reproduces every action, assignment, event and task of the
  complete3,852 control. All seven saved states are now available for offline
  decision comparisons; new motion-component benchmarks are queued/running.

- The seven-state decision audit confirms exact snapshot state and whole-run
  trajectory equality.128-step score/completion correlations are weak and mostly
  negative later, but three-seed leave-one-out gains are mixed. This supports
  further diagnosis, not a proven scoring defect. All 168 counterfactuals finish
  and reproduce their saved first decision.
  [Audit and limitations](random05/results/decision-probes-v67/REPORT.md).
- 3,857 passes independent full action/task replay. All75timestamped frontiers
  audit against hashes and strict full runs. Its completed max1937steps,
  initial unfinished135/unopened102; final maximum remains censored at>=2000.
- Additional declared controls: triage1.25 on planner seeds0/3, and completion
  bonuses2/4/8 with the current14-future average. The latter failed before
  averaging; this recheck tests the specific change in reward noise, with
  complete settings frozen and no gain assumed.

- The component-disabled source68 full control reproduces3,852 exactly in all
  six trajectory fields. The three component variants remain in progress.
- New **trick** experiment: directional deadline estimates blend hop distance
  with oriented chain cost, normalized to preserve the pool's total estimated
  distance. This changes which tasks appear feasible without simply moving the
  global cutoff. Source69 regression checks pass16.81s; five full cases include
  an unchanged control and mixes.5/1 atscales1.25/1.5. No gain assumed.

- Reject all three general motion-component presets:3,702/3,721/3,740 tasks
  versus the exact3,852 control. Every full run is valid, so this is a genuine
  throughput regression, not a timeout or collision issue. Extra local score
  improvements do not establish throughput gains. Keep the feature disabled.
  [All outcomes](random05/results/components-split-full-v68/outcomes.json).
- Expanded four still-queued directional-estimate jobs to23previously verified
  EPYC9354hosts. Only their hard queue lists changed; inputs, binaries, seeds,
  sixteen physical cores,32workers and strict limits remain frozen. The running
  control keeps its original allocation. Runtime CPU-model preflight still applies.
  [Recorded queue changes](random05/results/reference-host-expansion-20260920/queue-change.json).

- Cutoff scale1.25 did not improve the two replication seeds: seed0=3,780
  (versus3,785 at1.5), seed3=3,785 (versus3,794). The seed4 maximum3,857
  remains a selected result, with no replicated gain established.
  [All outcomes](random05/results/record-triage-seeds-split-full-v65/outcomes.json).
- Completion bonuses2/4/8 with14-future averaged scoring give3,746/3,811/3,781
  versus3,852 atbonus0. All strict full runs are valid; reject these presets.
  [All outcomes](random05/results/averaged-completion-split-full-v65/outcomes.json).

- Directional-triage mix0.5 atscale1.25 reaches **3,872**,15tasks above3,857.
  Mix1 at1.25=3,841; mixes.5/1 at1.5=3,832/3,843. All five full runs valid,
  with the disabled control reproducing all six trajectory fields of3,857.
  Independent replay of3,872 passed; source233f5bf, finish22:49:18UTC, mean500ms,
  max535ms, RSS579MB. This remains a selected seed4 gain until replication.
  [Outcomes and control](random05/results/directed-triage-split-full-v69/outcomes.json),
  [replay](random05/results/directed-triage-split-full-v69/replay-3872.json).

- Source70 adds optional repeated replanning inside short forecasts, using only
  the current visible task pool. Regression suite passes27.78s; a one-root check
  preserves every dense decision and persistent state. Six strict full cases
  compare the3,872 control and fixed-budget variants, starting atstep50 to avoid
  repeating startup matching costs. No throughput gain is assumed.

- Replanning original attempts retain two distinct failures:8roots*2futures
  atK32 never launched because GRID failed to bind16cores (64observed);
 4roots*2futures atK64 exited124 atstep62 (1,051ms). Neither is a valid
  throughput score. Resubmit the allocation-only failure with the same solver
  settings on the other verified hosts; leave the original evidence intact.
- **Trick: bounded guidance retune under current averaged search.** Eight
  predeclared single-factor cases remove the selected flip, extend its seeded
  prefix to2/4flips, test contrast2/2.8/3.2 or turn0.4/0.8. All other settings
  stay at3,872, seed4. Prior lower-budget guidance searches mostly lost; this
  tests interaction with the newer search, not a presumed improvement. Every
  run uses --trick RANDOM-05 and the archived development input only.

- Phase diagnosis: in the first250steps ours reaches2,196waypoints and finishes
 413orders; NMS reaches2,181waypoints and finishes521orders. Initial chain
  selections are similar. This is a difference in progress concentration, not
  evidence of108recoverable tasks. The3,857/3,872 actions first diverge at1544.
  [Audited counts](random05/results/4k-gap-audit-v69/REPORT.md).
- Next generic hypothesis: retain a fraction of priority age across intermediate
  waypoints, while still resetting at whole-task completion. Default0 preserves
  the current solver. This tests chain continuity without replacing PIBT aging
  with a shortest-task ordering; it may also hurt by favoring long tasks.

- The8root/1future/K32 forecast also exceeds1s atstep459 (1,020ms).
  Preserve that strict failure. Two separately declared5s diagnostic runs
  finish this setting and4root/2future/K64 to assess algorithmic throughput
  despite the strict failures; they cannot enter the strict frontier. This
  follows the user's allowance for relaxed timing during development while
  retaining the one-second reference and fixed-work/no-partial-output rule.

- Directional cutoff mix.5 now has three paired planner seeds atscale1.25:
  seed0 3,778vs3,780; seed3 3,845vs3,785; seed4 3,872vs3,857. Two positive
  pairs, aggregate+0.64%; this is a small development sample, not fresh-input
  validation. Mix.75seed4=3,858. Mix.25 failed atstep1575 (1,985ms); a separately
  declared5s diagnostic is queued and cannot enter the strict frontier.
  [Paired outcomes](random05/results/directed-triage-followup-split-full-v69/paired-outcomes.json).
- Source70 with replanning disabled reproduces all six trajectory fields of
 3,872 exactly. The one-root control and remaining forecast comparisons continue.

- All eight current-search guidance retunes lose: noflip3,746; two/four flips
 3,827/3,707; contrast2/2.8/3.2 gives3,714/3,714/3,780; turn.4/.8 gives
 3,740/3,663. Every run is valid. Keep the3,872 field unchanged.
- New **trick** hypothesis from the NMS/phase audit: weight score progress by
  rank of remaining whole-chain cost, while preserving PIBT priority aging.
  Equal costs share a rank and weights have mean1 to hold the cost scale.
  It requires --trick RANDOM-05 and may increase long-order delays. The
  colleague's failed urgency-weighting tests are a caution; full runs decide.

- Replanning full strict results: disabled andone-root controls both reproduce
 3,872 in all six trajectory fields.4roots/2futures/K32 scores3,752;
 8roots/2futures/K32 allocation repeat scores3,790. Both valid but worse.
  The8roots/1future/K32 and4roots/2futures/K64 relaxed diagnostics continue;
  their original strict failures remain recorded. Leave reranking disabled.
- Source72 rank-weighted score regression passes27.92s. The real entry rejects
  the new preference without --trick RANDOM-05 (exit125), as required. Five
  strict full cases test powers0/.25/.5/1/2 with3,872 settings; power0 is the
  unchanged-behavior control. Age retention and reranking are off in these cases.

- Source73 regression passes28.72s. It splits the same worker allowance between
  whole forecasts and their internal searches, and optionally retains live
  generations/elites/history within each smaller forecast. Nested worker tests
  preserve decisions. The full batch includes the earlier3,752-rule control
  with a new worker split, two strictK128 candidates, and two explicitly
  relaxed5s diagnostics (more futures orK256). No gain is assumed.
- The rank-score power.25 original never launched because GRID exposed64cores
  instead of binding16. Repeat the same solver settings with a valid allocation;
  keep the original preflight failure. Other power cases continue.

- Priority retention is rejected:0/.25/.5/.75/1 gives3,872/3,790/3,711/3,785/
 3,749. All full runs valid; zero reproduces all six3,872 trajectory fields.
  [Outcomes](random05/results/waypoint-age-split-full-v71/outcomes.json).
- The relaxed quarter-mix cutoff run completes3,856, below3,872. It stays a
  diagnostic even though it completed; the original strict failure is preserved.
- More faithful K128 forecasts exceed1s on their first enabled step50:
  R4F2=1,135ms andR8F1=1,078ms. Heavier5s diagnostics are still running.
  Two declared strict follow-ups reduce innerK to64 at8steps or useK96 at
 6steps, retaining the live generation/elite/history structure. No time-based
  partial answer or automatic budget reduction is used.

- Both relaxed first-generation forecast diagnostics lose:8roots/1future/K32
  gives3,682;4roots/2futures/K64 gives3,723. Original strict failures remain.
  [Diagnostic outcomes](random05/results/replanning-relaxed-diagnostic-split-full-v70/outcomes.json).

- NMS-inspired rank weights over the whole run are rejected on this preset:
  powers0/.25/.5/1/2 give3,872/3,867/3,845/3,670/3,716. Power.25 uses its
  separately declared allocation repeat; the original preflight failure stays
  recorded. Zero reproduces all six baseline trajectory fields.
  [Full outcomes](random05/results/rank-progress-split-full-v72/outcomes.json).
- The weighting phase audit motivates a startup-only **trick** test: power.5
  gives471 versus413 completions in steps1–250, but loses27 over the full run.
  These different trajectories do not establish a gain from switching policies.
  An optional declared startup window returns to equal progress scoring at
  its boundary; it remains off by default and requires --trick RANDOM-05.
- Six source69 full cases test additional continuation averaging (B18/22/26),
  each at approximately fixed total work and at fixed3,264candidate roots.
  They retain the3,872 settings otherwise. Earlier larger-K-only runs lost,
  so this changes how the work is spent; no gain is assumed.

- Source74 startup-window regressions pass19.23s, including equivalence to
  unlimited weighting before the boundary and equal scoring at/after it from
  identical saved states. Dense assignment locks, checkpoint replay and worker
  determinism pass. Seven full strict cases are declared: two unchanged-policy
  controls and startup powers.25/.5/1, with.5windows100/250/500steps.

- Source75 tests independent future samples for finalist selection. The ordinary
  search remains unchanged; completed finalists get a separate common set of
  priority continuations. It completes every declared branch before choosing,
  and never reads future tasks. One-finalist and original-score controls retain
  every decision and persistent state. Dense collision/assignment checks, exact
  work accounting, worker determinism and replay regressions pass20.30s.
  Seven full strict cases compare off/one-root controls and4/16/32finalists
  with32/64/128branches, including a half-original-score blend. No gain yet.

- New selected strict record: **3,877**, source233f5bf/build-v69, B18 atK16320
  andfirst7968, seed4. Mean484.147ms/max526.147ms/RSS545736KiB. Independent
  action/task replay passed. This is only+5 versus3,872; no mean gain is claimed.
  Max completed-order latency1966steps;131initial orders unfinished,94unopened.
  B22/B26 at similar fixed work lose (3,732/3,705); fixed-root counterparts
  still run. [Replay](random05/results/branch-allocation-split-full-v69/replay-3877.json).

- The nested-worker legacy-forecast control exactly reproduces3,752 in all
  six trajectory fields. The more faithful strict K64/depth8 forecast completes
 3,785, below3,872; K96/depth6 fails the deadline. The two heavier5s diagnostic
  cases are still running. Optional forecast reranking remains disabled.
- Ten predeclared source69 runs compare B14/B18 on planner seeds5–8, plus B18
  on previously measured seeds0/3. These are RNG-seed comparisons on the same
  archived development input; no new task/start inputs or held-out data are used.
  All77frontier rows and updated waiting metrics passed the source/full-run audit.

- **Startup trick reaches3,917** at power.25 for250steps, source1e266b0,
  completed2026-09-21T00:05:35.106669+00:00. Mean487.500ms/max531.571ms,
  RSS565272KiB. Independent full replay passed; controls exactly reproduce
 3,872 (power0) and3,845 (unlimited.5) in all six trajectory fields.
  Power.5for100/250/500 gives3,879/3,756/3,801; power1for250 gives3,755.
  The3,879 run completed after3,917, so it is not a separate frontier.
  Maxcompletedwait1953steps; initial unfinished126/unopened97.
  [Replay](random05/results/startup-rank-split-full-v74/replay-3917.json).
- Ten follow-up full runs test milder/stronger startup preference, duration,
  coupling with B18, and planner seeds0/3/5/6. This is development selection
  and paired-seed checking; no fresh task/start input has been generated.

- All six extra-averaging runs finish strict valid. At roughly fixedwork,
  B18/B22/B26 give3,877/3,732/3,705; at fixed3,264roots they give
 3,837/3,871/3,772. Only the small B18fixedwork gain survives this batch;
  its paired-seed test is running.
- Independent rescoring partial results: off/one-finalist controls both3,872,
  exact allsixfields. R4B32=3,885; R16B64=3,906; half-original blend=3,819.
  Pure R16B64 improves34 over its exact3,872control, but stays below the
 3,917startup record. Two128-branch variants still run.
- The fresh-input auditor now reads a frozen allocation declaration, preserving
  the four-core default, and can check a prior-solver baseline alongside NMS.
  Allocation rejection tests pass; the original frozenV4 result remains exactly
  +25.42%. It additionally verifies simulator source/headers and protocol-commit
  order. No new validation inputs have been generated or candidate frozen yet.

- B18 plannerseed5 gives **3,928**, source233f5bf, finished00:19:49.212073UTC.
  Mean499.181ms/max554.944ms/RSS546252KiB. Independent replay passed.
  Maxcompletedwait1952steps; initialunfinished130/unopened88. The seven-seed
  B18total26,705 is0.27% below B14's26,778, despite four positive pairs and
  the higher selected maximum. Both facts are retained.
  [Paired evidence](random05/results/branch-seeds-split-full-v69/paired-outcomes.json).
- Predeclare additional plannerseeds9–24 at the unchanged B18configuration,
  retaining all16outcomes. The user permits selected seeds to meet the target;
  this search is not claimed as a new algorithmic or mean improvement. Also
  test the selectedseed5control with startup preference and/or independent
  rescoring in five full runs. No new task/start inputs are used.

- Startup follow-up results are complete except the separately named allocation
  repeat: power.125/.375for250 gives3,800/3,926; power.25for375/500 gives
 3,724/3,881; B18coupling onseed4 gives3,754. The original125step case was
  refused before launch on research42 and is preserved beside its repeat.
  The.25/250 paired check on seeds0/3/4/5/6 totals19,155 versus19,182
  (-0.14%, two positive pairs). Its selected3,917 is not a mean improvement.
- The last heavy closed-loop diagnostic finishes3,682 (R4F2K256,5s cap).
  R4F4K128 gives3,723. Both lose, like the smaller forecasts; keep this
  feature disabled. All strict failures remain in the archive.
- Phase counts do not imply additive gains: startup3,917 is behind baseline
  by29 atstep500, then recovers later. Independent-rescore3,906 also gains
  most in later blocks. The combination experiments test their interaction.
  [Phase audit](random05/results/startup-rescore-phase-v75/audit.json).


### 2026-09-21 00:59 UTC: replication and bounded forecast-scoring follow-up

The verified frontier remains **3,928**, with 72 tasks left to reach 4,000.
Source75 controls reproduce both 3,928 and 3,917 exactly in all six trajectory
fields. The selected B18 configuration's sixteen additional planner seeds9–24
produce fifteen valid runs (best3,904) and one strict deadline failure atstep1455;
the failed attempt is retained. Eight new guidance patterns top out at3,899.
Neither sweep changes the record.

Independent R16B64 rescoring on the B14 configuration improves all three checked
planner seeds0/3/4: 3,805/3,893/3,906 versus3,778/3,845/3,872. Aggregate gain is
**0.948%** on this development input. Startup weighting and rescoring together
lose; there is no evidence their individual gains add. These comparisons are
not fresh-input validation. [Paired evidence](random05/results/startup-rescore-coupling-split-full-v75/paired-rescore-seeds.json).

Build-v76 passes regression tests and separates the unchanged-priority forecast's
weight from the number of random futures. Eight full strict tests are declared
in [the preset](random05/experiments/rescore-static-mixture-full-v76.json).
The legacy setting must reproduce3,906. No new fresh task/start inputs have been
generated, and all previous validation inputs remain excluded from tuning.


### 2026-09-21 02:16 UTC: 3,978 selected record and broader RANDOM campaign

- Bias3, planner seed0 gives3,978; seeds3/4 give3,839/3,782. Together withseed5's
  3,941, the paired aggregate is15,540 versus15,146 for bias0 (+2.60%,3/4positive).
  Independent replay passes for the new maximum. Completed maximum wait1,959;
  initial unfinished130/unopened92. Same development input, not fresh validation.
- Amplitudes2.5/3.5 atseed5 give3,890/3,835. Bias3 with K24480 gives3,945;
  it completed after3,978 and is therefore not a frontier record.
- Source79 introduces a configurable proposal fraction. Quarter control3,941
  reproduces all six trajectory fields exactly. Bias3 fractions1/8,1/2,1 give
  3,904/3,909/3,900; bias2 fractions1/2,1 give3,898/3,897. All six strict;
  retain the quarter default. New bias3 planner seeds6/7/8/24 remain pending.
- User expanded active development to all five RANDOM cases. See the separate
  [all-density ledger](random05/RANDOM_PROGRESS.md). Large maps are deferred.
