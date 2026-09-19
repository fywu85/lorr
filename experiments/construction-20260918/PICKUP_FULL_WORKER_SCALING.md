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

## Six-seed comparison complete

Jobs8898991/8898992 and8898993/8898994 completed the remaining seeds with matched
one-worker controls. All12 one/sixteen-worker runs are valid, all60,000 entries meet
one second, and all six controls exactly reproduce V42/64 trajectories. Every
source/test hash matches exactV43/1661176.

Sixteen-worker seeds0..5:143749/143241/143571/143643/141876/142587, mean
**143111.1667**, **+0.44663%** versus142474.8333. This is the highest observed
six-seed mean so far. Paired totals change+1920/−84/+583/+1841/−112/−330, and final
windows change+227/−87/+199/+280/+47/−101. Three totals and two tails regress;
there is no uniform improvement or established significance from these six seeds.
Keep the one-worker working reference while testing regional repair. No new
absolute record exceeds the existing143767, and no confirmed-reference milestone
is added.

Across six sixteen-worker runs, mean latency is326.1–392.0ms, maximum954.12ms,
average CPU6.27–7.68 cores of16 reserved, and peakRSS12.048GB. The broader range
supersedes the first-pair resource range when discussing this six-seed cohort.
[Full verification and paired data](results/pickup-full-workers16-six-seed-v43.json),
[confirmation configuration](pickup-full-workers16-confirm-variants.json).
