# End-of-run work after the calibrated margin

Both saved candidate runs account for all100million robot steps. Current control
trajectories exactly reproduce the previous minimal-horizon control, and their
independently replayed phase counts agree with the reused old work accounting.
All input hashes and the handoff/unfinished-task boundary fixture pass.

| Seed | Full tasks before → after | Work on unfinished tasks before → after | Reduction | Completed chain mean before → after |
|---|---|---|---|---|
|0|153614 → 154889|1632309 → 1155533|476776 (29.21%)|242.188 → 242.327|
|2|153220 → 154556|1655252 → 1164023|491229 (29.68%)|242.188 → 242.306|

The extra completions accompany476,776/491,229fewer robot-steps spent on tasks
still unfinished at the cutoff, about29%less. Completed chains are slightly
longer on average; global short-task preference staysOFF. For first admissions
at/after4600, finished counts rise4603→5282 and4555→5302, with completion fractions
33.58%→35.27% and33.39%→35.43%. Empty work across all tasks increases40,192/33,813.

These are observed differences across changing task cohorts and trajectories,
not an exact causal partition of the throughput gain or a bound on future gains.
The policy improves the late cutoff outcome; no new steady-state rate is claimed.
