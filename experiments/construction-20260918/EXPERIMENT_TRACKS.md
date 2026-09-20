# Generic improvements and map-specific tricks

User instruction, 2026-09-20 UTC: keep generalizable improvement separate from
map-specific tricks. If generic improvement proves difficult to close the gap,
map-specific techniques from NMS, KittyKnight or elsewhere may be considered,
provided they are marked as tricks in both commits and the experiment log.

The generic frontier remains144,510single-run tasks and143,941.2over six seeds.
There is no CGAR trick-track result yet. A trick-assisted score must not replace
the generic frontier or be reported as a generic algorithm improvement.

For a map-specific implementation or experiment:

- Put **`[trick]`** in the commit title and label its progress-log entry **TRICK**.
- Require the simulator command-line flag **`--trick <map-instance-name>`**
  (for example, `--trick WAREHOUSE`) as the sole switch enabling map-specific
  behavior. No implicit detection, default activation, or environment-only
  activation. Use a separate `trick_...` benchmark profile, record that exact
  flag in commands/logs, and identify the generic baseline and source commit.
  This is the required interface for future tricks; none is implemented yet.
- State its map/layout/data assumptions and origin: for example NMS's warehouse
  lane mask and coordinate bands, or KK's supplied warehouse edge-weight file.
- Compare full runs against that baseline; report all tested seeds, failures,
  task-age tradeoffs and resource usage. Keep the existing strict promotion criteria.

Generic mechanisms can use obstacle connectivity, heading, revealed tasks and
observed traffic without depending on a particular map identity or coordinate
template. A topology-derived estimator still needs a measured benefit; calling
it generic is not a performance claim.

Both tracks retain complete prescribed-work decisions, explicit timeout failures,
collision/schedule validity, processRSS below32,000,000,000bytes, and isolated GRID
allocations. No hidden future tasks are used. The existing generic work continues;
the competitor guidance ablations will help choose the first trick experiment.
