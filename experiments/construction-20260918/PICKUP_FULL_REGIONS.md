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
One round is not promoted. Two-round confirmation is complete below.
[Verified comparison](results/pickup-full-regions-full-v44/comparison.json).

Confirmation8899053/analysis8899054 is complete. All eight additional runs are valid
and all four disabled controls exactly reproduce their V42 full trajectories. Source,
binary, physical allocation, entry deadlines, RSS and25 regional counter frames per
candidate were independently checked. Across all six paired seeds:

| Seed | Disabled | Two rounds | Task change | Final1,000 change | Agep90 change |
|---|---:|---:|---:|---:|---:|
|0|141,829|144,510|+2,681|+287|−3|
|1|143,325|143,933|+608|−108|+1|
|2|142,988|144,107|+1,119|+120|−2|
|3|141,802|143,134|+1,332|+167|−2|
|4|141,988|143,934|+1,946|+408|−6|
|5|142,917|144,029|+1,112|+195|−1|

Mean143,941.2 versus142,474.8 (**+1.029%**); all six full totals improve. Five tails
and ages improve, with the small seed1 regressions preserved above. Promote two
rounds as the benchmark reference. This confirms the earlier single-run high144,510;
it does not set a new high. The mean remains5.91% below local KittyKnight152,981.
All60,000 paired entries meet1s; candidate maximum962.69ms, RSS13.909GB,
mean401.2–419.4ms, whole-process CPU1.726–1.770cores/4reserved. Full runs take
34.50–35.93minutes. Loaded turns fall4.72–13.44% and waits1.78–12.82% across all
six; empty robot-steps change−0.53% to+0.72%. These associations are not causal
savings or runtime-speedup estimates. No map-specific policy or partial plans.
[Full six-seed verification](results/pickup-full-regions-six-seed-v44.json),
[confirmation validation](results/pickup-full-regions-confirm-v44/validation.json),
[working configuration](warehouse-reference-variants.json).
