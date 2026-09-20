# Warehouse construction study

This study separates the uniform-cost NMS reference's initial construction from
its later improvement search. It uses identical robot positions and goals,
including late full-run states, and records whether construction finishes before
its own deadline. These fixed-goal motion probes diagnose coordination; full
5,000-step lifelong runs remain the criterion for adopting CGAR improvements.

The original CGAR control used orientation guidance with an8GiB turn table and
a six-seed mean43,176.17. The current [generic reference](warehouse-reference-variants.json)
adds temporal planning, observed-flow guidance, complete pickup fields, regional
repair and bounded matching, reaching144,392.17over six solver seeds. The separate
explicit Warehouse TRICK now reaches155,057tasks, above the NMS target154,795.
It uses native Warehouse guidance and a configured horizon with a prospective
p90 completion margin; ordinary fairness stays enabled.
[Current full result](results/horizon-percentile-full-v96/summary.md) and the
[root history](../../WAREHOUSE_PROGRESS.md) supersede historical probe summaries.
Current development allows shared hosts and5s complete-entry deadlines while
retaining the32,000,000,000-byte RSS cap; strict1s certification remains separate.

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
