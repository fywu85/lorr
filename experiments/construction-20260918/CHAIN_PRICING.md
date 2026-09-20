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

V60-R3 adds20 controlled production scheduling cases across a changed publication,
covering both native caches, scales1/4 and modes0..4. The first quotes, complete
flush/native fallback and rebuilt quotes are checked against hand costs. All
regressions pass; production sources and the binary are exactly V59. Earlier
fixture failures and their correction are [documented](build-provenance/v60-r3/README.md).
Source4964309. The prepared next comparison is native0 versus resident/imputed2
versus ratio-only3, preserving age preference, ordinary matching and search work.
It is not promoted; the checks and launch below supersede the earlier pending status.

Full shadow verification completed 2026-09-20T06:12:12UTC: 144510 tasks and the
exact full baseline trajectory, all 5000 decisions valid, max959.305ms and
11.358GB peak RSS on shared research46. All 4871 active calls through t4999 have
coverage/ranking reports. Nonzero chain coverage by elapsed calls is 0.575% at1,
4.990% at2–16, 21.492% at17–64, 41.242% at65–256 and 54.170% after256. The snapshot
ratio ranges1.01612–1.02308. Among143309 native retained shortlists:109800 unchanged,
27016 mode2-only,2066 mode3-only,3004 both-same,1423 both-different. These are
conditional first-choice rankings, not actual assignment changes or throughput.
[Verified shadow](results/chain-shadow-full-v59/verification.json), [coverage](results/chain-shadow-full-v59/coverage.json).

Active startup8899334/analysis8899339 passed for native0, resident/imputed2 and
ratio-only3. The control is exact, and all128 prepublication action/schedule steps
are identical across arms. Startup totals do not establish a quality ranking.
[Screen](results/chain-pricing-screen-v60/comparison.json). Full job8899340 compares
all three modes at seeds0/2 concurrently, four physical cores per case on shared
research50,5s deadline/32GB RSS. Frozen source4964309/binaryfbee2a5; held independent
analysis reports complete totals, age tails, empty work and mode2 versus mode3.
