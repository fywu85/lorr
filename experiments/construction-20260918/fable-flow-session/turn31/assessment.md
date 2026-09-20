# Local assessment of Fable turn31

Read-only Fable5.1/max in the existing session, completed2026-09-20T06:24:43.634376UTC.
No tools/tests were run by the reviewer. Reported cost/token totals are cumulative
session accounting, not this request alone. The CLI result contains only the last
visible segment; read emitted-findings.md for the full visible answer. Raw protocol
and private reasoning remain excluded from Git.

Adopt the main diagnosis: short preference lowers completed chain length while
raising empty work enough to lose throughput. Existing pool exchange is limited
to32 events every10 ticks (at most16000 in this horizon), uses unit-distance
prices, and runs after ordinary assignment. It is a weak candidate for recovering
the roughly3.2million extra empty steps in these short-preferring runs. Keep the
TRICK guards rejecting the old swap/pool environment flags; no new exception is
implemented. Fable explicitly withdraws its turn30 suggestion to copy that pass.

Qualifications to the review:

- The mean interaction is−51 tasks, but individual effects are−997/+895. Two
  seeds do not establish stable additivity across workloads.
- The age numbers2687/2713 are for unassigned tasks, not all unpicked tasks.
  Held-unopened tasks have a separate distribution.
- Empty steps per completion plus mean loaded steps of completed tasks omit
  unfinished loaded work. They are useful cohort accounting, not a throughput
  identity or a causal forecast. No speculative+1–3% gain is accepted.
- A task's one-time retarget marker bounds retargets before pickup; event rate
  per completion also depends on initial stock, revealed tasks and incomplete
  tasks. Half the assignment rate is not a universal finite-horizon bound.
- The old exchange's numerical cap and unit metric make it unpromising here,
  but they do not prove it cannot help through changed congestion/cohorts.
- Held pickup tables are not guaranteed resident. Fixed table budgets, refresh
  invalidation and eviction apply. A complete robot-centered field is a valid
  common metric only when it is fully computed for that source/heading.
- Spatial/weighted pickup saving is not realized empty-time saving. A ratio
  calibrated on one policy's assignments is not a causal conversion for new
  retargets, and repeated shadow opportunities can double-count a stale option.
- A finite retarget budget prevents infinite resets per task; it does not
  independently prove service fairness or global liveness for the new scheduler.
  Short preference deliberately preserves no finite unpicked-wait bound.
- Transient fields still consume memory. Additional workers/buffers need a
  measured peak; RSS cannot be assumed unchanged.

Possible next TRICK study, deferred while generic pricing/search trials run:
complete robot-centered fields for a fixed quota of eligible unopened holders
before ordinary assignment. Shadow only initially, with exact baseline hash,
new/old pickup and total-cost comparisons in one metric, deduplicated opportunities,
protection/quota counts, CPU/RSS, and explicit timeout failure. A broader active
retarget budget and quota256 are reviewer proposals, not accepted constants or
measured performance. Any active trial would preserve started/primary/recovery
protection and require a separate CLI selector, receipts and full paired controls.
