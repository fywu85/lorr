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
