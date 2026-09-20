# RANDOM-05 combined-track throughput progress

Goal: approach the colleague's reported roughly 27–28% gain over matched NMS
combined-track runs, then improve further. Their 3,531 and 3,715 task counts use
private synthetic instances; those absolute counts are reference targets, not
direct baselines for the archived competition instance.

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
- Fixed-work screens accelerate development. Full runs determine the frontier.
- Record every new best, including a seed-specific or trick-enabled result,
  alongside its source commit/hash and UTC timestamp. Distinguish that best from
  replicated improvements across seeds.

## Verified local frontier

Best verified combined result: **3,299 tasks / 2,000 steps** on 32 workers
(16 physical cores with SMT), versus **3,172** for NMS with the same allocation:
**+4.0%**. Planner seed0, generated field seed15, K1024, noise200, dispersion0.8,
local5/equal, wait0.5, exact matching with oriented guidance and keep bonus0.5,
and known-horizon triage scale1.5 (`--trick RANDOM-05`). Mean latency131 ms,
maximum224 ms, peak RAM419 MiB. This is a single-seed best.

The four-core best is **3,127** versus NMS **2,903** (**+7.7%**). It additionally
prepares cycles among robots PIBT already leaves waiting. Its six-seed mean is
3,021, slightly below3,034 without that change: the maximum improved, the mean
did not. Best without known-horizon triage is **2,914** on four cores. The
colleague's roughly 27–28% matched advantage remains the campaign objective.
All frontier/reference runs have zero planner/scheduler errors and timeouts.

[NMS four-worker evidence](random05/results/nms4-full-v1/summary.json),
[NMS 32-worker evidence](random05/results/nms-original-full-v1/summary.json).
The four-worker build changes only the existing local reference's worker constant;
both references retain the earlier constructor-initialization safety fix documented
in the NMS snapshot. Neither benchmark removes NMS's combined-track features.

