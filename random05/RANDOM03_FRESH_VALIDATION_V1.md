# RANDOM-03 frozen fresh-input validation v1

Completed 2026-09-21 08:15 UTC. **+10.41% aggregate** against the stronger of two unmodified NMS repetitions on each fresh input. Individual gains are +11.69% and +9.13%; this does not establish a ten-percent advantage on every input.

The archived candidate scores 2,602 versus matched NMS 2,359 (+10.30%). Its exact repeat preserves all six action/schedule/event/task fields. Planner seeds 5/0/3 give 2,602/2,548/2,566, with maxima 661/655/674 ms in this qualification batch; the original record peaked at 710 ms.

Source [02bccaa7](https://github.com/fywu85/lorr/commit/02bccaa7), build-v102. The [protocol](experiments/random03-fresh-validation-v1-protocol.json) and [case manifest](experiments/random03-fresh-validation-v1-full.json) were committed in [0fb8a953](https://github.com/fywu85/lorr/commit/0fb8a953e571d66e37aa0bd4d01eba34f60f6c5c) at 07:55:00 UTC, before generation at 07:55:03.289264 UTC. Task/start seeds 50011 and 50012 remain excluded from tuning.

Both candidate and previous archived 2,580 configuration use the same binary. Their sole configuration difference is the known-horizon triage scale, 0.875 versus 1. This is an explicit `--trick RANDOM-03` configuration. All runs use the full 800 steps, 16 physical EPYC9354 cores /32 SMT workers, 32 decimal GB process guard, 30-second preprocessing, and strict one-second steps. Candidate work is fixed; no timed partial result is returned.

| Input seed | Candidate | Previous | NMS repetition 1 | NMS repetition 2 | Gain over stronger NMS | Candidate mean/max ms |
|---|---:|---:|---:|---:|---:|---:|
| 50011 | 2599 | 2590 | 2318 | 2327 | +11.69% | 507.8/700.1 |
| 50012 | 2557 | 2535 | 2343 | 2322 | +9.13% | 471.9/675.6 |

Aggregate candidate 5,156 versus stronger NMS 4,670 (+10.4069%), and previous 5,125 (+0.6049%). Candidate improves over the previous configuration on both inputs. All eight runs finish with no planner, scheduler or timeout errors. Independent replay verifies all movement, collision, assignments and task events. These are fresh task/start streams on the same layout, not unseen maps or the colleague's private inputs.

| Run | Longest completed order, steps | Oldest unfinished, censored steps | Initial orders still unfinished |
|---|---:|---:|---:|
| seed50011-ours | 797 | 800 | 50 |
| seed50011-baseline | 791 | 800 | 52 |
| seed50011-nms-repeat1 | 788 | 800 | 59 |
| seed50011-nms-repeat2 | 785 | 800 | 53 |
| seed50012-ours | 795 | 800 | 27 |
| seed50012-baseline | 748 | 800 | 29 |
| seed50012-nms-repeat1 | 780 | 800 | 43 |
| seed50012-nms-repeat2 | 789 | 800 | 40 |

Waiting times do not change throughput selection. Unfinished orders are censored at the horizon; their eventual maximum wait is unknown.

The initial audit invocation expected PILOT's `entryComputeSamples` field in NMS output. NMS instead reports the full combined entry in `plannerTimes`. The auditor now validates exactly 800 finite subsecond timing samples for both schemas and checks their maximum against the summaries; PILOT still must provide its additional entry sample count. This corrects the checker without changing any binary, run, threshold or frozen configuration.

Evidence: [source/resource/protocol/timing audit](results/random03-fresh-validation-v1/audit.json), [independent replay and waiting audit](results/random03-fresh-validation-v1/replay-audit.json), [archived repeat proof](results/random03-record2602-validation-split-full-v102/control-equivalence.json), [runtime observations](results/random34-runtime/REPORT.md).
