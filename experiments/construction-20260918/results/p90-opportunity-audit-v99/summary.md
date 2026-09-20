# Remaining completion opportunities after the p90 change

Read-only replay of the verified 155,057 / 154,727 runs completed 2026-09-20 16:34:19 UTC. Both 50-million-step accounts agree with their frozen references; independent spatial-feasibility and prospective-training fixtures pass.

| First admissions from step 4,000 | Seed 0 | Seed 2 |
|---|---:|---:|
| Completed by 5,000 | 24,037 | 23,969 |
| Unfinished, physically impossible for the initial holder | 9,179 | 9,179 |
| Unfinished, physically feasible for the initial holder | 791 | 788 |
| Distinct feasible-alternative witnesses for impossible admissions | 9 | 4 |

The preceding mean-margin policy left 1,053 / 1,065 physically feasible unfinished admissions in this cohort. P90 leaves fewer, while more initially impossible tasks are admitted near the end. Ordinary fairness still admits work even when the visible pool has no suitable finishable alternative. An impossible initial pairing is not necessarily an impossible task for every robot; this audit conditions on the observed initial holder and assignment time.

Only 9 / 4 task-disjoint physical alternative witnesses remain under this conservative audit. This does not establish a throughput ceiling: it holds the trajectory fixed, excludes same-batch assignments, and does not solve a joint counterfactual allocation. The exact spatial bounds can also differ from production cache/mixed estimates.

The output's `prospective_margin_audit` deliberately retains the earlier **mean-margin diagnostic**: it reports 559 / 575 exposed mean-unsafe pairs and 25 / 33 mean-safer alternatives. These are offline comparisons on a p90 trajectory, not classifications from the active p90 scheduler, and must not be interpreted as violations of its comparator.

The full p90 work audit finds 9,340,992 / 9,342,813 empty robot steps, already below the 9,612,055 in the saved NMS diagnostic. NMS's much shorter last-holder pickup distance is affected by repeated rematching; it does not by itself imply lower aggregate empty work. That NMS run had one timeout and exceeded 32 GB, so it is a mechanism diagnostic, not a new qualifying competitor score.

This audit justifies checking the pending percentile refinements and pickup interaction, but gives no evidence for promising a large gain from another simple late-task filter. No production change or new throughput score comes from this replay.

[Accounting and limitations](accounting.json), [frozen references](reference.json), [fixtures](fixtures.json), [source hashes](request.json), [NMS diagnostic](../nms-task-pool-audit/nms_diagnostic.json).
