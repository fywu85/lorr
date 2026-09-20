# Complete pickup kernel: identical work, lower CPU time

The cyclic bucket index removes runtime division from complete pickup searches.
On one bound AMD EPYC9354 core,24 alternating-order pairs per profile (after two
warmup pairs) produce exactly identical distances, pops and settled-state counts.
Thread CPU time is measured independently from wall time.

| Field | Previous median CPU ms | Ring median CPU ms | Change |
|---|---:|---:|---:|
| Uniform1 |1.806495|1.385050|−23.33%|
| Legacy4/16, turn4 |1.751335|1.524085|−12.98%|
| Native20/200, turn1 |1.905670|1.703545|−10.61%|
| Native20/200 with bands, turn1 |1.934220|1.751795|−9.43%|

The two legacy profiles also match the original V72 constant-modulo implementation
exactly; CPU medians improve12.32%/8.81% relative to that older implementation.
This addresses the default-path concern raised in Fable36. These are kernel
measurements on a shared host, not whole-entry speed or task-throughput gains.
The full regression suite passes. New trajectory controls follow before using
the optimized binary for later comparisons. [Measured results](comparison.json),
[exact source](exact-source-commit.json), [build](../../build-provenance/v77/exact-source-commit.json).
