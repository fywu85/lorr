# Second fresh-input validation protocol

Declared on 20 September 2026 before generating the new inputs or observing
any results. This is a separate validation set from seeds 50001 and 50002,
which remain excluded from tuning.

Freeze source **e896201**, build-v45, and the exact settings in
`experiments/fresh-validation-v2-full.json`: the confirmed four-core 3,501-task
configuration, planner seed 3, K2048/B8/start2/local0, with exact prefix reuse,
packed priority order and sparse dispersion counting. Guidance field 15, one
flip with seed 5, and known-horizon triage remain explicit `--trick RANDOM-05`.
No risk penalty, search generations or later experiments are included.
The machine-readable protocol pins both solver binary hashes.

Generate base seeds **50003 and 50004** with the unchanged archived map helper:
800 distinct starts, 100000 tasks of 2–5 uniformly sampled stops, reveal pool
1.5 times the agent count, map random-32-32-20. Record generator hash, NumPy
version, actual agent/task RNG seeds and every input hash. These are new local
validation inputs, not the colleague's private instances.

For each input, run the frozen candidate once and the existing combined NMS
four-worker build twice. All six runs use four physical EPYC 9354 cores,
2,000 steps, a 1,000 ms entry deadline, 30,000 ms initialization and a 32 decimal
GB process limit. Use research33/research39, with checked core binding and CPU
model; these are shared hosts. NMS retains all combined-track features and the
previously documented constructor initialization fix.

Report every outcome, both NMS repetitions, validity, latency and RAM. Use the
stronger valid NMS result per input for the matched ratio and aggregate ratio.
Never silently replace an input or hide a failed run. These inputs cannot be
used to select configuration changes while continuing to be called held out.
Ongoing development continues solely on the archived development input.

## Completed results

The second validation freezes source [e896201](https://github.com/fywu85/lorr/commit/e896201)
at protocol [e6b2dbe](https://github.com/fywu85/lorr/commit/e6b2dbe), before generating
seeds 50003/50004. It tests the 3,501 development configuration with the same
four-core / 1s / 32GB rules. All six runs are valid; both NMS repeats are retained.

| Instance seed | Candidate completed UTC | Candidate tasks | NMS repeats | Gain over stronger NMS | Evidence |
|---|---|---:|---|---:|---|
| 50003 | 2026-09-20T14:50:00.584389+00:00 | 3494 | 2946 / 2984 | +17.1% | [Matched audit](results/fresh-validation-v2/audit.json) |
| 50004 | 2026-09-20T14:50:17.990772+00:00 | 3387 | 2892 / 2875 | +17.1% | [Matched audit](results/fresh-validation-v2/audit.json) |

Aggregate: **6,881 versus 5,876 tasks (+17.1%)**. This supports an improvement
on two new task/start inputs on the same map. It remains below the colleague's
reported 27–28%; the two validation versions use different inputs, so their
11.8% and 17.1% figures are not a paired effect estimate. Keep all four validation
inputs out of configuration selection. Full per-run latency, CPU time, RSS,
input/binary hashes and allocation are linked in the audits.

| Run | Mean / max entry (ms) | Peak RSS (decimal MB) | CPU user seconds |
|---|---:|---:|---:|
| seed50003-nms-repeat1 | 950.3 / 951.0 | 1686.4 | 7589.7 |
| seed50003-nms-repeat2 | 950.3 / 951.3 | 1687.2 | 7595.2 |
| seed50003-ours | 459.6 / 577.7 | 286.3 | 3646.7 |
| seed50004-nms-repeat1 | 950.3 / 950.5 | 1686.3 | 7595.2 |
| seed50004-nms-repeat2 | 950.3 / 950.9 | 1686.7 | 7596.0 |
| seed50004-ours | 463.2 / 578.5 | 286.6 | 3675.9 |
