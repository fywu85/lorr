# Current dense CGAR configurations

Both use source [8352226](https://github.com/fywu85/lorr/commit/83522266d6ae8c88241d11e91a125e30848cbca9),
binary SHA256 `7bbe4e2b516ffc76dee9d7e429194b7797475673dc2de9bde0d156e529e6e0d0`.
The frozen executable and build manifest are under
`runs/cgar-random-native-build-v1-r2-20260920/` (raw files are not committed).
The [build evidence](guidance/build-v1/verification.json) verifies all31 source/test
hashes and the complete regression suite.

| Instance | Track | Best / seed | Mean over seeds0,2,4 | Profile |
|---|---|---|---:|---|
| RANDOM-04 | GENERIC | 1503 /0 | 1492.33 | [best-random04.json](best-random04.json) |
| RANDOM-05 | TRICK | 2608 /2 | 2570.00 | [best-random05.json](best-random05.json) |

Use full1000/2000-step horizons and `--time-limit-ms 1000` in the matrix runner,
with four physical cores per process and32 decimal GB RSS cap. RANDOM-05 requires
both `--instances RANDOM-05` and `--trick RANDOM-05`; RANDOM-04 has no trick flag.
Profiles preserve certificates, CGAR primary/recovery paths and fixed complete
search work. They do not enable known-horizon triage or the short-task trick.

[Full three-seed comparisons](scheduling-three-seed-summary.json),
[RANDOM-04 seed2/4 evidence](../results/random04-scheduler-seeds-v2/summary.md),
[RANDOM-05 seed2/4 evidence](../results/random05-scheduler-seeds-v2/summary.md),
[timestamped record history](../BEST_HISTORY.md).

These are three planner seeds per archived instance. The standalone RANDOM-05
solver is a separate algorithm and its working tree, tests and jobs were untouched.
The historical NMS targets2547/3050 are still above these records.
