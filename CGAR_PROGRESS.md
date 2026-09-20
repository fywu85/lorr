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

| Instance | Latest full CGAR evidence | Status |
|---|---:|---|
| WAREHOUSE |155173|TRICK; full strict1s seeds0/2 confirmed, max945/958ms|
| SORTATION |146147|Generic; shared5s development|
| CITY-01 |7305|Generic; shared5s development|
| CITY-02 |13922|Generic; shared5s development|
| GAME |No accepted score|Generic transfer timeout|
| RANDOM-01 |588|Generic; shared5s development|
| RANDOM-02 |982|Generic; shared5s development|
| RANDOM-03 |1409|Generic; shared5s development|
| RANDOM-04 |997|Generic; shared5s development|
| RANDOM-05 |No accepted CGAR transfer score|Separate agent owns standalone work|

The nine-instance transfer uses sourceabbe36b and one planner seed, before later
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
The two-budget/eight-instance generic matrix8900861 and its verifier8900862
are still running; no cross-instance frontier is promoted before analysis.
