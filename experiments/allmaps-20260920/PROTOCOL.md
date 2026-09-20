# Cross-instance throughput and fairness protocol

Frozen starting policy: 2026-09-20, before the new capped-region comparisons.

1. Finish the existing full Warehouse V123 check at1000ms; require both source
   and input hashes, all5000complete entries, no errors/timeouts, RSS below
   32,000,000,000bytes, and exact seed0/2 trajectories. Retain failed attempts.
2. Use the archived MR24 inputs and full horizons for quality rankings. Reserve
   disjoint physical cores with GRID, record topology/CPU model and process CPU
   work. Shared5s runs remain development evidence; final promotion needs1000ms.
   A benchmark stops and fails on a deadline, never returns a partial score.
3. NMS is the primary reference. Preserve supplied combined-track features.
   Rerun NMS and KK on matched input hashes, horizon and declared CPU allocation
   when existing references are capped, invalid, or otherwise unmatched. Keep
   published competition scores in a separate column. Report reference memory
   and timeout violations without treating a failed reference as zero throughput.
4. Preserve CGAR identity, safety, primary/recovery commitments and valid
   started-task ownership. Fair admission and retarget policies may be compared
   for throughput; disclose effects on waiting as secondary metrics. Record
   excluded tasks, parked robots and capacity restrictions;
   restricted-workload scores cannot establish a whole-workload fairness claim.
   A standalone solver in another directory is not a CGAR improvement.
5. Generic changes use observable topology/workload statistics or the same
   configuration across inputs. Hand-tuned fields, instance-specific settings,
   and known-horizon rules require an explicit `--trick <instance>` gate,
   `TRICK` result labels and `[trick]` commits. Do not enable them by filename
   without the flag. The current CLI only implements WAREHOUSE gating.
6. Record completed tasks; release-to-first-assignment and release-to-pickup
   delays; completed release-to-completion latency; outstanding ages atp90/p99
   and maximum; initially released tasks still unfinished and never assigned;
   retarget count and assignments after pickup. Separate never assigned, never
   opened, and unfinished tasks. Censoring at the horizon is not completion.
7. Compare shared task-ID prefixes and identical reveal cohorts where possible.
   Faster solvers reveal later tasks earlier, so whole-pool age summaries can
   otherwise mix different populations. Include task-chain length strata and
   completion fractions for initial and first-half cohorts. Do not infer fairness
   from the longest completed task alone or claim a proof from a finite run.
8. Preserve every selected single-seed best with source commit, exact settings,
   input/binary hashes, timestamp and trick status. Confirm engineering changes
   on paired planner seeds; freeze candidates before independent input tests.
   Do not replace held-out candidates after seeing their results. Keep throughput
   and waiting-time tradeoffs visible instead of calling every faster run fairer.

Milestone per instance: match or exceed the qualified NMS throughput reference.
The latest user clarification makes throughput primary and fairness secondary.
A waiting-time regression does not veto a faster candidate; quantify and disclose
it. Claim less starvation only when the observed metrics support that statement.
A20%lead is a separate stretch target, not the admission criterion.

RANDOM-05 is read-only and owned by another agent; no duplicate campaign jobs
or modifications under `random05/` are authorized for this campaign.
