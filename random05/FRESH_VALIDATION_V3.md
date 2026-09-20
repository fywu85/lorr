# Third fresh-input validation protocol

Declared before generating seeds50005/50006 or observing their solver results.
Source **6ce9312**, build-v52, exact K5120/B8/start2/local0/four-generation
configuration that completed3,637 tasks on four cores. Planner seed3; mean
continuation score (no variance bonus/penalty), one parent per generation.
All exact CPU optimizations are enabled, including goal rows, radix ordering
and the per-state ranking cache. The manifest and protocol JSON pin settings
and both executable hashes. No later experimental solver may be substituted.

Field15/one flip with flipseed5/contrast2.4 and known-horizon triage2000/scale1.5
remain declared with `--trick RANDOM-05`. Unchanged map helper generates800
unique starts and100000 tasks of2–5 uniform stops with a1.5-agent reveal pool.
Map: random-32-32-20. Record the helper hash, NumPy version, RNG offsets and all
input hashes. These are new local validation inputs, not the colleague's inputs.

For each new input: one candidate run, two NMS4 repetitions. All six use four
physical EPYC9354 cores on research33/39,2000 steps, a strict1000ms entry limit,
30000ms preprocessing and a32decimalGB process address-space guard. Shared hosts
are allowed. NMS retains all combined-track features and its documented
constructor-initialization fix; only the worker constant differs from NMS32.

Report every attempt, both NMS repeats, all errors/timeouts, latency and RSS.
Use the stronger valid NMS repeat per input for the ratio and aggregate. Never
silently restart, substitute a configuration/input, or omit a failed run.
Existing50001–50004 and these new50005/50006 remain excluded from tuning.
Development continues on the original archived input, including planner-seed
checks; those results do not replace this frozen candidate.