| Completed UTC | Source commit | Configuration / seed | Tasks / 2,000 | NMS reference | Gain | Evidence |
|---|---|---|---:|---:|---:|---|
| 2026-09-20T07:55:37.615086+00:00 | [215fd6d](https://github.com/fywu85/lorr/commit/215fd6d167c96695e9ac6db6f8517791f2dab077) | Generated flow seed 1; turn cost 0.6; K=16; planner seed 0; `--trick RANDOM-05` | 54 | 2903 (4 workers) | -98.1% | [Full evidence](random05/results/flow-first-v4/summary.json) |
| 2026-09-20T07:58:41.020438+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | lanes-intent; K=16; seed 0; `--trick RANDOM-05` | 2113 | 2903 (4 workers) | -27.2% | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T07:58:41.152623+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | flow-intent; K=16; seed 0; `--trick RANDOM-05` | 2230 | 2903 (4 workers) | -23.2% | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T07:58:41.330442+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | flow06-disp08; K=16; seed 0; `--trick RANDOM-05` | 2346 | 2903 (4 workers) | -19.2% | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T07:58:42.118225+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | upstream-intent; K=16; seed 0; `--trick RANDOM-05` | 2431 | 2903 (4 workers) | -16.3% | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T08:02:13.099706+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | file-k64-disp0-sched0; seed 0; `--trick RANDOM-05` | 2488 | 2903 (4 workers) | -14.3% | [Full evidence](random05/results/scale-full-v5/summary.json) |
| 2026-09-20T08:05:44.745252+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | file-k256-disp0.8-sched1; seed 0; `--trick RANDOM-05` | 2562 | 2903 (4 workers) | -11.7% | [Full evidence](random05/results/scale-full-v5/summary.json) |
| 2026-09-20T08:05:50.770563+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | file-k256-disp0.8-sched0; seed 0; `--trick RANDOM-05` | 2594 | 2903 (4 workers) | -10.6% | [Full evidence](random05/results/scale-full-v5/summary.json) |
| 2026-09-20T08:10:24.936747+00:00 | [8993a43](https://github.com/fywu85/lorr/commit/8993a43) | K=64; local5; equal; dispersion 0.8; horizon 2000; seed 0; `--trick RANDOM-05` | 2658 | 2903 (4 workers) | -8.4% | [Full evidence](random05/results/refinements-full-v6/summary.json) |
| 2026-09-20T08:18:53.857208+00:00 | [ca80563](https://github.com/fywu85/lorr/commit/ca80563) | predict-match; K=64; local5/equal/horizon2000; seed 0; `--trick RANDOM-05` | 2669 | 2903 (4 workers) | -8.1% | [Full evidence](random05/results/motion-guidance-full-v8/summary.json) |
| 2026-09-20T08:19:08.147723+00:00 | [ca80563](https://github.com/fywu85/lorr/commit/ca80563) | loop4; K=64; local5/equal/horizon2000; seed 0; `--trick RANDOM-05` | 2677 | 2903 (4 workers) | -7.8% | [Full evidence](random05/results/motion-guidance-full-v8/summary.json) |
| 2026-09-20T08:32:56.449253+00:00 | [6fb222e](https://github.com/fywu85/lorr/commit/6fb222e) | Wait0.5; cached K=64; local5/equal/horizon2000; seed 0; `--trick RANDOM-05` | 2729 | 2903 (4 workers) | -6.0% | [Full evidence](random05/results/wait-cost-full-v9/summary.json) |
| 2026-09-20T08:41:54.502028+00:00 | [f7ca98c](https://github.com/fywu85/lorr/commit/f7ca98c) | exact160-guided; wait0.5; K64/local5/equal/horizon2000; seed0; `--trick RANDOM-05` | 2766 | 2903 (4 workers) | -4.7% | [Full evidence](random05/results/matching-wait-full-v10/summary.json) |
| 2026-09-20T08:41:56.906339+00:00 | [f7ca98c](https://github.com/fywu85/lorr/commit/f7ca98c) | triage09; wait0.5; K64/local5/equal/horizon2000; seed0; `--trick RANDOM-05` | 2767 | 2903 (4 workers) | -4.7% | [Full evidence](random05/results/matching-wait-full-v10/summary.json) |
| 2026-09-20T08:48:21.604864+00:00 | [8619b95](https://github.com/fywu85/lorr/commit/8619b95) | exact800-guide; K64/wait0.5/local5/equal/horizon2000 scale0.9; seed0; `--trick RANDOM-05` | 2841 | 2903 (4 workers) | -2.1% | [Full evidence](random05/results/task-cost-full-v11/summary.json) |
| 2026-09-20T08:55:52.615079+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | avg-p3-cb0.75-s5-exact-guide; K64/triage0.9; planner seed0; `--trick RANDOM-05` | 2842 | 2903 (4 workers) | -2.1% | [Full evidence](random05/results/traffic-field-validation-full-v12/summary.json) |
| 2026-09-20T08:55:55.708615+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | avg-p3-cb0.75-s5-greedy; K64/triage0.9; planner seed0; `--trick RANDOM-05` | 2868 | 2903 (4 workers) | -1.2% | [Full evidence](random05/results/traffic-field-validation-full-v12/summary.json) |
| 2026-09-20T08:59:55.016021+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | Public field; exact/oriented matching; triage1.5; K64; seed0; `--trick RANDOM-05` | 2872 | 2903 (4 workers) | -1.1% | [Full evidence](random05/results/triage-scale-full-v12-bound/summary.json) |
| 2026-09-20T09:01:43.509210+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | flow-seed15; generated field; K64; planner seed0; `--trick RANDOM-05` | 2910 | 2903 (4 workers) | +0.2% | [Full evidence](random05/results/traffic-seeds-full-v12/summary.json) |
| 2026-09-20T09:05:15.036957+00:00 | [b79a218](https://github.com/fywu85/lorr/commit/b79a218) | noise200; generated field; K64; planner seed0; `--trick RANDOM-05` | 2916 | 2903 (4 workers) | +0.4% | [Full evidence](random05/results/priority-search-full-v13/summary.json) |
| 2026-09-20T09:14:27.889620+00:00 | [134faa8](https://github.com/fywu85/lorr/commit/134faa8) | field15-triage15; K64/noise200; planner seed0; `--trick RANDOM-05` | 2997 | 2903 (4 workers) | +3.2% | [Full evidence](random05/results/pockets-combinations-full-v15/summary.json) |
| 2026-09-20T09:19:02.607264+00:00 | [6aed8ba](https://github.com/fywu85/lorr/commit/6aed8ba) | blocked-extent2; K64; planner seed0; `--trick RANDOM-05` | 3127 | 2903 (4 workers) | +7.7% | [Full evidence](random05/results/cycle-portfolio-full-v16/summary.json) |
| 2026-09-20T09:21:46.636885+00:00 | [134faa8](https://github.com/fywu85/lorr/commit/134faa8) | K1024, 32 workers, field15/noise200/triage1.5; seed0; `--trick RANDOM-05` | 3231 | 3172 (32 workers) | +1.9% | [Full evidence](random05/results/frontier-compute-full-v15/summary.json) |
| 2026-09-20T09:40:30.891971+00:00 | [6aed8ba](https://github.com/fywu85/lorr/commit/6aed8ba) | Exact/oriented matching, keep0.5; K1024/32 workers; field15/noise200/triage1.5; seed0; `--trick RANDOM-05` | 3299 | 3172 (32 workers) | +4.0% | [Full evidence](random05/results/compound-search-full-v16/summary.json) |

## Reference evidence supplied by the user

| Colleague instance seed | Their combined solver | NMS better local build | Gain |
|---|---:|---:|---:|
| 41001 | 3,715 | 2,932 | 26.7% |
| 41002 | 3,531 | 2,762 | 27.8% |

Both are reported 4-core / one-second runs with a known 2,000-step horizon.
The published NMS score of 3,050 used different instances and hardware.

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
