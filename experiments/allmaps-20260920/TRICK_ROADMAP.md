# General mechanisms and explicit instance tricks proceed together

Updated 2026-09-21T04:30:40.141812+00:00. Throughput is primary; fairness is secondary. Every instance
trick requires `--trick INSTANCE`, a labelled commit, and a declared configuration.
Individual seeds can set best records. The independent RANDOM-05 solver remains
read-only; no edits, builds, held-out-stream access, or job changes there.

| Instance | Current selected full strict1s score | Next comparison |
|---|---:|---|
| WAREHOUSE |155173|Preserve validated8-core profile. |
| SORTATION |150333|Preserve2Mglobal/8-core profile; future dispatch or lane factors. |
| CITY-01 |8427|Four seed results near NMS8420; pickup16 seed2 selected, no large-margin claim. |
| CITY-02 |16315|Budget2 unopened retarget improved seed0; replicate and test scheduling interactions. |
| GAME |21742|Fleet2000/2250/2500/2750, plus paired control/2750 on seeds2/4, full5000steps. |
| RANDOM-01 |621|Test matching cadence separately before adding complexity. |
| RANDOM-02 |1188|Cadence1 budget1=1183, budget2/4/8=1188; most gain is cadence. Replicate. |
| RANDOM-03 |1902|KK forward field plus chain ranks, pickup4, global8M. Exact oriented chain potential remains structural candidate. |
| RANDOM-04 |1645|Map fields and repeated rematching lose. Test regional rounds/partitions; inspect rotation and multi-step coordination. |
| RANDOM-05 |2806|Combine region4rounds with budget2/4 rematching; test16Mregional or8rounds. |

General mechanisms with measured gains include construction-priority portfolios,
regional repair, oriented pickup matching, and finite retarget budgets. Their
benefits differ by map, so retain separate controls. The dense reference's exact
remaining-chain orientation DP and coherent multi-step motion pipeline remain
unimplemented structural candidates; current-goal or one-turn approximations
are not those algorithms.

Explicit tricks already tested include NMS arrows and squared ranks, KK forward
fields, the standalone-derived RANDOM guidance field, and GAME fleet selection.
Uniform2750 beats the KK-tabu2750 adaptation on seed0, but both beat the full
fleet control by about39%. This deliberately withholds tasks from some robots;
all robots remain in CGAR movement and no held task is dropped. GAME's1000-step
prefix predicts the wrong sign, so use the full5000-step horizon.

Reference code facts and hashes are in[the source study](trick-review-source-hashes.json)
and[the RANDOM transfer review](random-transfer/REVIEW.md). KK's GAME fleet limit
can be exceeded by exempt starting locations; CGAR preserves this behavior when
tabu is selected. The shuffle adaptation is documented and independently audited.
KK's variable stay/turn fields are not reproduced by copying only forward edges.
NMS/KK source-derived hypotheses count only after controlled CGAR measurements.

Known-horizon triage is allowed as a labelled trick. Current CGAR spatial bounds
require unrestricted full cores, which RANDOM/CITY/GAME do not have. Do not
bypass that restriction: use a valid full-free-space bound or explicitly declared
heuristic with independent tests. Current dense bests do not use horizon triage.

Higher regional budgets sometimes help but more global candidates and the narrow
after-turn promise often lose. Certificate disabling lost on both dense RANDOM
maps. Keep those negative findings instead of repeating the same hypotheses.
Full raw outputs, source/binary hashes, resource limits, task ownership and waiting
metrics remain recorded. Shared hosts are allowed, with exact physical bindings
and new SMT-aware queue reservations; timeout is always failure.
