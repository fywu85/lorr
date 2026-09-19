# Refreshed cost magnitude and fixed parallel search work

After full follow-ups, refresh256 slightly loses to512, and warm reuse adds only
0.283% over seeds0/2. The next bounded tests use existing validated options on
frozen v33; they do not change planner source or introduce map-specific policies.

## Cost magnitude with a refreshed field

Scale1/2/4 expresses one physical slot as1/2/4 integer units, with a strength1
contraflow surcharge of1. Relative penalties are therefore100%/50%/25%. Every
physical score term is scaled consistently, as validated in FLOW_COST_SCALE.md.
This tests a different setting from the completed frozen-field scale experiment.

Screen8898671 passes all three200-step profiles. The scale1 prefix exactly
matches ordinary refresh512; max entries0.774367/0.778038/0.776137seconds,
RSS<3.682GB. No additional refresh occurs by200; prefix task counts are not ranked.
Full **8898673 /8898674** tests all three scales at seeds0/2,5000steps,
six concurrent cases,24reserved physical cores/72GiB allocation. Each case has
four physical cores, a complete1-second decision and a32GB process RSS check.

## Fixed search work across four workers

The existing independent-worker implementation finishes all prescribed searches,
joins every worker, and deterministically selects the best scored complete result.
No clock determines a winning subset; a timeout from any worker fails the decision.
Work allowances count candidates per worker, including mandatory construction;
complete attempts may overshoot the prescribed allowance.

Screen8898672 passes all three200-step cases on four physical cores:

| Workers | Candidate allowance per worker | Wall seconds | Max entry seconds |
|---|---:|---:|---:|
| 1 | 4million | 43.204 | 0.778860 |
| 4 | 1million | 31.078 | 0.526211 |
| 4 | 4million | 45.391 | 0.801996 |

All peak RSS values are below3.681GB. The one-worker control exactly matches the
prior prefix. The other prefixes differ intentionally; their short task counts do
not rank full throughput. The4x1M arm probes restarts with the same nominal total
allowance;4x4M adds work using the reserved cores. Actual candidate counts and
full deadlines still require inspection.

Full **8898675 /8898676** tests all three profiles at seeds0/2, with the same
six-way allocation and per-case limits as the cost study. Neither full experiment
has an outcome at submission; no new setting is promoted.

[Cost screen](results/flow-refresh-scale-screen-v33/),
[worker screen](results/flow-refresh-workers-screen-v33/),
[cost profiles](flow-refresh-scale-variants.json),
[worker profiles](flow-refresh-workers-variants.json).
