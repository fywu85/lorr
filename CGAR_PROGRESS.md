# CGAR competition progress

Updated 2026-09-20. The active goal is to carry the CGAR framework across all ten
2024 instances after finishing Warehouse's strict one-second validation.
NMS remains the primary reference. Matching or exceeding throughput is the first
milestone; +20%per instance is a stretch goal. Fairness is secondary under the
latest user clarification: track and disclose waiting regressions without making
them a veto on a throughput improvement.

[Campaign and protocol](experiments/allmaps-20260920/README.md).
[Published NMS targets for all ten instances](experiments/allmaps-20260920/TARGETS.md).
[Warehouse best history](WAREHOUSE_PROGRESS.md).
[RANDOM-05 independent campaign, read-only](RANDOM05_PROGRESS.md).

| Instance | Current selected full CGAR result | Status |
|---|---:|---|
| WAREHOUSE |155173|TRICK; full strict1s seeds0/2 confirmed, max945/958ms|
| SORTATION |146460|Generic; full shared5s development, generic_regions_4m|
| CITY-01 |7305|Generic; full shared5s development, generic_regions_4m|
| CITY-02 |14068|Generic; full shared5s development, generic_regions_4m|
| GAME |6519|Generic; full shared5s development, generic_regions_4m|
| RANDOM-01 |611|Generic; full shared5s development, generic_match64_direct|
| RANDOM-02 |1084|Generic; full shared5s development, generic_match64_direct|
| RANDOM-03 |1484|Generic; full shared5s development, generic_match64_direct|
| RANDOM-04 |1330|Generic; full shared5s development, generic_chain_rank_8workers|
| RANDOM-05 |2036|Generic; full shared5s development, generic_noise50_cold|

The initial nine-instance transfer used sourceabbe36b and one planner seed, before later
Warehouse matching/trick work; it is a starting measurement, not each instance's
best possible configuration. Full evidence and source hashes are linked in the
[campaign inventory](experiments/allmaps-20260920/baseline-inventory.json).
Do not compare its counts directly to capped reference runs or label every row
competition-qualified. Future record rows will include exact source links and
run-completion timestamps as in the Warehouse log.

Warehouse strict confirmation completed2026-09-20T23:23:25.627892+00:00,
source[3e319f1](https://github.com/fywu85/lorr/commit/3e319f175859396b91f97364b320c6794a85cbb7),
[verified10000entries and100millionactions](experiments/construction-20260918/results/strict-runtime-full-v123/summary.md).
The original155173record timestamp/source remain in the Warehouse history.
The two-budget/eight-instance matrix and the six-factor RANDOM-01..04 matrix
are complete: all40full runs verified, no failed runs. The selected current
results above come from sourceeb99380. [Timestamped history and configurations](experiments/allmaps-20260920/BEST_HISTORY.md).
RANDOM-04 remaining-chain ordering and rank weights reach1249versus842under the
same bounded control, one seed. Direct-cost assignment with local matching gives
611/1084/1484on RANDOM-01/02/03. Fairness remains recorded as secondary.

The user requested a deeper read of the separate RANDOM-05 algorithm for transfer
into CGAR on RANDOM-04/05. [Frozen source study and next experiments](experiments/allmaps-20260920/random-transfer/REVIEW.md).
The first optional persistent-priority implementation passed the full regression
suite and its ten full dense-map runs. The eight-worker control scores1330/1989;
noise50without retention scores1306/2036. Retention itself was weaker. Selected
maxima remain one-seed results; strict1sseed0/2/4replication is pending. It preserves CGAR's existing kernel and
protections. SORTATION's explicit native field passed all regressions and its
full three-way comparison is running; no new trick score yet.

The first full CGAR RANDOM-05 comparison completed with1534(control) and
1847(remaining-chain rank); mean123/124ms, maximum183/178ms,191MBRAM.
These were enforced5sdevelopment runs, not strict1squalification.
[Full evidence](experiments/allmaps-20260920/results/random05-baseline-full-v1/summary.md).
Rows above use different exploratory configurations; they are not one universal
preset or an automatic per-map dispatcher.

The original one-worker full controls reproduce both earlier whole trajectory
fingerprints across sourceeb99380->5d3c5bf, with new features disabled.
[Equivalence](experiments/allmaps-20260920/random-transfer/disabled-full-equivalence.json).
Both existing scoring alternatives lost in the six full dense-map checks;
[all outcomes](experiments/allmaps-20260920/results/dense-objectives-full-v1/summary.md).
An analysis allocation preflight failed before reading results; its unchanged
retry succeeded. This was not a solver failure.
