# Eight parallel regions with the confirmed pickup policy

A second bounded regional hypothesis increases four regions/four repair threads to
eight regions/eight threads, retaining two rounds and25,000 fixed attempts per
region per round. Both profiles use one global4M worker, the confirmed64-field
pickup policy, temperature1000ppm, next-errandOFF and read-only peak auditing.
Partitions still derive only from grid dimensions. No map identity or lane template.

This doubles regional attempt work from200,000 to400,000 per full decision and
narrows each region; it may improve parallel search or lose useful cross-boundary
moves. Both effects are unknown. Both arms reserve eight physical cores per process,
so the four-region control must exactly reproduce its prior four-core trajectory.
This is a policy/work experiment, not a claim of equal-work speedup. Actual process
RSS must remain below32decimalGB and every complete decision must finish within1s.

Start with200steps for feasibility and exact-control checks. If both pass, compare
full0/2 and require the remaining four seeds before promotion. The cumulative peak
counters observe every batch; more scalar gains do not imply more completed tasks.
[Profiles](regional-eight-variants.json), [peak audit](REGIONAL_PEAKS.md).

Screen8899146 is complete: both profiles valid, maxima975.44ms (four regions) and
974.06ms (eight), peakRSS5.289GB, eight disjoint physical cores per process and no
CPU quota. Four-region trajectory and every cumulative peak counter exactly match
the preceding four-core screen. Eight regions execute all3,200batches/80million
attempts over200steps; only one batch loses a peak, by256 scalar units. These are
feasibility and mechanism observations, not throughput acceptance.
[Screen verification](results/regional-eight-screen-v48/equivalence.json).

Full8899150/analysis8899151 now compares four versus eight regions on full seeds0/2:
four simultaneous cases, eight physical cores each,32cores/64GiB aggregate,32GB
processRSS cap. Same frozen V48 binary in all cases. Four-region full trajectories
must reproduce the confirmed regional reference; assess complete totals, tails,
ages, motion, limits and all cumulative peak counters before accepting any effect.

All22 frozen V48 source/test hashes match exact commit [8ba2484](https://github.com/fywu85/lorr/commit/8ba2484dd5f1b9bfc719f5f1e8f4533d7543a7bd).
