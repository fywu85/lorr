# Fractional turns and mixed complete workers

The loaded-motion audit and persistent Fable turn11 review motivate two independent
optional policy tests. Neither is a reproduced bug fix or an established throughput
improvement. The reference remains the six-seed64-field policy until full evidence
supports a change. [Review and local assessment](fable-flow-session/turn11/).

## Turn surcharge in scaled units

CGAR_TURN_SURCHARGE defaults to0. A nonzero value requires temporal planning,
enabled flow and physical turn cost1. The effective oriented turn cost is
physical_turn_cost * flow_scale + surcharge, bounded by16. With scale4, surcharge1/2
prices a turn at5/6 while the physical slot and base forward price remain4.
Guide routes remain mutually exclusive. No map identities or supplied weights.

The same effective cost reaches the reverse orientation oracle, PIBT's complete
turn-then-forward macro, temporal prefix and virtual terminal-wait compensation,
bounded pickup search and complete pickup fields. These are static routing costs,
not a physical change to execution time or a prediction of waiting. The existing
fallback remains unit-turn because it knows only the first useful corner. The
scheduler still uses the last complete flow publication and approximate spatial
chain costs. Cold scheduling retains its previous metric; oriented planning pays
the configured turn price from initialization, so cold table-build time matters.

## Mixed starts

CGAR_TEMPORAL_MIXED_START defaults to0. Value1 requires warm start and at least two
planning workers. Worker0 receives the previous complete plan's validated suffix;
other workers start cold. Every worker completes its configured fixed work, joins,
and receives the same scalar comparison. Strict score ties retain worker0. Invalid
or unavailable history starts all workers cold. All-warm and all-cold modes retain
their prior behavior when the option is0. Protected actions and complete-plan
validation remain. There is no deadline-based selection of partial alternatives.

New counters record actual warm/cold constructor participation and the selected
worker's start mode. They describe search initialization, not whether every robot
retained its suffix or which worker produced the final post-repair plan.

## Validation

Build8898977 passes all CGAR regressions. New/extended checks include88704 pickup
goal costs from independent repeated graph relaxation, reverse/bounded/complete
basis agreement,12288 independent weighted distances,724608 paid-progress score
bounds,120768 pure-scaling scores, and11520 unchanged neutral production decisions.
Twelve production pickup cases exercise bounded/full/cached costs, rounding,
fair admission, the16-edge boundary and unchanged simulator metadata.

Mixed-worker fixtures compare3840 serial/parallel robot decisions with and without
a fractional metric, verify first-entry all-cold behavior and subsequent worker0
warm participation, retain nonzero valid suffixes, and reject invalid settings.
Existing failure propagation, thread joins, history conflicts, compact fallback,
protected-primary and capacity regressions also pass. These are finite checks,
not a general liveness or performance proof.
[Build and independently reconstructed22source/test hashes](build-provenance/v44/).

Screen8898980 completed surcharge0/1/2 with one4M worker and four cold/all-warm/
mixed workers with surcharge0. All six200-step cases pass, maximum0.935833s and
peakRSS5.233GB. Both zero-surcharge one/four-cold controls exactly reproduce the
v43 prefixes, including paths, schedules, events and tasks. Diagnostic counters
verify199 mixed-history steps with one warm/three cold constructors, the all-warm
and all-cold controls, and the configured effective turn costs.
[Screen and exact controls](results/fractional-mixed-screen-v44/).

Full8898981/analysis8898982 compares all six profiles on seeds0/2, twelve cases
simultaneously on48 physical cores, four per process,192GiB aggregate reservation
and32decimalGB per-process cap. One-worker surcharge0 must reproduce141829/142988;
four-cold must reproduce143340/143409. Compare turn surcharges with one-worker0,
and all-warm/mixed with four-cold. The automatic paired table uses one-worker0;
it does not replace these matched comparisons. All full decisions must meet1s.
Any positive effect requires six-seed confirmation before promotion.
[Configuration](fractional-mixed-screen-variants.json).

## Full results: keep both options disabled

Jobs8898981/8898982 completed all twelve full runs. All60,000 complete entries
meet one second (maximum937.83ms), with peakRSS12.147GB. Every source/test hash
matches exact commitf6d0ae448504762136c4f5d85f25d7d207207965. Both one-worker and
four-cold controls exactly reproduce the prior full trajectories. Disjoint physical
core bindings and absence of a CPU quota were verified.

| Policy | Seed0 tasks | Seed2 tasks | Mean change versus matched control |
|---|---:|---:|---:|
|One worker, surcharge0|141829|142988|Control|
|One worker, surcharge1|141768|141778|−0.446%|
|One worker, surcharge2|141263|142599|−0.335%|
|Four cold workers|143340|143409|Control|
|Four warm workers|142094|142669|−0.693%|
|One warm/three cold workers|142724|142817|−0.421%|

Every candidate loses both full totals to its matched control. Surcharge2 reduces
loaded turns9.04%/9.54%, but loaded waits rise12.86%/10.93%; fewer turns alone do
not imply more completed tasks. Both warm variants also lose both final1,000-step
windows. At step5000, mixed mode records4,999 warm and15,001 cold constructor
runs; all-warm records19,996 warm and4 cold, confirming nonvacuous participation.

Neither option is promoted or queued for six-seed confirmation. The existing
six-seed reference and its defaults remain unchanged.
[Full matched comparisons and verification](results/fractional-mixed-full-v44/comparison.json).
