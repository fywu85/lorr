# Resident learned-flow task-chain prices

V57-r3 is a default-off generic scheduling experiment. GRID8899300 completed the
full regression suite on 2026-09-20T04:37:58UTC using four reserved physical cores
on a shared host. This is functional validation, not a throughput result.
[Build/source manifest](build-provenance/v57-r3/build.json),
[test receipt](build-provenance/v57-r3/status.json).

`CGAR_CHAIN_FLOW_PRICING` selects native (0), resident whole-chain quotes with
native fallback (1), resident quotes with snapshot-ratio imputation (2), ratio-only
pricing (3), or unchanged-policy shadow diagnostics (4). All are independent of
map geometry. Mode4 ranks alternatives over the same retained native shortlist;
it does not alter actual scheduling or claim a complete assignment counterfactual.
Modes1–4 require published learned pickup guidance and reject static tricks,
remaining-flow temporal scores, guide routes and task rematching.

A quote requires every nontrivial remaining leg to have a current resident
oriented-distance table. It minimizes the start heading independently for each
leg, so it is a surrogate rather than an exact chained-orientation cost. Missing,
out-of-domain, unreachable or invalid chains fall back as a whole. Const peeks
build no tables, change no cache recency and use no random draws. Original spatial
chain estimation and its cache behavior remain intact. Age preference, oldest-task
admission and started/primary/recovery protection are preserved.

The tests cover hand-computed tolled choices in all five modes at two metric
scales and three coverage levels, imputation arithmetic, whole-chain fallback,
LRU noninterference and refresh invalidation, started-task and oldest-task
protection, rejected configurations, and 2,304 native-versus-shadow robot steps
across both chain-cache modes. All 4,608 actions are checked for collisions.

Fable reviewed this in the existing session. [Assessment](fable-flow-session/turn29/assessment.md)
identifies diagnostic improvements before the warehouse shadow run. No active
policy has been promoted. The shadow must reproduce the 200-step reference hash
before the full 5,000-step seed0 run, under the explicitly labelled shared-host
5-second development budget. Changes in task selection must be evaluated with
completed totals, waiting ages and all empty travel, not short-horizon totals.

V59 incorporates the turn29 diagnostic findings. Per-call shadow rankings now
include every active scheduling tick, with separate unchanged/mode2-only/mode3-only/
both-same/both-different categories and shortlists with fewer than two candidates.
Active assignment counts provide a matching denominator for covered assignments;
native table/approximate basis counts are testable without log parsing. Clipped
native chains are excluded from the snapshot ratio and counted separately.

Build8899322 passes the complete suite, including60 hand-computed production cases
across both native chain-cache implementations. Basis, nonzero resident coverage,
missing coverage, published tolls, nonzero ratios and unchanged scheduling/cache
work are asserted. The warehouse shadow remains a diagnostic until exact native
and mode4 hashes are verified. Active pricing is still not promoted.
[Build/test receipt](build-provenance/v59/status.json).

The V59 startup screen8899323/analysis8899324 is valid and both native and shadow
trajectories exactly match the200-step reference hash. All71 active calls through
timestep199 have snapshot and ranking records. Nonzero resident coverage is0.34%
on the first post-publication call,3.62% over calls2–16,20.12% over17–64 and31.51%
over65–71. Snapshot ratios span approximately1.0161–1.0188. Coverage is weighted by
repeated task observations. Conditional alternative first choices do occur, while
actual scheduling and actions remain identical. No throughput gain is claimed.
[Evidence](results/chain-shadow-screen-v59/coverage.json).
Full seed0 shadow8899328, held analysis8899329, runs on four verified physical cores
of shared research46 with the5s development deadline. It must reproduce the full
native144,510 trajectory before this diagnostic is used to choose active trials.
