# CGAR next experiments across ten instances

Updated 2026-09-21T09:08:18.069431+00:00. Targets remain +10% over max(published NMS,KK)
on RANDOM01-05 and a stretch +5% elsewhere. Throughput primary, fairness secondary.
Selected-seed maxima qualify. Preserve general mechanisms and explicit instance
tricks separately; all selected profiles currently require `--trick INSTANCE`.
Historical published targets are not matched local competitor runs.

| Instance | Selected full strict1s | Next work |
|---|---:|---|
|WAREHOUSE|155173|Preserve qualified eight-core profile; new5%target is a stretch.|
|SORTATION|150894|Retain150894; runtime alternative150865has895ms max.|
|CITY-01|8427|Transfer CITY02 pickup grouping and two allowed rematches in a2x2factor.|
|CITY-02|16315|Lower lane penalties all lost; investigate assignment or route quality.|
|GAME|24447|New margin goal met; lower lane penalties lose sharply.|
|RANDOM-01|668|Replicate uniform-window668; explore forecast initialization.|
|RANDOM-02|1215|Keep1215; turn40/wait20lose.|
|RANDOM-03|1939|Keep1939history-off; horizon10/15/25factors pending.|
|RANDOM-04|2023|Replicate2023common-future gain; longer/extra branches and chain composition.|
|RANDOM-05|3065|Keep3065selected seed10; common-future factors pending.|

The window layer is seeded by a valid CGAR plan and preserves real primary and
support actions. Atomic group repairs, exact task-stage costs, independent random
streams, serial/parallel identity and timeout failure are regression-tested.
R01/R02/R03 now benefit on selected seeds; R01/R03 improve all three matched seeds.
History-off helps R03. A remaining hypothesis is that truncating a retained prefix
into waits makes it incomparable with a fully projected fresh seed. A separate
optional tail refresh can test that; do not silently alter current frontiers.

The common-future selector is general and defaultoff. It compares completed CGAR
proposals under common future orders, keeps all protected paths, and finishes
all declared evaluations. It does not reproduce PILOT's two-phase motion pipeline.
R04's15action arm improves seed0while10action arms lose; replication is necessary.
The independent PILOT tree, jobs, builds and held-out streams remain untouched.

[Exact selected settings](selected-full-results.json), [history](BEST_HISTORY.md),
[current jobs](ACTIVE_JOBS.json), [field negatives](field-options/first-results.json),
[window replication](rolling-window/random03-three-seed-summary.json),
[common-future design](common-futures/README.md).
