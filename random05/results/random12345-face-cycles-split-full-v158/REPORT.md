# Bounded topology face-cycle comparison

Frozen source1bd28fee/build158;32 full archived-input runs across all five general and trick profiles. Independent replay, source/input hashes, resource guards and strict timing checks are complete. All ten disabled controls reproduce all six complete trace fields exactly.

| Profile | Off | Maximum face length8 | Length16 | Length32 |
|---|---:|---:|---:|---:|
| general RANDOM-01 | 727 | 727 | 727 | — |
| trick RANDOM-01 | 729 | 729 | 729 | — |
| general RANDOM-02 | 1397 | 1397 | 1397 | — |
| trick RANDOM-02 | 1408 | 1408 | 1408 | — |
| general RANDOM-03 | 1634 | 1616 | 1587 | — |
| trick RANDOM-03 | 2620 | 2606 | 2604 | — |
| general RANDOM-04 | 1595 | 1616 | 1597 | — |
| trick RANDOM-04 | 2782 | 2725 | 2745 | deadline failure |
| general RANDOM-05 | 2226 | 2189 | 2172 | — |
| trick RANDOM-05 | 4242 | deadline failure | deadline failure | deadline failure |

Only general RANDOM-04 improves:1,595→1,616 on the same planner seed (+1.32%). It remains39.00% below the matched max(NMS,KK)=2,649. The selected trick score2,782 is unchanged. A repeated-seed qualification is required before treating the general gain as reliable. All other selected profiles are unchanged or worse.

The four failed variants exceed the first-step strict1s bound: RANDOM-04/length32=1,027.104ms; RANDOM-05/length8,16,32 all exceed1s. Their original failures are retained; no throughput score is assigned. The on/off jobs use separately allocated cores on shared hosts, so time differences alone do not establish causation.

[Full independent audit](audit.json), [exact disabled controls](control-equivalence.json). Default remains off.
