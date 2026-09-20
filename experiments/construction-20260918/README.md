# Warehouse construction study

This study separates the uniform-cost NMS reference's initial construction from
its later improvement search. It uses identical robot positions and goals,
including late full-run states, and records whether construction finishes before
its own deadline. These fixed-goal motion probes diagnose coordination; full
5,000-step lifelong runs remain the criterion for adopting CGAR improvements.

The original CGAR control was `CGAR_ORIENTATION_GUIDANCE=1 CGAR_TURN_FIRST=1
CGAR_TURN_TABLE_MB=8192`, with a six-seed mean of43,176.17 tasks. The current
[benchmark reference](warehouse-reference-variants.json) adds temporal planning
and refreshed observed-flow guidance, complete pickup fields and regional repair,
reaching **143,941.2 mean tasks over all six seeds**. Its
[full evidence](results/pickup-full-regions-six-seed-v44.json)
and the root [throughput history](../../WAREHOUSE_PROGRESS.md) supersede the
original control as the current performance summary. The valid local
KittyKnight reference completed 152,981 with a larger CPU allocation and more
than 32 GB memory. The CGAR target remains below 32,000,000,000 measured bytes,
one-second complete decisions, and generic rules without map-specific policies.

The [current bottleneck and literature review](bottleneck_review/REPORT.md)
accounts for all 300 million robot steps of the six-seed reference, compares the
active competitor implementations, and records the next experiments after the
persistent Fable review. V50 preserves the existing full trajectories; its small
runtime optimization is not a new throughput record.

All NMS modifications are applied to a frozen experimental source copy. The
archived implementation and the active CGAR planner are unchanged by the probe.
NMS remains a clock-limited diagnostic reference; an interrupted construction
must never be described as a completed fixed-work policy.

The subsequent [full-run results](RESULTS.md) establish a 107,413-task three-seed
mean for the optional temporal profile. [PROBE.md](PROBE.md) explains the native
construction diagnostic and kernel conformance. [Fable's review](fable/review.md)
and the [checked assessment](fable/assessment.md) cover safety, timing and the
next experiments. Source patches and build checks are retained under
`build-provenance/`; compact matrices, hashes and resource records are under
`results/`. The original probe did not modify the active planner; the opt-in
implementation was added afterwards and is described in the full-run report.

[ROUTING.md](ROUTING.md) records the subsequent weighted-turn and regional-repair
experiments, their deadline-only screens, full-run queue, and validation limits.
