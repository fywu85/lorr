# Deferred selection-store component probe

Completed2026-09-20T13:34:21.255572UTC, job8899794, one bound physical core on shared
research44. All128observations (two fixture seeds, four modes, eight alternating
pairs each) pass exact plan/score-bit/work-counter checksums and independent
vertex/edge validation. Each observation performs20complete4M-candidate trials.
Baseline headers reconstruct exactly from7b29df5609a6b5245ab167242517fc98916c9719;
candidate is the archived patch. Both binary hashes and identical compiler flags
are recorded. No production source changed during this probe.

| Fixture seed | Mode | Median paired candidate/baseline CPU | Wall |
|---|---|---:|---:|
|81473|mode0|0.985154|0.985276|
|81473|mode1|0.990154|0.992133|
|81473|mode2|0.986935|0.985155|
|81473|mode3|0.992908|0.993859|
|2749|mode0|0.984578|0.981882|
|2749|mode1|0.986046|0.988696|
|2749|mode2|0.984464|0.984448|
|2749|mode3|0.986687|0.987816|

Modes0/1/2/3: cold global, region-filtered, valid warm plan with unequal weights,
and region-filtered with actual clock callbacks. CPU reduction0.71–1.55% across
the eight settings is component evidence only, not a measured whole-entry gain
or throughput improvement. Production regressions and warehouse equivalence are
required before adoption. Timing is on a shared host, using the same core with
alternating pair order; process CPU is reported separately from elapsed time.
