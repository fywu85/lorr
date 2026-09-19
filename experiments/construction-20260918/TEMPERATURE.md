# Regional temperature ablation

Fable's frozen-v11 review suggested that restarting regional annealing at 0.001
may spend work undoing an already improved global plan. The completed v14
one-round profile retains all four regions in all 25 sampled decisions. For
two rounds, the second round adds only 0.185% to the internal score on average
(at those sampled decisions), versus 2.312% for the first. These diagnostics
motivate an ablation; they do not prove excessive temperature is the cause.
Full task totals are 111,411 for one round and 112,164 for two.

`CGAR_TEMPORAL_REGION_TEMPERATURE_PPM` chooses a prescribed initial temperature
(default 1000 = 0.001; range 0–10,000). Cooling, work limits, boundaries, whole-plan
validation, and explicit deadline failure are unchanged. The global search
still starts at 0.001. Zero rejects every score-decreasing regional attempt;
100 uses one tenth the original temperature. No elapsed-time acceptance or
stopping rule is introduced.

The v17 build and complete regression suite pass. Across 72 dense cases,
regional search improves 52, and lower temperatures change 20 compared with the
matched default. All 3,456 robot selections match between serial and four-thread
execution. The fixtures independently check merged collisions, protected paths,
boundaries and propagated timeouts. Production episodes also cover 100 and 0,
with 4,800 matched actions each. The archived source patch reconstructs every
tested hash. All four 200-step screens pass, with maxima 0.706/0.706/0.694/0.865 seconds
for the control, temperature 100, temperature 0, and two rounds at temperature 0.
Default regional trajectories exactly match v14 for every recorded field.
[Screen evidence](results/temperature-screen-v17/),
[equivalence](results/temperature-default-equivalence.json).

Full job **8898445** tests all four profiles after warm-start jobs 8898475/8898476,
using two four-core instances and 24 GiB total reserved memory. Analysis
**8898447** follows. Its first analysis submission used an incorrect dependency
and was rejected before execution; that failed submission is retained in the
raw directory alongside the successful one.
No new-temperature throughput evidence exists yet.

The pending temperature job was moved behind warm-start validation to prioritize
that larger standalone mechanism. Profiles, binary, resource allocation and seed
remain identical. The scheduler dependency update is retained in
[temperature-queue-update.json](results/temperature-queue-update.json).
