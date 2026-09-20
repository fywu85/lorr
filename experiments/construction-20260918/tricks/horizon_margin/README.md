# TRICK: prospective completion margin

Requires `--trick WAREHOUSE`, positive `CGAR_TRICK_KNOWN_HORIZON`, and independent
`CGAR_TRICK_HORIZON_MARGIN=1` (default0). The configured horizon remains an explicit
assumption. This experiment retains CGAR motion, original fairness, task ownership
protection, and matching. It does not enable the global short-task preference.

The V86 full minimal-horizon result is153614/153220. Its late distance-feasible
but unfinished first admissions number2774/2823. A saved, prospective exact-BFS
mean-margin audit identifies2528/2589 with insufficient mean slack, and1946/1982
chronological task-disjoint safer-alternative witnesses. They exclude unrevealed,
previously assigned, and same-batch assigned alternatives. This is enough to test;
it is not a predicted gain, counterfactual rollout, or upper bound. Exact offline
BFS can differ from production's resident-table/Manhattan bound. See
results/horizon-margin-opportunity-v90.

At each entry start, update five cumulative buckets of observed duration minus
initial physical bound: bound<50,<100,<200,<400,and>=400. Only completed tasks whose
first accepted holder never changed train them. Dropped/readmitted tasks stay
excluded. A task admitted at decision tick a and observed absent at the next
consecutive schedule tick n contributes duration n-a, after checking its holder
is free at the final cell. Incomplete observations do not invent completion times.
The ledger records the final post-matching proposal; unexecuted intermediate
assignments are not holder changes. TaskManager's removal-on-completion contract
is the source of the observation. No simulator metadata is changed.

One immutable bucket snapshot covers the entire scheduler entry. New admission
pairs have three tiers: bound+mean_margin<=remaining; bound<=remaining; physically
impossible. Equality is inclusive, means use integer cross-products, and a bucket
without observations has zero margin. Preserve original HRRN/cost/id/robot order
within each tier, endpoint-bucket order and forced-oldest admission. The margin is
a heuristic, never a lower bound or completion guarantee. All-impossible sets still
receive assignments; held and started tasks are not dropped. At/pastH ordering is
ordinary. All work completes or the entry raises timeout.

Pool-exchange mode is rejected for now, because it may introduce fresh assignments
through a metadata-free early return. Current best already fixes it OFF. Existing
horizon full-core/original-chain/cost-key guards also apply. Normal matching and
pair swaps remain supported; every later holder change invalidates calibration.
The calibration bound uses the existing chain cache and const spatial peek at the
final holder, with repeated-service ticks, and never builds or promotes a table.

Validation plan: full regression suite including independent ledger timing,
future/retarget/drop exclusion, fractional/equal boundary, immutable snapshots,
final-proposal holder and real scheduler/fair admission fixtures. Startup compares
H200/H5000 x marginOFF/ON; OFF cases must exactly reproduce V86, H5000 ON must also
remain exact in the first200steps. Separately reproduce the generic800step hash.
Then four full cases, seeds0/2 x marginOFF/ON atH5000, four distinct bound physical
cores each,32decimalGB/shared5s. Controls must reproduce153614/153220 and their full
hashes. Action/schedule/event prefixes before the first comparison changed by the
margin must match. Report all full outcomes, tails, ages, unfinished work and runtime.
No throughput claim is made before these checks.

V91source8546afa and V92source92bc748 both pass the complete regression suite.
V92 adds an explicit failure for a physically impossible calibration observation
and the actual fresh-task matching integration test requested by Fable43. Exact
source/binary records are in build-provenance/v91 and v92. No new ranking factor
is added by this hardening. V92 startup8899761/analysis8899764 passed at13:16:35UTC,
including the exact88-step pre-margin prefix in the activeH200 case. The generic800
control8899762/analysis8899765 passed at13:20:45UTC with the exact18958-task trajectory.
Full comparison8899779/analysis8899780 started13:21:24UTC on16 bound physical cores
of shared research38. It uses frozen V92 source/binary; no full margin result yet.
