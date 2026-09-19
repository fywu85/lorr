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
tested hash. A 200-step screen is running as job 8898441 to check deadlines
and default-trajectory equivalence, followed by full runs for passing profiles.
No new-temperature throughput evidence exists yet.
