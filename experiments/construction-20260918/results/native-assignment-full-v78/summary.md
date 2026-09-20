# TRICK: native assignment raises the best full score to152,383

All twelve5,000-step/10,000robot runs pass:60,000complete entries, all28source/test
hashes, installed native-field fingerprints, fixed work,48disjoint bound physical
cores, no CPU quota, shared5s development deadlines and32GB process RSS. The four
unmatched native controls exactly reproduce V73. Frozen source c6d9755, binary
acfb4f113824faca286e864b524ac26b682c79009dd16554fcfa51b06d3d449a. Every arm requires
`--trick WAREHOUSE`; short preference is OFF and ordinary age/fairness stays ON.

| Field | Matching groups | Pickup weight | Seed0 | Seed2 |
|---|---:|---:|---:|---:|
|no bands|OFF|1|149915|149871|
|no bands|64|1|150510|150406|
|no bands|64|5|150565|150526|
|bands|OFF|1|151443|151630|
|bands|64|1|151901|152066|
|bands|64|5|152383|152048|

Matching alone improves+595/+535 without bands and+458/+436 with bands. Raising
pickupweight from1to5 adds+55/+120 without bands, and+482/-18 with bands. Thus
weight5 improves the paired mean but is not uniformly better; the seed2band loss
remains recorded. All four matched policies beat their unmatched controls on both
full seeds. No unmatched/weight5 arm was tested, so no standalone weight effect or
factorial interaction is inferred.

The best seed0band/matching64/weight5 run finishes152,383tasks, up753 from the prior
best151,630. Its own unmatched seed0control is151,443 (+940); final1000steps improve
+263, outstanding-agep90 falls30, and empty work falls270,076robot steps. The seed2
weight5 total is152,048 (+418 over unmatched, -18 versus matched/weight1); its tail
improves+99 over unmatched and+2 over matched/weight1. Both weight5 profiles reduce
empty work, while loaded-wait effects differ by seed. Cohorts and routing change
together; these are measured accounting differences, not a causal decomposition.

Best-run mean entry450.823ms, maximum1.054859s, peakRSS11.880GB, average1.498CPUcores
of4reserved, wall38.58minutes. Shared5s development; not strict1s certification.
Remaining target gap2,412tasks, requiring1.5829%growth from this best. Generic
reference144392.17 and generic best145083 remain unchanged.

[Best source/configuration/timestamp/hash](best-record.json), [full validation](verification.json),
[paired effects](comparison.json), [matching counter audit](assignment-diagnostics.json).
Savings divided by20 are forward-base guidance units, not physical steps;
protected/exposed counts are repeated observations, not unique missed opportunities.
