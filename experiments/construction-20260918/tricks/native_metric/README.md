# TRICK: native Warehouse cost model

This is one bounded fidelity experiment inside CGAR. It requires `--trick WAREHOUSE`,
`CGAR_TRICK_NATIVE_METRIC=1`, `CGAR_TRICK_REMAINING_FLOW=1`, lanes ON and cost scale20.
Short preference and matching are OFF. `CGAR_TRICK_NATIVE_BANDS=1` independently adds
the native band surcharge; it requires the native metric. No flag means no trick.

The field uses preferred forward20, opposed200 and turn1. The band arm adds1 to
forward edges in the cells identified by the archived unmodified NMS constructor.
Every free directed value is checked against that verified dump; unused wall slots
are normalized to20. [Asset provenance](asset-generation.json) records all154,344
free directed checks, the two SHA256/FNV fingerprints and the extraction source.

The scalar score is explicitly `50 * P - operation_id`, with raw operation IDs.
If any macro slot reaches the current goal, `P` is the negative last goal slot
(0..4). Otherwise it is the remaining oriented distance; a trailing wait considers
the two90-degree headings and two trailing waits also consider180degrees. There
are no paid forward or turn extras. This avoids a negative `turn_cost-unit_cost`
term when turn1 is cheaper than forward20. Idle robots retain the raw operation
order. The physical simulator still charges one tick per action.

Scheduler pickup quotes use the same native oriented metric and its base20;
spatial chain estimates and age scaling also use20. Thus this simultaneously
reprices route choices, the scalar goal/tie terms and pickup assignment. It does
not isolate a causal cheap-turn effect, copy NMS's entire scheduler, or establish
that the planners are equivalent. CGAR's protection/recovery layer and prescribed
search, cache-build and field quotas remain in place. Missing-table planner
fallbacks retain scaled spatial estimates; they are not native all-pairs tables.

The reverse distance oracle and complete pickup search have explicit positive
cost limits. Existing callers keep limit16 and the complete field's17buckets;
native calls use200/201 and201/202buckets. Bounded pickup remains heap-based with
its prescribed pop limit. Integer path bounds are checked, complete tables only
are published, and wide distances retain lossless32-bit storage with logical-byte
LRU accounting. Native diagnostics count published wide-fallback tables. A timeout
fails the entry; no partial quality result is accepted. Process RSS remains32GB.

Source [06a8258](https://github.com/fywu85/lorr/commit/06a825855718513c3e965a22aac227ad13fd5595),
[exact28-file build binding](../../build-provenance/v73/exact-source-commit.json).
The full regression suite passes, including617,376 oriented-state comparisons,
39,302 independently replayed macro scores with1,416service cases, two native
asset fingerprints, three forward-search oracle profiles, invalid cost/default
bounds, forced80,000distance overflow/eviction, raw scalar hand cases, two real
scheduling/planning profiles and activation/incompatibility gates. Native receipt
and legacy-OFF compatibility checks also pass; an initial Python import-path error
was corrected before those checks ran.

Four-arm200-step screen8899459/8899460 is valid and both legacy prefixes are exact.
Paid/potential controls3709/3805; native no-bands/bands3848/3872. Those are startup
counts, not performance evidence. Native maximum entries1.0044/1.0351seconds and
peakRSS6.078GB are within the shared5s development budget, not strict1s certification.
Separate generic800-step check8899461/8899462 reproduces the promoted matching
reference exactly. Full native comparison8899463/8899464 is running: both native
arms versus exact legacy paid and remaining-potential controls, seeds0/2,32 bound
physical cores. The startup results establish feasibility, not full performance.
