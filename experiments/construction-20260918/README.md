# Warehouse construction study

This study separates the uniform-cost NMS reference's initial construction from
its later improvement search. It uses identical robot positions and goals,
including late full-run states, and records whether construction finishes before
its own deadline. These fixed-goal motion probes diagnose coordination; full
5,000-step lifelong runs remain the criterion for adopting CGAR improvements.

The active CGAR control is `CGAR_ORIENTATION_GUIDANCE=1 CGAR_TURN_FIRST=1
CGAR_TURN_TABLE_MB=8192`. Its six-seed mean is 43,176.17 tasks. The valid local
KittyKnight reference completed 152,981 with a larger CPU allocation and more
than 32 GB memory. The CGAR target remains below 32,000,000,000 measured bytes,
one-second complete decisions, and generic rules without map-specific policies.

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
