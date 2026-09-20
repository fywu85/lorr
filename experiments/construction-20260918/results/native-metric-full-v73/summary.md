# TRICK: native cost model reaches151,630

The native20/200, turn1 cost model with the NMS+1bands completes **151,443 /151,630**
tasks on full seeds0/2. Without bands it completes149915/149871. The exact legacy
remaining-potential controls are147422/147502; paid controls146659/146566. All eight
full runs and40,000entries pass. All four control trajectories reproduce exactly.

| Native field | Seed0 gain vs remaining control | Seed2 gain | Mean gain | Final1000 gains |
|---|---:|---:|---:|---|
| Without bands |2493|2369|1.64856%|629 /569|
| With bands |4021|4128|2.76308%|937 /1035|

Both native fields reduce outstanding-age p90, empty work, loaded turns and loaded
waits. The band arm's empty work is62.50/62.43steps per completed task. Complete
motion/cohort replay follows; raw aggregate differences do not establish a causal
partition. The native arm jointly changes the field, turn price, raw operation and
service units, fallback scale and scheduler prices. It is not an isolated cheap-turn
experiment or a reproduction of the entire NMS policy.

The best151630 run is seed2, source06a8258, completed2026-09-20T10:25:07.422058+00:00.
Matching and short preference are OFF, pickupweight1. Mean449.312ms per entry,
maximum1.016591s, RSS11.937GB; average1.563CPU cores out of four reserved. Full wall
time38.51minutes. Shared5s development, not strict1s/exclusive-core certification.
The goal remains **3165tasks** away, requiring2.0873% growth. The generic reference
is unchanged. [Best source/configuration/hash](best-record.json),
[all comparisons](comparison.json), [full verification](verification.json).
