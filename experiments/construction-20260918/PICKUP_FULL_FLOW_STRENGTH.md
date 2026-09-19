# Traffic-cost strength on the complete-field policy

The loaded-motion audit finds residual forward detours, turns and waits in the
confirmed64-field policy. Test whether changing the strength of observed opposing
traffic costs trades these more effectively. Existing graded costs add a strength-
scaled directional imbalance above a25% confidence threshold, on base cost4.

Use strengths2/4/6/8, holding scale4, margin25, refresh512, turn-build128,64 complete
pickup fields/four field threads, ordinary HRRN, and one4M planning worker fixed.
Strength4 is the confirmed control. All edge costs remain positive and<=12, within
the complete-field kernel's1–16 contract. No source changes or map-specific rules.

Screen8898964 completed all four200-step cases on distinct four-core allocations.
All pass, maximum0.888135s and peakRSS5.206GB. Strength4 exactly reproduces the
v42/64 full prefix (paths, schedules, events and tasks). These are deadline and
equivalence checks, not a quality ranking or full-horizon memory guarantee.
[Screen and equivalence](results/pickup-full-flow-strength-screen-v43/).

Full8898971/analysis8898972 compares all four strengths over5000 steps on seeds0/2,
eight simultaneous cases on32 physical cores, four per process,128GiB aggregate
and32decimalGB per-process cap. Verify the strength4 full controls against v42/64
before interpreting effects. Inspect full totals, final windows, outstanding ages
and motion alongside complete-entry timing and memory. A promising policy still
requires six seeds before promotion.
[Configuration](pickup-full-flow-strength-variants.json), [motion audit](LOADED_MOTION.md).
