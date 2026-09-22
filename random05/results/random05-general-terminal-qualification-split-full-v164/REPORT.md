# General RANDOM-05 terminal-credit qualification

The2,314 record repeats exactly in all six full trace fields. Four paired planner seeds give **+3.4599% aggregate** over disabled credit, with three gains and one loss. All seven qualification runs and both original selecting-pair runs pass strict full-run checks and independent replay.

| Planner seed | Credit1 | Credit0 | Difference |
|---|---:|---:|---:|
| 5 | 2314 | 2226 | +88 |
| 6 | 2147 | 2082 | +65 |
| 7 | 2147 | 1997 | +150 |
| 8 | 2303 | 2308 | -5 |

Totals 8911 versus 8613; means 2227.75 versus 2153.25. The selecting seed5 is included. Largest entry across all nine runs is595.950ms; peakRSS491.377MB. All runs use full2,000steps,32workers on16bound EPYC9354cores,1s entries and32decimalGB guard.

The general record2,314 remains **27.05%below matched max(NMS, Kitty Knight)=3,172**. The selected trick record4,302 is separate and its qualification is still pending. No map-specific guidance or known horizon is used in this general profile. These are planner seeds on one development input, not independent task/start inputs or unseen layouts.

Frozen qualification commit4d94594a; source164/30a56647. [Paired data](paired-comparison.json), [independent audit](audit.json), [exact repeat](control-equivalence.json).
