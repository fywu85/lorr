# Frozen flow with warm-start reuse

Full **8898632 / 8898633** uses frozen v31 on an exclusive EPYC 9354 host,
four physical cores per instance, two concurrent instances, all 5,000 steps,
and the usual one-second decisions and 32 GB process RSS check.

| Seed | Cold construction | Warm reuse | Final 1,000 with reuse | Max entry with reuse (s) |
|---|---:|---:|---:|---:|
| 0 | 122,896 | 123,373 | 25,344 | 0.764623 |
| 2 | 70,171 | 123,251 | 25,281 | 0.759598 |

All four runs pass. Peak RSS is below 11.916 GB. The controls reproduce the
old full trajectory hashes exactly. Warm reuse recovers seed2 while leaving the
traffic-field policy frozen, and yields only a small seed0 gain. The two-seed mean
123,312 remains below refresh512's 134,515. This does not prove why the initial
field failed: changing the local construction also changes subsequent trajectories.
No six-seed promotion or additive gain with periodic refresh is established.

[Full evidence](results/flow-warm-full-v31/),
[control equivalence](results/flow-recovery-controls-v30-v31.json),
[screen](results/flow-warm-screen-v31/),
[configuration](flow-warm-reuse-variants.json).
