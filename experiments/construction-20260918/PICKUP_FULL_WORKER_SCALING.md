# Additional complete planning workers

Eight and sixteen complete alternatives were tested with four million work units
per worker. Every worker finishes before score selection. The confirmed reference
still uses one worker; the earlier four-worker candidate was not promoted because
seed5 regressed during its six-seed confirmation.

Screen8898975 completed4/8/16 workers with16 physical cores per process. All three
200-step prefixes meet one second, and the four-worker control exactly matches
its earlier four-core trajectory. Maxima are902.5/918.8/968.9ms; peakRSS5.283GB.
[Screen and exact control](results/pickup-full-worker-scaling-screen-v43/).

Full8898978/analysis8898979 completed all four cases on64 physical cores,16 per
process,64GiB aggregate reservation and32decimalGB per-process cap. Every one of
20,000 complete entries meets one second; peakRSS11.976GB. All22 source/test hashes
match exact V43 commit1661176ca3dd6854a7ba8d1a8a0ec8e33a2cffa9. Core bindings are
disjoint and there is no CPU quota.

| Workers | Seed0 tasks | Seed2 tasks | Mean tasks | Change versus one worker |
|---|---:|---:|---:|---:|
|1 (prior control)|141829|142988|142408.5|Control|
|4 (prior candidate)|143340|143409|143374.5|+0.678%|
|8|143157|142645|142901.0|+0.346%|
|16|143749|143571|143660.0|+0.879%|

Eight workers improve only one seed against the confirmed policy and lose both
full totals to four workers. Sixteen workers improve both full totals and final
1,000-step windows against both policies. Against one worker, they add1,920/583
tasks, final windows rise227/199, empty robot-steps fall0.15%/1.00%, and outstanding
age p90 falls6/2 steps. This supports confirmation, not a new established reference.

| Workers | Mean step latency | Maximum step | Average CPU cores | Peak RSS |
|---|---:|---:|---:|---:|
|8|347.9–352.9ms|925.84ms|4.04–4.06|11.956GB|
|16|379.5–392.0ms|954.12ms|7.43–7.68|11.976GB|

CPU is whole-process user+system time divided by elapsed time. These runs reserve
16 physical cores per process, whereas the earlier one/four-worker runs reserve
four. The fixed-work, same-CPU-family comparison measures policy quality; it is
not an equal-resource runtime-speedup claim. Memory is the whole-process high-water
mark. [Full comparisons and resource evidence](results/pickup-full-worker-scaling-full-v43/comparison.json).

## Six-seed confirmation underway

Seeds1/3 run in job8898991 (analysis8898992), and seeds4/5 in job8898993
(analysis8898994). Each matrix runs four cases concurrently on64 physical cores:
one/sixteen workers per seed, with16 cores reserved per process. Both use the
frozen V43 binary,64GiB aggregate reservations and the32GB per-process cap.
The one-worker controls must exactly reproduce their existing full trajectories.
All six seeds must be assessed before changing the confirmed reference.
[Confirmation configuration](pickup-full-workers16-confirm-variants.json),
[first-pair configuration](pickup-full-worker-scaling-full-variants.json).
