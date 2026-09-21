# CGAR competition progress

Updated 2026-09-21 UTC. The active goal is to carry the CGAR framework across all ten
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
| SORTATION |150353|TRICK; full shared5s development, max1152ms; strict1s work remains|
| CITY-01 |7305|Generic; full shared5s development, generic_regions_4m|
| CITY-02 |14068|Generic; full shared5s development, generic_regions_4m|
| GAME |6519|Generic; full shared5s development, generic_regions_4m|
| RANDOM-01 |611|Generic; full shared5s development, generic_match64_direct|
| RANDOM-02 |1084|Generic; full shared5s development, generic_match64_direct|
| RANDOM-03 |1484|Generic; full shared5s development, generic_match64_direct|
| RANDOM-04 |1367|Generic; full strict1s, noise50 seed4; eight-worker control has better three-seed mean|
| RANDOM-05 |2457|TRICK; full strict1s, tuned integer field, seed0; max178ms|

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
are complete: all40full runs verified, no failed runs. Those original regional/factor
results come from sourceeb99380; later selected records have their own source links. [Timestamped history and configurations](experiments/allmaps-20260920/BEST_HISTORY.md).
RANDOM-04 remaining-chain ordering and rank weights reach1249versus842under the
same bounded control, one seed. Direct-cost assignment with local matching gives
611/1084/1484on RANDOM-01/02/03. Fairness remains recorded as secondary.

The user requested a deeper read of the separate RANDOM-05 algorithm for transfer
into CGAR on RANDOM-04/05. [Frozen source study and next experiments](experiments/allmaps-20260920/random-transfer/REVIEW.md).
The first optional persistent-priority implementation passed the full regression
suite and its ten full dense-map runs. The eight-worker control scores1330/1989;
noise50without retention scores1306/2036. Retention itself was weaker. Selected
maxima remain individual-seed records. All18strict1sseed0/2/4runs passed, with
seed0 whole trajectories unchanged and maximum307.61ms. It preserves CGAR's existing kernel and
protections. SORTATION's full explicit-trick comparison is complete: control146460,
native lanes149321, native lanes with outer-band costs150353. The control's whole
trajectory is unchanged. The selected trick is still above1s on its slowest steps.
[Full SORTATION evidence](experiments/allmaps-20260920/results/sortation-native-full-v1/summary.md).

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

The standalone RANDOM field has been exported reproducibly and installed behind
explicit `--trick RANDOM-04` / `--trick RANDOM-05` gates, including exact occupancy
and team-size checks. An integer adaptation and uniform-cost control are declared;
the first full strict1s comparisons now pass: RANDOM-04 control1306/uniform1205/field1353;
RANDOM-05 control2036/uniform1941/field2457 (+20.68%over its generic control).
The field profiles max at136/178ms. Further planner seeds are next.

Strict dense-map means: RANDOM-04 one-worker1235.67, eight-worker1307.00,
noise50=1301.33; RANDOM-05 one-worker1829.67, eight-worker1960.33, noise50=2032.67.
[Full replication](experiments/allmaps-20260920/results/priority-portfolio-strict-seeds-v2/summary.md).

The RANDOM-05 capacity policy excludes any task with a stop outside the core.
A read-only audit attributes427of431never-assigned tasks in the2036control to
this rule. This is a structural limitation, not a measured counterfactual gain.
[Certificate and event audit](experiments/allmaps-20260920/random-transfer/capacity-audit-v1/audit.json).
