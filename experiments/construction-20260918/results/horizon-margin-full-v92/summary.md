# TRICK: prospective completion margin reaches the NMS target

**154,889 tasks on seed0**,94above the user-specified NMS target154,795. This
satisfies the agreed criterion of at least one valid full seed with explicit tricks
allowed. Seed2 is154,556,239below target; the two-seed mean154,722.5 remains72.5below.
No claim that the generic solver or a validated multi-seed mean beats NMS.

| Seed | Exact minimal-horizon control | Mean-margin ON | Gain | Last1000 gain | Outstanding agep90 change |
|---|---:|---:|---:|---:|---:|
|0|153614|154889|1275|1275|-37|
|2|153220|154556|1336|1336|-39|

All four full5,000-step/10,000-robot runs and20,000complete entries pass the shared
5s development deadline and32,000,000,000-byte processRSS cap, with zero failures,
planner/schedule errors or timeouts. Both OFF totals and entire trajectory hashes
exactly reproduce V86. Both ON action/schedule/event prefixes are exact through
step3804, before the first margin comparison reversal. Full source28-file hashes,
frozen binary, receipts, nonoverlapping physical cores and calibration counters
are independently checked; no calibration lower-bound violation occurred.

Exact production source[92bc748](https://github.com/fywu85/lorr/commit/92bc748e81416de9d66edaec0e83a833ae3bac18).
Binary SHA256 `d3f464a8c6fed95a53f54b6a623eb6492a1b7830f5c3602122914c93711910e8`.
Seed0 completed2026-09-20T13:59:35.054025UTC; verification14:00:08.321810UTC.
Trajectory SHA256 `68fbb514dfab11a80ccbf8b1f4b8af98a4fd2bdd572a6963a4ee865a4ebd0aca`.
[Exact configuration and resource record](best-record.json),
[all controls and pairs](comparison.json), [independent full verification](verification.json).

The winning policy retains native20/200 Warehouse bands, turn1, raw remaining
potential, matching64, pickupweight5 and ordinary fairness. Global short preference
staysOFF. It additionally requires `--trick WAREHOUSE`,
`CGAR_TRICK_KNOWN_HORIZON=5000`, and `CGAR_TRICK_HORIZON_MARGIN=1`.
Known run length is explicitly configured; it is not supplied by SharedEnvironment.
The margin learns only from completed, never-retargeted tasks observed before the
current entry. It changes fresh admission ordering, preserves held/started tasks
and complete fixed-work CGAR motion, and never returns a partial timeout result.
The calibrated mean is a heuristic, not a completion guarantee.

Seed0 mean entry446.648ms, maximum1.011548497s, peakRSS13.330decimalGB,
average1.507CPU cores out of4reserved, full wall2290.514s (38.18minutes).
Seed2 mean445.674ms, maximum1.005681074s, peakRSS11.874GB. Runs shared research38
with16bound physical cores total/four per case. **This is not strict one-second
competition certification**; the small observed peak overruns remain visible.
The target achievement uses the user's explicitly relaxed development conditions.

The separately validated generic reference remains144,392.17 across six seeds.
The later general deferred-selection optimizationbdf5e7a is not the source of this
score; it has independent regression and exact-prefix checks. Reproduce this record
from the frozen V92 source/binary and [single best profile](best-variant.json), seed0,
full5,000steps and explicit Warehouse flag; do not substitute current HEAD silently.

The [saved-work replay](../horizon-margin-work-audit-v94/summary.md) finds
476,776/491,229fewer robot-steps on unfinished tasks (about29%less), while completed
chain means rise slightly. All200million control/candidate robot steps replay
successfully. [Entry timing](../horizon-margin-latency-v94/summary.md) shows no
observed late latency spike; shared-host timing is not a controlled speedup.
