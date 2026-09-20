# Generic improvements and map-specific tricks

User instruction, 2026-09-20 UTC: keep generalizable improvement separate from
map-specific tricks. If generic improvement proves difficult to close the gap,
map-specific techniques from NMS, KittyKnight or elsewhere may be considered,
provided they are marked as tricks in both commits and the experiment log.

The generic single-run frontier is145,083 under the shared-host5s development policy; the confirmed six-seed reference remains143,941.2.
Static lanes plus bounded matching now reaches147,328/147,204 on full seeds0/2;147,328 is the best overall validated single run. Lane-only146,659/146,566 remains the exact control. [Verified matching factorial](results/trick-match-full-v65/summary.md). Per the user's latest instruction, any valid full seed reaching154,795 may achieve the NMS target, including an explicitly flagged trick. It must remain labelled TRICK and must not be reported as a generic algorithm improvement.

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
