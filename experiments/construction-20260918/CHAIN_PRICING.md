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
