# Motion replay of all four full margin cases

Completed14:02:25UTC, job8899864. All200million robot steps replay against the map,
with exact input hashes, full phase-action totals and task boundary fixtures.
No planner errors, schedule errors, timeouts or assignments after pickup.
Both controls exactly reproduce prior minimal-horizon trajectories. See
[combined unfinished-work comparison](../horizon-margin-work-audit-v94/summary.md).

The completed spatial-chain means rise242.188→242.327 on seed0 and242.188→242.306
on seed2. Empty robot-steps increase40,192/33,813. The throughput gain is associated
with much less work left on unfinished tasks at the cutoff, rather than a shorter
completed-chain mix. Cohorts change, so these observations do not identify a
complete causal decomposition or establish a steady-state throughput gain.
