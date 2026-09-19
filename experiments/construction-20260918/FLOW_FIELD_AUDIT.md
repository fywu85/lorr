# Frozen-field and motion diagnosis

Read-only GRID audit **8898630** reconstructs the learned field from the first
128 actual trajectory actions and replays the complete 5,000-step motion records.
It compares the three frozen margin-50 flow seeds with the matched seed-2 no-flow
control. All four action totals exactly match independent production movement
counters. Each active field's observed moves and penalized edge counts exactly
match its production publication log. A hand-counted directional-graph fixture
and exact 50% margin boundary check pass.

All three fields have **one strongly connected component containing all 38,586
free cells even after penalized directions are removed**. Thus the particular
hypothesis that this seed's neutral-cost graph partitions the warehouse into
unreachable regions is false. This does not establish adequate capacity, short
routes, or successful multi-robot coordination. Every penalty remains soft.

| Last 1,000 steps | Good flow seed 0 | Good flow seed 1 | Collapsing flow seed 2 | No-flow seed 2 |
|---|---:|---:|---:|---:|
| Forward actions | 8,686,072 | 8,654,553 | 2,694,082 | 8,388,451 |
| Wait actions | 645,927 | 666,323 | 4,654,656 | 681,833 |
| Turn actions | 668,001 | 679,124 | 2,651,262 | 929,716 |
| Opposite turn without intervening forward | 46,741 | 51,374 | 740,346 | 74,286 |
| At most 16 distinct cells in final 256 steps | 0 | 0 | 933 | 0 |
| No forward in final 100 steps | 0 | 0 | 4 | 0 |

The last row is a snapshot, not a claim about every earlier interval. All counts
include the full robot fleet. The opposite-turn counter allows intervening waits
and does not prove every event is avoidable. Only three robots in collapsing seed
2 visit at most four cells in the final 256 steps. The broad failure consists of
slow, inefficient movement and repeated turns rather than thousands of completely
immobile robots at the final snapshot. The first two 1,000-step windows still have
8.39M/8.41M forward actions; degradation develops later.

Similar penalty counts do not mean identical fields. The good seed-0/seed-1
fields share 8,508 penalized directed edges and differ on 17,148. Seed0/seed2
share 8,312 and differ on 17,665. These are descriptions, not evidence that any
particular changed edge causes the collapse. First-window edge classifications
apply the eventual frozen field retrospectively, including the warmup period.

A focused next test reuses the existing `CGAR_TEMPORAL_WARM_START` mechanism with
the frozen margin-50 field, holding the binary, 4M work and other policy settings
fixed. That tests whether carrying a compatible previous short plan helps reduce
replanning churn. Prior no-flow warm experiments were modest; their result does
not settle the failing guided seed. Warm starts are not promoted without full
seed-0/seed-2 comparisons. Refresh and penalty scale are separate experiments.

[Generic analysis source](flow_field_audit.py),
[full evidence](results/flow-field-audit-v1.json),
[provenance](results/flow-field-audit-v1-provenance.json).
