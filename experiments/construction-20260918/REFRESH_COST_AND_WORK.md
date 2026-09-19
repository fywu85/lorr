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


## Completed refreshed-cost comparison

All six full cases pass, and both scale1 controls reproduce the prior complete
trajectory fingerprints. Scale2 scores133,232 /132,682 (mean132,957), and
scale4 scores130,359 /131,288 (mean130,823.5). Against scale1's134,515 mean,
these lose1.16% and2.74% respectively. Gentler penalties are not promoted.
[Full contrast and validity evidence](results/flow-refresh-scale-full-v33/comparison.json).

The four-worker1M summaries have also finished at51,791 /42,001 tasks, with
valid deadlines. Final selected-worker construction counts exceed the nominal
1M allowance and leave zero repair attempts at step5000. This arm therefore
is not an exact equal-work comparison: construction is mandatory and may
overshoot. Full trajectory analysis waits for the4M-per-worker arm to finish.


## Completed fixed-worker comparison

All six cases and independent analyses pass, and both one-worker controls
reproduce prior full fingerprints. Four workers with4M allowance each score
134,975 /134,603 (mean134,789), only0.204% above the one-worker134,515 mean.
Their final1,000-step counts are28,244 /28,248. The four1M workers deteriorate
to4,812 /4,835 in the final window; both are valid completed runs, not timeouts.

The4x4M arm uses additional fixed search work but supplies little paired gain,
so it is not the next default. More CPU by itself does not close the gap.
The1M arm's final selected workers inspect9.51M /4.46M candidates in construction
and perform zero repair attempts. Actual total work across all workers was not
logged; these selected-worker counts cannot establish an equal-compute test.

[Full throughput, resource costs and selected-worker work](results/flow-refresh-workers-full-v33/comparison.json).
