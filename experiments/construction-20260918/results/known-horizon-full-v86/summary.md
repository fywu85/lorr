# TRICK: configured horizon raises the best to 153,614

All four full 5,000-step / 10,000-robot runs pass all 20,000 complete entries, the 32 decimal GB RSS cap and shared-host 5s deadline. The exact source is [4e1a8fe](https://github.com/fywu85/lorr/commit/4e1a8fe5e995e275faa6ddfaa0e8f8ae61fc5352). Both no-horizon controls reproduce V80 exactly. Actions, schedules and events match before the first logged comparator reversal, at ticks 3843/3828.

| Seed | Ordinary control | Configured H=5000 | Gain | Final 1,000 gain | Age p90 change |
|---|---:|---:|---:|---:|---:|
|0|152383|153614|+1231|+1231|+5|
|2|152048|153220|+1172|+1172|+7|

The paired mean rises 0.78934%. The new best leaves **1,181 tasks (0.76881% growth)** to the user-specified NMS target of 154,795. The complete [best record](best-record.json) binds the configuration, seed, binary/trajectory evidence and actual completion time. Generic reference remains 144,392.17; this is an explicitly gated TRICK result.

The rule changes new admissions only: physically impossible robot/task pairs rank behind lower-bound-feasible ones. Ordinary HRRN order remains within each tier; forced-oldest admission, held/started work and matching stay unchanged. The horizon is an explicit configured assumption, not information supplied by SharedEnvironment. A bound-feasible task is not guaranteed to finish, and a later reassignment can rescue an initially impossible pair.

ON mean latency is 445.669/446.128ms, maximum 989.051/989.619ms, average CPU use 1.504/1.506 cores out of four bound physical cores per case, and peak RSS 11.912/11.938GB. Full runs take 38.11/38.15 minutes. Observed maxima are below one second in these shared5s runs; this is not formal exclusive-host or strict1s certification, nor a controlled speedup claim.

Empty robot work increases 86,961/90,917 steps. All throughput gains occur in the final 1,000-step window; outstanding-age p90 rises 5/7 ticks. Saved-trajectory work and physical-bound audits are queued to examine the remaining loss before choosing another policy. Neither shortest-feasible ranking nor calibration is implemented in this source.
