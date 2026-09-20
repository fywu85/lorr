# Native assignment: saved-run motion and task-cohort audit

All twelve full trajectories are replayed:600million physical robot steps. Input
and trajectory fingerprints, independent base/phase fixtures and field-mismatch
rejection pass. Lane counts classify directions only; they are not native weighted
cost or band-visitation accounting. The quality/source/resource checks remain in
the linked full comparison.

| Profile / seed | Tasks | Completed shortest chain | Empty steps/task | Loaded overhead/task | Loaded waits/task |
|---|---:|---:|---:|---:|---:|
|trick_native_band0_match0_pickup1-s0-r0|149915|242.294|63.791|18.337|8.850|
|trick_native_band0_match1_pickup1-s0-r0|150510|242.340|62.734|18.082|8.624|
|trick_native_band0_match1_pickup5-s0-r0|150565|242.988|61.625|18.270|8.803|
|trick_native_band1_match0_pickup1-s0-r0|151443|242.283|62.498|16.370|7.690|
|trick_native_band1_match1_pickup1-s0-r0|151901|242.317|61.662|16.203|7.564|
|trick_native_band1_match1_pickup5-s0-r0|152383|242.954|60.340|15.888|7.220|
|trick_native_band0_match0_pickup1-s2-r0|149871|242.334|63.844|18.361|8.885|
|trick_native_band0_match1_pickup1-s2-r0|150406|242.314|62.788|18.305|8.859|
|trick_native_band0_match1_pickup5-s2-r0|150526|242.992|61.668|18.302|8.812|
|trick_native_band1_match0_pickup1-s2-r0|151630|242.340|62.430|16.113|7.449|
|trick_native_band1_match1_pickup1-s2-r0|152066|242.293|61.631|15.928|7.287|
|trick_native_band1_match1_pickup5-s2-r0|152048|242.968|60.485|16.425|7.725|

These are observed cohort and motion differences, not a causal savings partition.
In particular, the weight5 increment is mixed on the band field (+482/-18 tasks
versus weight1), despite lower empty travel on both seeds. The next short-task
comparison is intended to test whether task selection can improve the remaining
gap; it is not assumed to succeed. [Replay checks](accounting.json),
[full quality comparison](../native-assignment-full-v78/summary.md).
