# Generic improvements and map-specific tricks

User instruction, 2026-09-20 UTC: keep generalizable improvement separate from
map-specific tricks. If generic improvement proves difficult to close the gap,
map-specific techniques from NMS, KittyKnight or elsewhere may be considered,
provided they are marked as tricks in both commits and the experiment log.

The generic single-run frontier is145,083 under the shared-host5s development policy;
the confirmed six-seed reference is144,392.17. The native20/200Warehouse field with
+1bands, turn1 and raw remaining potential reaches **151,443/151,630** on full
seeds0/2. **151,630** is the best overall validated run. Matching and short preference
are OFF, pickupweight1. All eight native/legacy cases valid; controls exact.
[Verified comparison](results/native-metric-full-v73/summary.md).
Any valid full seed reaching154,795 may achieve the user target, including an
explicitly flagged trick; it must remain labelled TRICK.

For a map-specific implementation or experiment:

- Put **`[trick]`** in the commit title and label its progress-log entry **TRICK**.
- Require the simulator command-line flag **`--trick <map-instance-name>`**
  (for example, `--trick WAREHOUSE`) as the sole switch enabling map-specific
  behavior. No implicit detection, default activation, or environment-only
  activation. Use a separate `trick_...` benchmark profile, record that exact
  flag in commands/logs, and identify the generic baseline and source commit.
  The first experimental implementation is [NMS-derived warehouse lane directions](tricks/nms_warehouse/README.md).
  The user also requested [short-task preference](tricks/short_tasks/README.md) as an explicit TRICK, despite that preference itself being generic. Component environment settings only select behavior after the CLI flag; they cannot activate it alone.
- State its map/layout/data assumptions and origin: for example NMS's warehouse
  lane mask and coordinate bands, or KK's supplied warehouse edge-weight file.
- Compare full runs against that baseline; report all tested seeds, failures,
  task-age tradeoffs and resource usage. Use the single-seed criterion for overall goal achievement and best-run logging; retain multiseed confirmation as the separate generic-reference promotion standard.

Generic mechanisms can use obstacle connectivity, heading, revealed tasks and
observed traffic without depending on a particular map identity or coordinate
template. A topology-derived estimator still needs a measured benefit; calling
it generic is not a performance claim.

Both tracks retain complete prescribed-work decisions, explicit timeout failures,
collision/schedule validity, processRSS below32,000,000,000bytes, and reserved/bound GRID physical cores. Shared hosts and5-second development
deadlines are now user-authorized; later strict1s checks remain separate. No hidden future tasks are used. The existing generic work continues;
the short-task × lane factorial is complete: short preference loses 5.961%/5.902% without/with lanes, while empty work and waiting ages rise. It remains OFF. [Full paired result](results/trick-short-tasks-full-v58-r2/summary.md).

General implementation follow-up V81 (source85faca5): backward Dial relative ring
indexing preserves distances and prescribed work. Weighted kernel CPU medians
improve9.6–15.8% in paired tests; no whole-entry or throughput claim. Four native
shortOFF/ON trajectories reproduce V80 exactly; generic isolation is pending.
[Kernel evidence](results/turn-ring-kernel-v81/summary.md),
[native equivalence](results/turn-ring-native-screen-v81/summary.md).

V78 full native assignment is verified:12validfull cases/60kcomplete entries/four
exact unmatched controls. New overall TRICK best152383, seed0 nativebands +remaining
potential +matching64 +pickupweight5, shortOFF. Sourcec6d9755; completed11:10:21.393890UTC.
Gap2412 (1.58285%growth needed). Band matchedweight1:151901/152066; weight5:152383/152048.
Weight5's seed2increment-18 is retained; both profiles beat unmatched controls.
Shared5s development, bestmean450.823ms/max1.054859s/RSS11.880GB; no strict1s claim.
[Evidence](results/native-assignment-full-v78/summary.md). Eight fullshort cases
8899568/analysis8899569 now use exact-equivalentV81source85faca5 on32boundcores;
results/native-short-full-v80. SavedV78motionreplay8899570 runs independently.
