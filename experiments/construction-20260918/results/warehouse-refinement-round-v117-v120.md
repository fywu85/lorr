# Warehouse refinement: 20 full runs, current best retained

Completed 20 September 2026. All 20 runs complete 5,000 steps with 10,000 robots,
with zero planner/scheduler errors and timeouts, exact matched control trajectories,
and independent accounting of one billion robot actions. All processes fit below
32 decimal GB. These are selected solver seeds of the same input under the
permitted shared-host 5-second development deadline, not strict one-second
certification or independent competition instances.

**Best remains 155,173**, seed 0, 378 above the user-specified NMS target 154,795.
The confirmed guarded profile remains 155,173 / 155,056 / 155,090 / 155,008 on
seeds 0–3, mean 155,081.75. No new configuration is promoted in this round.

| Change | Full candidate totals | Paired changes | Decision |
|---|---|---|---|
| Guidance turn price 4, guard OFF, all four seeds | 155,095 / 155,115 / 155,146 / 154,861 | −25 / +116 / +90 / −159 | Two wins, two losses; mean +5.5. Keep price 1. Seeds 0/2 are earlier V113 evidence; this round adds 1/3. |
| Add matching cutoff guard at turn price 4, seeds 0/2 | 155,164 / 155,144 | +69 / −2 against guard OFF; −9 / +54 against guarded price 1 | Mixed; no new record. |
| Precompute every native distance table, seeds 0/2 | 155,135 / 154,767 | −38 / −323 | Keep prewarming OFF. |
| Larger unwarmed cache only, seeds 0/2 | 155,173 / 155,090 | 0 / 0, exact full trajectory hashes | No throughput gain; an isolated timing study may be worthwhile. |
| p95 cutoff margin, guarded profile, seeds 0/2 | 155,077 / 155,036 | −96 / −54 | Keep p90. |
| p99 cutoff margin, guarded profile, seeds 0/2 | 154,811 / 154,787 | −362 / −303 | Keep p90. |

Prewarming completes its map-only preparation in about 23.5 seconds within the
unchanged 30-second initialization limit. Full RSS reaches 19.69 GB and observed
entry peaks fall to about 0.747 seconds, but trajectories change and throughput
falls. The larger unwarmed cache uses 15.32 GB and preserves both trajectories.
Shared-host timing comparisons do not isolate speedups. Ordinary planning still
uses one global worker and four preparation/regional/pickup threads; these cache
experiments reserve eight physical cores per process for preprocessing, while
the other matrices reserve four.

Every task revealed in the first half of each run completes. No initially revealed
task remains never assigned. HRRN, forced-oldest admission, held/started and CGAR
primary/recovery protections remain; at most one unopened retarget occurs, with
none after pickup. Some waiting-time tails worsen, and these finite observations
do not prove starvation freedom. All Warehouse policies require
`--trick WAREHOUSE`; the configured 5,000-step horizon remains an explicit trick.

The refreshed physical-work audit finds no large remaining loaded-motion gap to
the saved NMS diagnostic. Empty work is about 18.65% of robot steps, but much of
it is necessary pickup travel. Broader pairing and better prospective cutoff
prediction remain research possibilities, not established gains. The earlier
hard 16/16 mixed grouping and doubled matching cadence already lost, so repeating
those changes is not the next step.

Evidence: [hashed round index](warehouse-refinement-round-v117-v120.json),
[turn price four-seed aggregate](native-turn-four-seeds-v118.json),
[additional turn seeds](native-turn-seeds13-v118/summary.md),
[turn plus guard](native-turn-guard-full-v119/summary.md),
[prewarming](native-prewarm-full-v117/summary.md),
[cutoff quantiles](cutoff-quantiles-full-v120/summary.md), and
[current physical-work diagnosis](current-best-work-audit-v121/summary.md).
