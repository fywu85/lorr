# The pickup quota binds only in the final 48 steps

A read-only audit of the verified p90 / pickup-weight-8 trajectories finds a small, late exposure to the 64-complete-field quota. The old generic quota comparison was exactly unchanged at 64 / 128 / 256; the stronger horizon-aware profile now has a late completion burst.

| Quantity | Seed 0 | Seed 2 |
|---|---:|---:|
| Full-run tasks | 155,120 | 155,056 |
| First decision with more than 64 newly free robots | 4,952 | 4,957 |
| Decisions exceeding the quota | 29 | 27 |
| Robots beyond the quota, summed over these decisions | 461 | 454 |
| Maximum newly free robots in one decision | 109 | 102 |

All excess occurs in the final 100 steps, representing about 0.3% of post-startup admissions. A quota of 128 would cover the observed demand, but an altered trajectory may have different demand. Excess robots use the existing bounded pickup-search fallback; they are still assigned. The audit does not demonstrate that any of those assignments is worse, nor that those late new tasks can finish before the horizon. More accurate quotes can also change congestion for other robots.

This is a narrower opportunity than the overall 18.6% empty-travel share might suggest. Defer a quota-quality comparison until the active motion and full-depth-worker results are known; do not treat a larger quota as an established improvement. A short startup benchmark would miss this exposure entirely.

Both complete raw and canonical trajectory hashes match. All 25 cumulative complete-field counters per seed equal the sum of the per-decision capped completion counts. There are no goal-less actions, consistent with completion-driven free-robot counts here. Initial tick 0 uses generic mass dispatch; completions at tick 5,000 have no following scheduling entry and are excluded. The final-tick counts are 30 / 40.

Completed 2026-09-20 17:48:59 UTC in GRID job 8900400, one bound physical core, no quota, exit status 0. Source `27be6e312fdd79ad310583d33eeb2a4781d75b97`, binary `37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752`. This adds no solver score. A local submission-preparation error (`shlex.join` unavailable in the system Python) was corrected before any job was submitted and is retained in the provenance.

[Accounting and all exposed ticks](accounting.json), [frozen request](request.json), [audit source](../../native_pickup/quota_audit.py), [physical-action budget](../p90-pickup-combined-v99/work-budget.md).
