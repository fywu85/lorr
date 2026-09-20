# TRICK: remaining potential and matching combine successfully

The combined static-lane policy completes **148,132 / 148,101** tasks on full
seeds 0/2. It improves on score-only 147,422 / 147,502 by **710 / 599**, and on
matching-only 147,328 / 147,204 by **804 / 897**. Mean gains are 0.4438% and
0.5775% respectively. Both final 1,000-step windows improve against both controls.
All four control trajectories reproduce exactly.

Empty travel falls by about 1.3–1.4 robot steps per completed task relative to
either control. Loaded waits also fall; turns are slightly lower than score-only
and higher than matching-only. These are trajectory associations, not a causal
partition of the gain. Task cohorts can change. Short preference stays OFF.

All six full runs, 30,000 complete entries, are valid under the shared-host 5s
budget and 32GB process RSS cap. Every case has four distinct reserved physical
cores. Both combined maxima are below one second, but this is not strict1s
certification. The best run averages 314.964ms per entry, maximum 976.717ms,
uses 11.939GB RSS and averages 1.833 CPU cores out of four reserved. Full wall time
is 27.38 minutes. Seed 0 completed at 2026-09-20T09:45:49.302966+00:00.

Exact source [c696d5f](https://github.com/fywu85/lorr/commit/c696d5f9d5b9b5bc6557830e6e43680913b3e7b7).
[Best configuration, timestamp and hash](best-record.json),
[complete comparison](comparison.json), [independent verification](verification.json).
This is the new overall best, **6,663 tasks** below NMS154,795. The generic
six-seed reference remains unchanged. All map-specific behavior requires
`--trick WAREHOUSE`; remaining potential and matching64 are explicitly ON.
