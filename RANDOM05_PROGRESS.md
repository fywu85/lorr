# RANDOM-05 combined-track throughput progress

Goal: approach the colleague's reported roughly 27–28% gain over matched NMS
combined-track runs, then improve further. Their 3,531 and 3,715 task counts use
private synthetic instances; those absolute counts are reference targets, not
direct baselines for the archived competition instance.

## Comparison rules

- RANDOM-05: 800 robots, 2,000 simulation steps, combined planner and scheduler.
- Compare both solvers on the same input hashes, horizon and CPU allocation.
- Record valid completion, planner/scheduler errors, entry timeouts, latency,
  CPU use and peak process RAM. Our implementation must fit within 32 GB.
- Map-specific guidance and known-horizon triage require `--trick RANDOM-05`.
  Reassignment of unopened tasks is a legitimate combined-track scheduling rule.
- Fixed-work screens accelerate development. Full runs determine the frontier.
- Record every new best, including a seed-specific or trick-enabled result,
  alongside its source commit/hash and UTC timestamp. Distinguish that best from
  replicated improvements across seeds.

## Verified local frontier

Best verified combined result: **2,594 tasks / 2,000 steps**, planner seed 0,
public guidance and dispersion 0.8, K=256, no horizon triage. The matching
scheduler uses ordinary grid-distance approach costs. Every tested full v5 run
is valid. Mean entry latency is 151 ms and max is 247 ms for this best run,
using four physical cores. The full NMS reference is still running.

| Completed UTC | Source commit | Configuration / seed | Tasks / 2,000 | Matched NMS | Gain | Evidence |
|---|---|---|---:|---:|---:|---|
| 2026-09-20T07:55:37.615086+00:00 | [215fd6d](https://github.com/fywu85/lorr/commit/215fd6d167c96695e9ac6db6f8517791f2dab077) | Generated flow seed 1; turn cost 0.6; K=16; planner seed 0; `--trick RANDOM-05` | 54 | Pending | Pending | [Full evidence](random05/results/flow-first-v4/summary.json) |
| 2026-09-20T07:58:41.020438+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | lanes-intent; K=16; seed 0; `--trick RANDOM-05` | 2113 | Pending | Pending | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T07:58:41.152623+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | flow-intent; K=16; seed 0; `--trick RANDOM-05` | 2230 | Pending | Pending | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T07:58:41.330442+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | flow06-disp08; K=16; seed 0; `--trick RANDOM-05` | 2346 | Pending | Pending | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T07:58:42.118225+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | upstream-intent; K=16; seed 0; `--trick RANDOM-05` | 2431 | Pending | Pending | [Full evidence](random05/results/intent-full-v5/summary.json) |
| 2026-09-20T08:02:13.099706+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | file-k64-disp0-sched0; seed 0; `--trick RANDOM-05` | 2488 | Pending | Pending | [Full evidence](random05/results/scale-full-v5/summary.json) |
| 2026-09-20T08:05:44.745252+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | file-k256-disp0.8-sched1; seed 0; `--trick RANDOM-05` | 2562 | Pending | Pending | [Full evidence](random05/results/scale-full-v5/summary.json) |
| 2026-09-20T08:05:50.770563+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | file-k256-disp0.8-sched0; seed 0; `--trick RANDOM-05` | 2594 | Pending | Pending | [Full evidence](random05/results/scale-full-v5/summary.json) |

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
