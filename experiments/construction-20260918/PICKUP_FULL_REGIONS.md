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

Full8899002/analysis8899003 runs all three profiles on seeds0/2 concurrently:
24 physical cores, four per process,96GiB aggregate reservation and32GB per-process
RSS cap. Default controls must exactly reproduce141829/142988 and their full
trajectories. Assess full totals, final1,000-step windows, outstanding task ages,
loaded motion, all5,000 entry deadlines and memory. Any useful candidate needs the
remaining four seeds before promotion.
[Configuration](pickup-full-regions-variants.json).
