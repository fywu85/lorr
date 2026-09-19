# Regional repair with complete pickup fields

The existing generic regional repair previously helped the no-flow planner. This
experiment combines it with the confirmed graded-flow/64-field pickup policy. It
starts from one complete4M-worker plan, repairs disjoint regions in parallel, then
merges complete compatible selections. Cross-boundary paths and protected robots
stay fixed within each round. It does not combine discarded global-worker plans.

All profiles retain the same one-worker reference and differ only in regional
repair: disabled, one round or two rounds; four regions, four repair threads and
25,000 fixed attempts per region per round. Every configured attempt and merge
finishes, or the entry fails explicitly. No wall-clock selection or map identity.
The existing V44 source is unchanged; all22 hashes match exactf6d0ae4.

Screen8899001 completes all three200-step cases. Disabled exactly reproduces the
prior full-prefix fingerprint. Maxima are875.99/922.75/961.48ms, and peakRSS5.277GB.
Regional counters show actual search and retained improvements in both enabled
profiles, with each completed round preserving or improving the scalar score.
These are feasibility/mechanism checks, not throughput acceptance.
[Screen](results/pickup-full-regions-screen-v44/equivalence.json).

Full8899002/analysis8899003 is complete. All six runs are valid; all30,000 entries
finish within1s, peakRSS13.909GB, four disjoint physical cores per process with no
CPU quota. All22 source/test hashes match exactf6d0ae4, the binary matches the
frozen build, and both disabled full trajectories exactly reproduce the V42/64
reference. Twenty-five sampled regional counters per enabled run show real repair,
fixed attempt counts, retained changes and nondecreasing merged scalar scores.

| Rounds | Seed0 tasks | Seed2 tasks | Mean effect | Final1,000 differences |
|---|---:|---:|---:|---|
|Disabled|141,829|142,988|reference|reference|
|One|142,237|141,623|−0.336%|−150 / −576|
|Two|144,510|144,107|+1.334%|+287 / +120|

Two rounds lower agep90 by3/2 steps, loaded turns by13.44%/5.12% and loaded waits
by12.82%/4.68%. Empty robot-steps change+0.72%/−0.20%. Mean entry401.2–419.4ms,
max953.19ms, CPU1.726–1.770 cores of four reserved, peakRSS13.909GB. These motion
associations do not prove an additive causal decomposition of the throughput gain.
One round is not promoted; two rounds need six seeds before promotion.
[Verified comparison](results/pickup-full-regions-full-v44/comparison.json).

Confirmation8899053/analysis8899054 runs disabled/two-round profiles on seeds1/3/4/5:
eight concurrent cases, four disjoint physical cores each,32 total,128GiB aggregate
reservation and32decimalGB per-process RSS cap. Same frozen V44 build. Controls
must exactly reproduce their confirmed V42/64 full trajectories.
[Confirmation configuration](pickup-full-regions-confirm-variants.json).
