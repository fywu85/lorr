# WAREHOUSE throughput progress

Updated: 2026-09-20 13:32 UTC.

**Best overall validated single run: 153,614 tasks (TRICK, seed 0)**, using native Warehouse bands, raw remaining-potential scoring, matching64, pickupweight5 and the explicit configured horizon H=5000, enabled by `--trick WAREHOUSE`. Ordinary fairness remains enabled; global short preference is OFF. This leaves **1,181 tasks** to the NMS target of **154,795** (**0.77% growth** needed). Any valid full seed may achieve the target; promotion of the generic benchmark reference remains separate.

**Best generic single run: 145,083**, from remaining-flow scoring; that candidate stays OFF after its mixed six-seed result. **Confirmed six-seed generic reference: 144,392.17**, using graded-flow pickup, two regional repair rounds and bounded matching64. All six full totals improve over the prior 143,941.17 reference. [Six-seed matching confirmation](experiments/construction-20260918/results/match-quota-confirm-v64/summary.md).

| Best overall record completed (UTC) | Full tasks | Seed / policy | Source and evidence |
|---|---:|---|---|
|2026-09-20 03:21:38.087713|**146,659**|0 / **TRICK** static NMS warehouse lanes, `--trick WAREHOUSE`|[abbe36b](https://github.com/fywu85/lorr/commit/abbe36b723bb43de2478c07bcb88621d5238a23a); [full verification](experiments/construction-20260918/results/trick-nms-warehouse-full-v54/verification.json).5,000steps,10,000robots, zero failures, max984.533ms, RSS11.914GB. Reproduced exactly in the [later factorial](experiments/construction-20260918/results/trick-short-tasks-full-v58-r2/factorial-verification.json).|
|2026-09-20 08:38:20.944797|**147,204**|2 / **TRICK** static lanes + matching64, `--trick WAREHOUSE`|[5a8a51a](https://github.com/fywu85/lorr/commit/5a8a51a38a8355811d49595f777572a819f9d10e); [full verification](experiments/construction-20260918/results/trick-match-full-v65/verification.json), [configuration/hash](experiments/construction-20260918/results/trick-match-full-v65/comparison.json).5,000steps,10,000robots, zero failures; shared-host5s development, max1010.043ms, RSS11.939GB.|
|2026-09-20 08:38:32.494327|**147,328**|0 / **TRICK** static lanes + matching64, `--trick WAREHOUSE`|[5a8a51a](https://github.com/fywu85/lorr/commit/5a8a51a38a8355811d49595f777572a819f9d10e); [full verification](experiments/construction-20260918/results/trick-match-full-v65/verification.json), [configuration/hash](experiments/construction-20260918/results/trick-match-full-v65/comparison.json).5,000steps,10,000robots, zero failures; shared-host5s development, max1011.861ms, RSS11.928GB.|
|2026-09-20 09:06:50.927809|**147,422**|0 / **TRICK** static lanes + remaining potential, `--trick WAREHOUSE`|[0f4183f](https://github.com/fywu85/lorr/commit/0f4183fcdafcaab391ace632f70a0c513caba14b); [full verification](experiments/construction-20260918/results/trick-potential-full-v68/verification.json), [configuration/hash](experiments/construction-20260918/results/trick-potential-full-v68/best-record.json). 5,000 steps, 10,000 robots, zero failures; shared-host 5s development, max 981.850ms, RSS 11.912GB.|
|2026-09-20 09:07:01.739904|**147,502**|2 / **TRICK** static lanes + remaining potential, `--trick WAREHOUSE`|[0f4183f](https://github.com/fywu85/lorr/commit/0f4183fcdafcaab391ace632f70a0c513caba14b); [full verification](experiments/construction-20260918/results/trick-potential-full-v68/verification.json), [configuration/hash](experiments/construction-20260918/results/trick-potential-full-v68/best-record.json). 5,000 steps, 10,000 robots, zero failures; shared-host 5s development, max 979.072ms, RSS 11.839GB.|
|2026-09-20 09:45:34.314669|**148,101**|2 / **TRICK** static lanes + remaining potential + matching64, `--trick WAREHOUSE`|[c696d5f](https://github.com/fywu85/lorr/commit/c696d5f9d5b9b5bc6557830e6e43680913b3e7b7); [full verification](experiments/construction-20260918/results/trick-score-match-full-v72/verification.json), [configuration/hash](experiments/construction-20260918/results/trick-score-match-full-v72/comparison.json). 5,000 steps, 10,000 robots, zero failures; shared-host 5s development, max 991.422ms, RSS 11.837GB.|
|2026-09-20 09:45:49.302966|**148,132**|0 / **TRICK** static lanes + remaining potential + matching64, `--trick WAREHOUSE`|[c696d5f](https://github.com/fywu85/lorr/commit/c696d5f9d5b9b5bc6557830e6e43680913b3e7b7); [full verification](experiments/construction-20260918/results/trick-score-match-full-v72/verification.json), [configuration/hash](experiments/construction-20260918/results/trick-score-match-full-v72/comparison.json). 5,000 steps, 10,000 robots, zero failures; shared-host 5s development, max 976.717ms, RSS 11.939GB.|
|2026-09-20 10:17:07.927721|**148,440**|2 / **TRICK** static lanes + remaining potential + matching64 + pickupweight5, `--trick WAREHOUSE`|[c696d5f](https://github.com/fywu85/lorr/commit/c696d5f9d5b9b5bc6557830e6e43680913b3e7b7); [full verification](experiments/construction-20260918/results/combined-pickup-full-v74/verification.json), [configuration/hash](experiments/construction-20260918/results/combined-pickup-full-v74/best-record.json). 5,000 steps, 10,000 robots, zero failures; shared-host 5s development, max1014.990ms, RSS11.836GB.|
|2026-09-20 10:16:33.754893|**149,871**|2 / **TRICK** native field without bands, raw remaining potential, matching/short OFF, `--trick WAREHOUSE`|[06a8258](https://github.com/fywu85/lorr/commit/06a825855718513c3e965a22aac227ad13fd5595); [full verification](experiments/construction-20260918/results/native-metric-full-v73/verification.json), [configuration/hash](experiments/construction-20260918/results/native-metric-full-v73/comparison.json). 5,000 steps, 10,000 robots, zero failures; shared-host5s development, max1004.828ms, RSS11.928GB.|
|2026-09-20 10:16:35.538762|**149,915**|0 / **TRICK** native field without bands, raw remaining potential, matching/short OFF, `--trick WAREHOUSE`|[06a8258](https://github.com/fywu85/lorr/commit/06a825855718513c3e965a22aac227ad13fd5595); [full verification](experiments/construction-20260918/results/native-metric-full-v73/verification.json), [configuration/hash](experiments/construction-20260918/results/native-metric-full-v73/comparison.json). 5,000 steps, 10,000 robots, zero failures; shared-host5s development, max1004.124ms, RSS11.933GB.|
|2026-09-20 10:24:54.744615|**151,443**|0 / **TRICK** native field with bands, raw remaining potential, matching/short OFF, `--trick WAREHOUSE`|[06a8258](https://github.com/fywu85/lorr/commit/06a825855718513c3e965a22aac227ad13fd5595); [full verification](experiments/construction-20260918/results/native-metric-full-v73/verification.json), [configuration/hash](experiments/construction-20260918/results/native-metric-full-v73/comparison.json). 5,000 steps, 10,000 robots, zero failures; shared-host5s development, max1031.268ms, RSS11.918GB.|
|2026-09-20 10:25:07.422058|**151,630**|2 / **TRICK** native field with bands, raw remaining potential, matching/short OFF, `--trick WAREHOUSE`|[06a8258](https://github.com/fywu85/lorr/commit/06a825855718513c3e965a22aac227ad13fd5595); [full verification](experiments/construction-20260918/results/native-metric-full-v73/verification.json), [configuration/hash](experiments/construction-20260918/results/native-metric-full-v73/comparison.json). 5,000 steps, 10,000 robots, zero failures; shared-host5s development, max1016.591ms, RSS11.937GB.|
|2026-09-20 11:10:19.993240|**152,048**|2 / **TRICK** native bands + remaining potential + matching64 + pickupweight5, short OFF, `--trick WAREHOUSE`|[c6d9755](https://github.com/fywu85/lorr/commit/c6d97550989e64a899d83b799b9e8ad5a528b960); [full verification](experiments/construction-20260918/results/native-assignment-full-v78/verification.json), [configuration/hash](experiments/construction-20260918/results/native-assignment-full-v78/comparison.json). 5,000steps,10,000robots,zero failures; shared5s development,max1045.427ms,RSS11.933GB.|
|2026-09-20 11:10:20.336325|**152,066**|2 / **TRICK** native bands + remaining potential + matching64 + pickupweight1, short OFF, `--trick WAREHOUSE`|[c6d9755](https://github.com/fywu85/lorr/commit/c6d97550989e64a899d83b799b9e8ad5a528b960); [full verification](experiments/construction-20260918/results/native-assignment-full-v78/verification.json), [configuration/hash](experiments/construction-20260918/results/native-assignment-full-v78/comparison.json). 5,000steps,10,000robots,zero failures; shared5s development,max1040.791ms,RSS11.933GB.|
|2026-09-20 11:10:21.393890|**152,383**|0 / **TRICK** native bands + remaining potential + matching64 + pickupweight5, short OFF, `--trick WAREHOUSE`|[c6d9755](https://github.com/fywu85/lorr/commit/c6d97550989e64a899d83b799b9e8ad5a528b960); [full verification](experiments/construction-20260918/results/native-assignment-full-v78/verification.json), [configuration/hash](experiments/construction-20260918/results/native-assignment-full-v78/comparison.json). 5,000steps,10,000robots,zero failures; shared5s development,max1054.859ms,RSS11.880GB.|
|2026-09-20 12:43:02.857282|**153,614**|0 / **TRICK** native bands + remaining potential + matching64 + pickupweight5 + configured H=5000, ordinary fairness, `--trick WAREHOUSE`|[4e1a8fe](https://github.com/fywu85/lorr/commit/4e1a8fe5e995e275faa6ddfaa0e8f8ae61fc5352); [full verification](experiments/construction-20260918/results/known-horizon-full-v86/verification.json), [configuration/hash](experiments/construction-20260918/results/known-horizon-full-v86/best-record.json). 5,000steps,10,000robots,zero failures; shared5s development,max989.051ms,RSS11.912GB. Horizon knowledge is an explicit configured assumption.|

Records above follow validation order. Parallel comparisons can finish their checks later even when their run-completion timestamp is earlier; 148,440 was an interim verified best before the full native comparison was checked.

The [full native assignment comparison](experiments/construction-20260918/results/native-assignment-full-v78/summary.md) raises the best to152,383. Matching improves both seeds under each native field. Pickupweight5 adds482tasks on band seed0 but loses18 on seed2 versus matching/weight1; all tested matched profiles beat their unmatched controls. All12full runs and four exact controls pass. Best mean450.823ms/max1.054859s/RSS11.880GB, shared5s development. The [eight full short-preference cases](experiments/construction-20260918/results/native-short-full-v80/summary.md) are now verified: short preference loses3645/3669 atweight1 and513/244 atweight5, so it stays OFF.

The [saved-run horizon audit](experiments/construction-20260918/results/horizon-work-audit-v84/summary.md) finds about4% of robot work on tasks still unfinished at the end, consistently across both best-profile seeds. The [corrected lower-bound audit](experiments/construction-20260918/results/horizon-bound-audit-v86-r2/summary.md) finds4024/3980 distinct feasible-alternative witnesses, sufficient to justify a test but not a predicted gain. A later retarget can rescue an initially impossible pair. The [full configured-horizon experiment](experiments/construction-20260918/results/known-horizon-full-v86/summary.md) is now verified: **153614/153220**, gains **1231/1172**, all in the last1000steps, with exact controls and pre-intervention prefixes. It preserves ordinary fairness and requires explicit --trick WAREHOUSE with disclosed horizon knowledge. The [saved ON work audit](experiments/construction-20260918/results/known-horizon-work-audit-v89/summary.md) finds unfinished work down365304/343086 robot-steps. A [prospective mean-margin audit](experiments/construction-20260918/results/horizon-margin-opportunity-v90/summary.md) identifies1946/1982 distinct safer-alternative witnesses, sufficient to justify a test but not predicted gains. The optional margin implementation [92bc748](https://github.com/fywu85/lorr/commit/92bc748e81416de9d66edaec0e83a833ae3bac18) passes the full suite, including real fresh-task matching and calibration-ledger checks. Its [startup controls](experiments/construction-20260918/results/horizon-margin-screen-v92/comparison.json) and [generic 800-step control](experiments/construction-20260918/results/horizon-margin-generic-v92/comparison.json) passed exactly. The full margin comparison is running on16 bound physical cores; no margin throughput result yet. The [full read-only matching-budget audit](experiments/construction-20260918/results/match-budget-subsets-full-v84/summary.md) now reproduces all four control trajectories and real-work counters exactly. With ordinary fairness, the sampled extra opportunity is small (0.0146/0.0149 guidance forward units per assignment); it is not an upper bound or a throughput estimate. A second retarget remains disabled.

The earlier increasing-record table below remains the **generic** history. A future valid single-seed best updates the appropriate history immediately, with its source, timestamp and flags; it does not imply repeatability across seeds.

The preceding regional-only reference totals were **144,510 /143,933 /144,107 /143,134 /143,934 /144,029**. Every full total improves over its matched control. Five final1,000-step windows and five outstanding-age p90 values improve; seed1 loses108 final-window tasks and agep90 rises1 step. Those secondary regressions remain documented. All60,000 candidate/control entries meet one second, all process RSS values stay below32decimalGB, and every control exactly reproduces its preceding full trajectory. [Six-seed evidence](experiments/construction-20260918/results/pickup-full-regions-six-seed-v44.json).

The active target is **154,795 completed tasks**, as corrected by the user on2026-09-20. The current six-seed mean needs **7.20% more throughput** to reach it. The earlier local KittyKnight measurement remains152,981 and used38.858GB RSS; our limit remains32,000,000,000bytes per planner. The new target is a user-specified objective, not a newly measured competitor result. The NMS target remains unmet. A valid full generic or explicitly enabled TRICK seed may satisfy it under the user's revised criterion; six-seed generic reference promotion remains separate. Development runs now allow shared hosts and a5-second timeout at the user's request. Fixed search work and the32GB limit remain; these results must be labelled separately from competition-budget confirmation. The best solver will receive a later one-second check.

The previously highest six-seed mean,143,111.2 from sixteen global workers, had three total regressions and was not promoted. Regional repair now exceeds that mean and improves all six paired full totals. The larger pickup quotas128/256 produce exactly the same full trajectories as64 on both tested seeds; they provide no observed quality gain. [Regional configuration and checks](experiments/construction-20260918/PICKUP_FULL_REGIONS.md) · [Quota comparison](experiments/construction-20260918/results/pickup-full-quota-full-v46.json).

The [new diagnosis and literature review](experiments/construction-20260918/bottleneck_review/REPORT.md)
replays all six current trajectories and accounts for every robot step. Loaded
turns/waits/detours contribute12.302 extra steps per completed task versus the
local KK run; the completed shortest-chain mix differs by8.025 steps. These are
observational differences, not guaranteed savings. Fable's same-session review
prioritizes competitor guidance/rematching ablations, then a controlled assignment
interaction and a motion change chosen from fresh blocker evidence. **No new
throughput record** is claimed.

The [KK guidance/rematching diagnosis](experiments/construction-20260918/kk_ablation/RESULTS.md)
is complete: supplied weights/rematching152,872 and152,786; supplied/keep142,510;
uniform/rematching24,574; uniform/keep23,620. All five have one timeout and exceed
our memory cap, so these are diagnostics. Full action and pool accounting pass;
weight removal changes the whole KK policy, not just motion. The large collapse
makes guidance quality a priority. Initial configuration failures are preserved. The
[current blocker audit](experiments/construction-20260918/bottleneck_review/CURRENT_BLOCKERS.md)
now reproduces both full CGAR reference trajectories exactly, with every decision
under one second. The [task-pool replay](experiments/construction-20260918/bottleneck_review/TASK_POOL.md)
also identifies a substantial waiting-time tradeoff in KK's shorter completed-task
mix. These diagnostics establish no new throughput gain.

The [NMS follow-up](experiments/construction-20260918/bottleneck_review/NMS_REVIEW.md)
adds active-source checks and complete action/task-pool accounting. Its lower
loaded overhead accompanies map-specific guidance and long-task deferral;
this is diagnostic evidence, not a new CGAR score. The
[read-only group snapshots](experiments/construction-20260918/bottleneck_review/GROUP_SNAPSHOTS.md)
pass regressions and both full reference trajectory/deadline checks. The complete
small-group probe finds44/2,368sampled groups improving both scalar and aggregate
guidance potential; this is diagnostic evidence, not a new throughput score.

**Track policy:** generic and explicitly labelled **TRICK** records are tracked separately. Map-specific
tricks require `--trick <map-instance-name>`, `[trick]` commit titles and **TRICK**
log entries. No environment variable or automatic map detection enables them.
[Policy](experiments/construction-20260918/EXPERIMENT_TRACKS.md).

**TRICK full results, eligible for the overall best history:** NMS-derived directions at CGAR4/16forward
costs require `--trick WAREHOUSE`. Seeds0/2 reach146,659/146,566, versus exact
no-flag144,510/144,107. All20,000entries across the four runs pass1s; peakRSS11.914GB.
The final1000differences are−93/+50, so a sustained-rate gain is not established.
Only two seeds are tested; static guidance, cache lifecycle and pickup quotes
change together. V53's earlier explicit startup failures remain archived.
[Implementation](experiments/construction-20260918/tricks/nms_warehouse/README.md),
[full comparison](experiments/construction-20260918/results/trick-nms-warehouse-full-v54/comparison.json).

| TRICK completed (UTC) | New TRICK best | Seed | Exact source | Evidence |
|---|---:|---:|---|---|
| 2026-09-20 03:21:17 |146,566|2|[abbe36b](https://github.com/fywu85/lorr/commit/abbe36b723bb43de2478c07bcb88621d5238a23a)|[verification](experiments/construction-20260918/results/trick-nms-warehouse-full-v54/verification.json)|
| 2026-09-20 03:21:38 |146,659|0|[abbe36b](https://github.com/fywu85/lorr/commit/abbe36b723bb43de2478c07bcb88621d5238a23a)|[verification](experiments/construction-20260918/results/trick-nms-warehouse-full-v54/trick-verification.json)|

These separate TRICK records do not alter the generic record table or satisfy
the generic six-seed goal. The full no-flag trajectories match the prior reference
byte for byte, independently confirming that the flag isolates the new behavior.
[Generic isolation](experiments/construction-20260918/results/generic-warehouse-full-v54/trick-verification.json).

The [generic traffic replay](experiments/construction-20260918/bottleneck_review/FLOW_COHERENCE.md)
validates400million actions and60published fields. No short corridor has mixed
toll signs; simple corridor pooling is dropped as the next policy candidate.
This is diagnostic progress, not a new generic score.

**TRICK experiment started (2026-09-20 05:21 UTC), no new record:** [f939b3b](https://github.com/fywu85/lorr/commit/f939b3b1ab44368e83617a9f97300f26c4063210) adds independently selected short-task preference and lanes, requiring `--trick WAREHOUSE`. The short-task component removes the age bonus and forced-oldest admission; started work stays protected. All four startup runs pass and reproduce both existing control trajectories. Eight full runs (seeds0/2 × four arms) now use32 verified physical cores on a shared host, job8899316. The first full launch was cancelled for unapplied GRID binding; it has no accepted score. [Design and evidence](experiments/construction-20260918/tricks/short_tasks/README.md). Fable reviewed the patch in the same session. The full comparison includes waiting ages and empty travel; it estimates a CGAR policy effect rather than a fraction of the competitor lead.

## New best scores over time

This log backfills every increasing single-run record from the full warehouse campaign. Each run uses 10,000 robots and all 5,000 steps, reports no planner/scheduler errors or timeouts, and stays below the memory limit. Timestamps are **actual run completion in UTC**, not commit dates. Partial runs and short screens never enter this table.

| Completed (UTC) | Best tasks | Profile / seed | Linked commit | Evidence |
|---|---:|---|---|---|
| 2026-09-18 18:41:50 | 37,511 | CGAR baseline / 1 | [0643ca7](https://github.com/fywu85/lorr/commit/0643ca7aad8515986e38a75cad18b244bb37bc97) † | [data](experiments/sequences-20260918/results/warehouse-full-v2/run-summaries.json) · [source](experiments/sequences-20260918/build-provenance/v2/) · legacy timing |
| 2026-09-18 18:42:10 | 40,764 | Turn-aware guidance / 2 | [0643ca7](https://github.com/fywu85/lorr/commit/0643ca7aad8515986e38a75cad18b244bb37bc97) † | [data](experiments/sequences-20260918/results/warehouse-full-v2/run-summaries.json) · [source](experiments/sequences-20260918/build-provenance/v2/) · legacy timing |
| 2026-09-18 18:42:11 | 40,902 | Turn-aware guidance / 0 | [0643ca7](https://github.com/fywu85/lorr/commit/0643ca7aad8515986e38a75cad18b244bb37bc97) † | [data](experiments/sequences-20260918/results/warehouse-full-v2/run-summaries.json) · [source](experiments/sequences-20260918/build-provenance/v2/) · legacy timing |
| 2026-09-18 18:42:12 | 40,984 | Turn-aware guidance / 1 | [0643ca7](https://github.com/fywu85/lorr/commit/0643ca7aad8515986e38a75cad18b244bb37bc97) † | [data](experiments/sequences-20260918/results/warehouse-full-v2/run-summaries.json) · [source](experiments/sequences-20260918/build-provenance/v2/) · legacy timing |
| 2026-09-18 18:52:35 | 42,600 | 8 GiB orientation cache / 2 | [0643ca7](https://github.com/fywu85/lorr/commit/0643ca7aad8515986e38a75cad18b244bb37bc97) † | [data](experiments/sequences-20260918/results/warehouse-cache-full-v3/run-summaries.json) · [source](experiments/sequences-20260918/build-provenance/v3/) · legacy timing |
| 2026-09-18 18:52:36 | 43,446 | 8 GiB orientation cache / 0 | [0643ca7](https://github.com/fywu85/lorr/commit/0643ca7aad8515986e38a75cad18b244bb37bc97) † | [data](experiments/sequences-20260918/results/warehouse-cache-full-v3/run-summaries.json) · [source](experiments/sequences-20260918/build-provenance/v3/) · legacy timing |
| 2026-09-18 18:52:38 | 44,174 | 8 GiB orientation cache / 1 | [0643ca7](https://github.com/fywu85/lorr/commit/0643ca7aad8515986e38a75cad18b244bb37bc97) † | [data](experiments/sequences-20260918/results/warehouse-cache-full-v3/run-summaries.json) · [source](experiments/sequences-20260918/build-provenance/v3/) · legacy timing |
| 2026-09-19 01:13:53 | 51,506 | Temporal construction, ticket order / 0 | [4766c63](https://github.com/fywu85/lorr/commit/4766c633056ffb851447656eeee0e71f4e157ebd) † | [data](experiments/construction-20260918/results/full-v1/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v1/) · legacy timing |
| 2026-09-19 01:18:48 | 89,093 | 10k repair attempts / 0 | [4766c63](https://github.com/fywu85/lorr/commit/4766c633056ffb851447656eeee0e71f4e157ebd) † | [data](experiments/construction-20260918/results/full-v1/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v1/) · legacy timing |
| 2026-09-19 01:18:51 | 89,518 | 10k repair attempts / 1 | [4766c63](https://github.com/fywu85/lorr/commit/4766c633056ffb851447656eeee0e71f4e157ebd) † | [data](experiments/construction-20260918/results/full-v1/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v1/) · legacy timing |
| 2026-09-19 01:24:01 | 104,573 | 50k repair attempts / 0 | [4766c63](https://github.com/fywu85/lorr/commit/4766c633056ffb851447656eeee0e71f4e157ebd) † | [data](experiments/construction-20260918/results/full-v1/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v1/) · legacy timing |
| 2026-09-19 01:25:00 | 104,788 | 50k repair attempts / 1 | [4766c63](https://github.com/fywu85/lorr/commit/4766c633056ffb851447656eeee0e71f4e157ebd) † | [data](experiments/construction-20260918/results/full-v1/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v1/) · legacy timing |
| 2026-09-19 01:46:57 | 107,579 | 50k repair, equal weights / 1 | [4766c63](https://github.com/fywu85/lorr/commit/4766c633056ffb851447656eeee0e71f4e157ebd) † | [data](experiments/construction-20260918/results/full-v3/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v3/) · legacy timing |
| 2026-09-19 01:54:12 | 108,047 | Four 50k workers, equal weights / 0 | [4766c63](https://github.com/fywu85/lorr/commit/4766c633056ffb851447656eeee0e71f4e157ebd) † | [data](experiments/construction-20260918/results/full-v4/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v4/) · legacy timing |
| 2026-09-19 02:28:22 | 109,676 | Pickup weighting + global shortlist / 0 | [4766c63](https://github.com/fywu85/lorr/commit/4766c633056ffb851447656eeee0e71f4e157ebd) † | [data](experiments/construction-20260918/results/scheduler-v7/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v7/) · full run |
| 2026-09-19 03:12:43 | 109,836 | Pickup weighting / 0 | [4766c63](https://github.com/fywu85/lorr/commit/4766c633056ffb851447656eeee0e71f4e157ebd) † | [data](experiments/construction-20260918/results/scheduler-v7/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v7/) · full run |
| 2026-09-19 04:02:33 | 111,118 | Pickup/global shortlist + 4M work / 0 | [f214e07](https://github.com/fywu85/lorr/commit/f214e07f3de33903a6117d6503e52cc224ca8be5) | [data](experiments/construction-20260918/results/work-full-v8/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v8/) · full run |
| 2026-09-19 05:08:36 | 111,411 | Regional repair, one round / 0 | [593f7f3](https://github.com/fywu85/lorr/commit/593f7f3ad5b685fc1b69d62b8791b3743863246b) | [data](experiments/construction-20260918/results/review-full-v14/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v14/) · full run |
| 2026-09-19 05:17:05 | 112,164 | Regional repair, two rounds / 0 | [593f7f3](https://github.com/fywu85/lorr/commit/593f7f3ad5b685fc1b69d62b8791b3743863246b) | [data](experiments/construction-20260918/results/review-full-v14/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v14/) · full run |
| 2026-09-19 08:11:17 | 122,896 | Frozen learned flow, margin 50 / 0 | [fafd9f5](https://github.com/fywu85/lorr/commit/fafd9f5cb028ccd87a848a0338cc5e126e2f8d62) | [data](experiments/construction-20260918/results/flow-margin-full-v20/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v20/) · not robust |
| 2026-09-19 10:49:30 | 134,511 | Learned flow, refresh every 512 / 0 | [e14ecfd](https://github.com/fywu85/lorr/commit/e14ecfdbb72dfa5125e217f7e8eefad3296ec0a1) | [data](experiments/construction-20260918/results/flow-refresh-first-pair-v30/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v30/) · seed 0 only |
| 2026-09-19 11:26:47 | 134,519 | Learned flow, refresh every 512 / 2 | [e14ecfd](https://github.com/fywu85/lorr/commit/e14ecfdbb72dfa5125e217f7e8eefad3296ec0a1) | [data](experiments/construction-20260918/results/flow-refresh-full-v30/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v30/) · seeds 0/2 confirmed |
| 2026-09-19 12:37:41 | 135,177 | Refresh512 + strict wait turns /0 | [846360e](https://github.com/fywu85/lorr/commit/846360ec9f8f0fddbd2d5d52544113e799945f43) | [data](experiments/construction-20260918/results/strict-wait-turn-full-v32/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v32/) · mixed two-seed effect |
| 2026-09-19 14:13:43 | 135,357 | Refresh512 + two-owner transactions /2 | [45ebea2](https://github.com/fywu85/lorr/commit/45ebea2680e393a1e3ae060f29194b494bedc254) | [data](experiments/construction-20260918/results/temporal-transaction-full-v36/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v36/) · not promoted: six-seed mean loses |
| 2026-09-19 14:19:27 | 136,149 | Direct cost + pickup weight5 /0 | [348b92f](https://github.com/fywu85/lorr/commit/348b92fd27b528a71c956d09d638cc40ebe33e0c) | [data](experiments/construction-20260918/results/flow-refresh-pickup-full-v33/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v33/) · not robust: seed2 collapses |
| 2026-09-19 14:58:29 | 136,888 | Refreshed flow, margin 25 / 0 | [45ebea2](https://github.com/fywu85/lorr/commit/45ebea2680e393a1e3ae060f29194b494bedc254) | [data](experiments/construction-20260918/results/flow-refresh-neighbors-full-v36/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v36/) · two seeds only |
| 2026-09-19 15:08:42 | 137,190 | Refresh512, turn builds 64 / 0 | [5c7f0fa](https://github.com/fywu85/lorr/commit/5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27) | [data](experiments/construction-20260918/results/turn-build-limit-full-v39/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v39/) · two seeds only |
| 2026-09-19 15:10:30 | 137,490 | Refresh512, turn builds 96 / 0 | [5c7f0fa](https://github.com/fywu85/lorr/commit/5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27) | [data](experiments/construction-20260918/results/turn-build-limit-full-v39/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v39/) · two seeds only |
| 2026-09-19 15:37:18 | 137,848 | Parallel batch512 / 0 | [50e8e6e](https://github.com/fywu85/lorr/commit/50e8e6e4cef1495a4054590e613cf762eb39009c) | [data](experiments/construction-20260918/results/temporal-table-batch-full-v40/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v40/) · not robust: seed2 regresses |
| 2026-09-19 15:38:41 | 138,003 | Parallel batch128 / 0 | [50e8e6e](https://github.com/fywu85/lorr/commit/50e8e6e4cef1495a4054590e613cf762eb39009c) | [data](experiments/construction-20260918/results/temporal-table-batch-full-v40/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v40/) · not promoted: six-seed mean loses |
| 2026-09-19 15:55:29 | 139,037 | Margin25 + turn builds128 / 2 | [5c7f0fa](https://github.com/fywu85/lorr/commit/5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27) | [data](experiments/construction-20260918/results/flow-turn-interaction-full-v39/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v39/) · six-seed reference confirmed |
| 2026-09-19 16:13:44 | 139,303 | Graded scale4/strength4, margin25 / 2 | [5c7f0fa](https://github.com/fywu85/lorr/commit/5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27) | [data](experiments/construction-20260918/results/flow-graded-turn128-full-v39/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v39/) · six-seed reference confirmed |
| 2026-09-19 16:13:53 | 139,796 | Graded scale4/strength4, margin0 / 2 | [5c7f0fa](https://github.com/fywu85/lorr/commit/5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27) | [data](experiments/construction-20260918/results/flow-graded-turn128-full-v39/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v39/) · rejected: seed0 collapses |
| 2026-09-19 17:42:42 | 141,725 | Complete pickup fields16 / 2 | [4872d04](https://github.com/fywu85/lorr/commit/4872d048dd8155927074ac2a4d27a55756266fd5) | [data](experiments/construction-20260918/results/pickup-full-full-v42/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v42/) · six-seed improvement confirmed |
| 2026-09-19 17:43:24 | 142,735 | Complete pickup fields32 / 2 | [4872d04](https://github.com/fywu85/lorr/commit/4872d048dd8155927074ac2a4d27a55756266fd5) | [data](experiments/construction-20260918/results/pickup-full-full-v42/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v42/) · rejected: seed0 collapses |
| 2026-09-19 17:43:34 | 142,988 | Complete pickup fields64 / 2 | [4872d04](https://github.com/fywu85/lorr/commit/4872d048dd8155927074ac2a4d27a55756266fd5) | [data](experiments/construction-20260918/results/pickup-full-full-v42/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v42/) · six-seed improvement confirmed |
| 2026-09-19 18:13:40 | 143,325 | Complete pickup fields64 / 1 | [4872d04](https://github.com/fywu85/lorr/commit/4872d048dd8155927074ac2a4d27a55756266fd5) | [data](experiments/construction-20260918/results/pickup-full-confirm-v42/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v42/) · six-seed reference confirmed |
| 2026-09-19 18:31:17 | 143,340 | Complete fields64 + four 4M workers / 0 | [1661176](https://github.com/fywu85/lorr/commit/1661176ca3dd6854a7ba8d1a8a0ec8e33a2cffa9) | [data](experiments/construction-20260918/results/pickup-full-workers-full-v43/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v43/) · not promoted: seed5 regresses |
| 2026-09-19 18:32:22 | 143,409 | Complete fields64 + four 4M workers / 2 | [1661176](https://github.com/fywu85/lorr/commit/1661176ca3dd6854a7ba8d1a8a0ec8e33a2cffa9) | [data](experiments/construction-20260918/results/pickup-full-workers-full-v43/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v43/) · not promoted: seed5 regresses |
| 2026-09-19 19:05:34 | 143,767 | Complete fields64 + four 4M workers / 1 | [1661176](https://github.com/fywu85/lorr/commit/1661176ca3dd6854a7ba8d1a8a0ec8e33a2cffa9) | [data](experiments/construction-20260918/results/pickup-full-workers-confirm-v43/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v43/) · not promoted: seed5 regresses |
| 2026-09-19 20:30:36 | 144,107 | Complete fields64 + two regional rounds / 2 | [f6d0ae4](https://github.com/fywu85/lorr/commit/f6d0ae448504762136c4f5d85f25d7d207207965) | [data](experiments/construction-20260918/results/pickup-full-regions-full-v44/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v44/) · six-seed improvement confirmed |
| 2026-09-19 20:32:02 | 144,510 | Complete fields64 + two regional rounds / 0 | [f6d0ae4](https://github.com/fywu85/lorr/commit/f6d0ae448504762136c4f5d85f25d7d207207965) | [data](experiments/construction-20260918/results/pickup-full-regions-full-v44/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v44/) · six-seed improvement confirmed |
| 2026-09-20 05:28:00 | 145,083 | Remaining-flow temporal score / 0 | [4524843](https://github.com/fywu85/lorr/commit/45248431dc38702ebdef9dd1ab2893697c00e00f) | [verified full pair](experiments/construction-20260918/results/remaining-flow-full-v56-shared/comparison.json) · shared-host5s development; six-seed result +0.08267%, not promoted |

† Some early binaries were built between commits. These links identify the commit containing their **frozen source snapshot/patch**, not a claim that the commit’s working tree exactly matches the binary. Unmarked commits were checked against **every recorded production-source SHA-256**. The source link and binary hash in the evidence distinguish experiments archived in the same commit.

“Legacy timing” means that the original run predates complete entry-time instrumentation. Its configured deadline, explicit timeout result, simulator validity and measured RSS are retained, but it does not supply the later per-decision timing evidence. CPU models and core counts also vary; this table tracks task completions, not runtime speedups.

The latest generic trial improves both full seeds0/2:145,083/144,215 versus exact
144,510/144,107 controls, a paired mean gain0.235953%. Final1000windows rise72/112;
outstanding-age p90 falls2/4. All20,000entries are valid under the authorized5s
budget and peakRSS11.340GB. One candidate entry reached1004.262ms; these shared-host
runs do not certify strict1s performance. The remaining seeds1/3/4/5 completed:
all six paired effects are +573/+606/+108/-793/+223/-3. The reference remains unchanged.

## Confirmed reference milestones

Each row uses all six full warehouse seeds. Completion is the last candidate run finishing; these are policy comparisons, not runtime-speedup claims.

| Completed (UTC) | Mean tasks | Range | Policy | Exact source | Evidence |
|---|---:|---:|---|---|---|
| 2026-09-19 15:28:50 | 136,426.2 | 135,838–136,888 | Binary margin25, build32 | [45ebea2](https://github.com/fywu85/lorr/commit/45ebea2680e393a1e3ae060f29194b494bedc254) | [data](experiments/construction-20260918/results/flow-refresh-margin25-six-seed-v36.json) |
| 2026-09-19 15:40:34 | 137,073.3 | 136,841–137,271 | Binary margin50, build128 | [5c7f0fa](https://github.com/fywu85/lorr/commit/5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27) | [data](experiments/construction-20260918/results/turn-build-limit-six-seed-v39.json) |
| 2026-09-19 16:34:42 | 138,104.2 | 137,149–139,037 | Binary margin25, build128 | [5c7f0fa](https://github.com/fywu85/lorr/commit/5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27) | [data](experiments/construction-20260918/results/flow-turn-interaction-six-seed-v39.json) |
| 2026-09-19 16:47:34 | 138,948.0 | 138,304–139,716 | Graded strength4/scale4, margin25/build128 | [5c7f0fa](https://github.com/fywu85/lorr/commit/5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27) | [data](experiments/construction-20260918/results/flow-graded-six-seed-v39.json) |
| 2026-09-19 17:57:50 | 139,387.8 | 138,671–140,002 | Graded margin25/build128 + pickup flow | [eb5b382](https://github.com/fywu85/lorr/commit/eb5b3827b9daf9f91aba88c78bef7be0d17cd07c) | [data](experiments/construction-20260918/results/graded-pickup-flow-six-seed-v41.json) |
| 2026-09-19 18:12:34 | 141,147.8 | 139,697–141,925 | Graded pickup + complete fields16 | [4872d04](https://github.com/fywu85/lorr/commit/4872d048dd8155927074ac2a4d27a55756266fd5) | [data](experiments/construction-20260918/results/pickup-full-six-seed-v42.json) |
| 2026-09-19 18:13:40 | 142,474.8 | 141,802–143,325 | Graded pickup + complete fields64 | [4872d04](https://github.com/fywu85/lorr/commit/4872d048dd8155927074ac2a4d27a55756266fd5) | [data](experiments/construction-20260918/results/pickup-full-six-seed-v42.json) |
| 2026-09-19 21:14:15 | 143,941.2 | 143,134–144,510 | Complete fields64 + two regional repair rounds | [f6d0ae4](https://github.com/fywu85/lorr/commit/f6d0ae448504762136c4f5d85f25d7d207207965) | [data](experiments/construction-20260918/results/pickup-full-regions-six-seed-v44.json) |
| 2026-09-20 09:13:44 | 144,392.2 | 143,340–144,967 | Regional reference + bounded matching64 | [0196851](https://github.com/fywu85/lorr/commit/019685109769788dfcc318815e8d102a376982a9) | [data](experiments/construction-20260918/results/match-quota-six-seed-v64.json); shared5s development |

## Confirmation and reversals

| Policy | Full-run evidence | Current interpretation |
|---|---|---|
| No-flow fixed 4M | Six seeds: 109,244 / 109,249 / 109,182 / 109,110 / 109,071 / 109,182; mean **109,173** | Stable matched control; full timing and memory checks pass. |
| Frozen margin-50 flow | Seeds 0/1/2: 122,896 / 122,195 / 70,171; mean **105,087.3** | Historical peak retained, but **not promoted**: seed 2 collapses late and the mean loses to its matched control. |
| Refresh every 512 | Six seeds: **134,511 / 134,859 / 134,519 / 134,061 / 134,626 / 134,966**; mean **134,590.3** | All six independently analyzed; repeated controls exactly reproduce. Final windows 28,167–28,368. |
| Refresh every 1,024 | Seeds 0/2: 133,652 / 131,316; mean 132,484 | Both recover steady final-window throughput, but interval 512 wins both full totals. |
| Frozen flow with warm reuse | Seeds 0/2: 123,373 / 123,251; mean 123,312 | Recovers seed 2 without updating the field; below refresh512 on both tested seeds. |
| Gentler frozen-flow penalties | Scale4 seeds0/2: 121,812 /119,927; scale8: 119,337 /117,897 | Both recover seed2, but lower healthy-seed throughput and remain below refresh512. |
| Strict wait turns + refresh512 | Seeds0/2: **135,177 /133,672**, mean134,424.5; all decisions valid | Historical single-run peak; mean is0.067% below ordinary refresh on these seeds. No additive gain established. |
| Refresh512 + warm reuse | Seeds0/2: 134,997 /134,794; mean134,895.5 | Small positive paired effect (+0.283%); broader confirmation pending. Refresh256 slightly loses on both seeds. |
| Cache-only resets | Four strict-mode/seed contrasts: actual refresh beats cache-only every time | Actual traffic-cost updates add8.74–9.84% above cache-only in healthy controls; legacy seed2 still deteriorates with cache-only. [Evidence](experiments/construction-20260918/results/flow-cache-only-full-v33/causal-comparison.json). |
| Additional parallel search | Four4M workers:134,975 /134,603, mean134,789 | Only+0.204% over one worker; four1M workers collapse despite valid decisions. [Full evidence](experiments/construction-20260918/results/flow-refresh-workers-full-v33/comparison.json). |
| Gentler refreshed costs | Scale2 mean132,957; scale4 mean130,823.5 | Lose1.16% /2.74% to ordinary refresh512; not promoted. |
| Direct-cost dispatch | Seeds 0/2: 131,199 / 130,947; mean 131,073 | Loses 2.559%; chain refinement leaves both trajectories unchanged. Shorter loaded chains accompany about 17% more empty robot-steps. [Evidence](experiments/construction-20260918/results/flow-refresh-dispatch-full-v33/comparison.json). |
| Two-owner temporal transactions | Six seeds:134,584 /135,299 /135,357 /132,125 /133,325 /134,525; mean134,202.5 | Wins3of6, loses0.288% in the mean. All12paired runs valid and all six controls exactly reproduce. Not promoted. [Evidence](experiments/construction-20260918/results/temporal-transaction-six-seed-v36.json). |
| Direct cost, pickup weights3/5 | Weight3:134,397 /134,194; weight5:136,149 /52,444 | Weight3 loses0.163%. Weight5 seed2 collapses to4,447 tasks in the final window. Historical peak retained; both rejected. [Evidence](experiments/construction-20260918/results/flow-refresh-pickup-full-v33/comparison.json). |
| Unopened pool exchange | Six seeds: 135,409 / 135,476 / 134,762 / 135,078 / 134,311 / 134,009; mean 134,840.8 | +0.186%; wins four, loses two. Outstanding age p90 rises on every seed. Not promoted. [Evidence](experiments/construction-20260918/results/pool-exchange-six-seed-v37.json). |
| Refreshed margin 25 | Six seeds: **136,888 / 135,838 / 135,960 / 136,885 / 136,582 / 136,404**; mean **136,426.2** | **Confirmed intermediate reference.** +1.364%; all six full totals and final windows improve, and outstanding age p90 falls on every seed. All 12 paired runs valid with exact controls. [Evidence](experiments/construction-20260918/results/flow-refresh-margin25-six-seed-v36.json). |
| Larger turn-table rebuild allowance | Count128 six seeds: **137,060 / 137,271 / 136,841 / 137,227 / 136,949 / 137,092**, mean **137,073.3** | **Confirmed intermediate reference.** +1.845% over build32, winning all six totals and final windows. Also exceeds margin25/build32 on every seed. [Evidence](experiments/construction-20260918/results/turn-build-limit-six-seed-v39.json). |
| Complete parallel table batches | Batch128 seeds0/2: **138,003 / 136,701**; batch512: **137,848 / 119,685** | Batch128 serial and parallel reproduce both full trajectories exactly; parallel wall time falls5.86–6.37%. Batch512 has100% sampled exact coverage but regresses, so coverage is not a monotone quality measure. [Evidence](experiments/construction-20260918/results/temporal-table-batch-full-v40/comparison.json). |
| Parallel batch128, six-seed confirmation | **138,003 /137,388 /136,701 /137,477 /134,785 /137,390**, mean **136,957.3** | Four wins/two losses; mean **0.085% below** demand-build128. Seed4 erases the gain. All decisions valid; four repeated stronger controls exact. Not promoted. [Evidence](experiments/construction-20260918/results/temporal-table-batch-six-seed-v40.json). |
| Margin25 + turn-build128 | Six seeds: **137,149 /138,572 /139,037 /137,535 /137,875 /138,457**, mean **138,104.2** | **Confirmed intermediate reference.** All six full totals and five final windows improve, mean **+0.752%** over margin50/build128. Seed3 final window is109tasks lower. All12runs valid and all six controls exact. [Evidence](experiments/construction-20260918/results/flow-turn-interaction-six-seed-v39.json). |
| Graded penalties, strength4/scale4, margin25 | Six seeds: **138,402 /139,716 /139,303 /138,506 /138,304 /139,457**, mean **138,948** | **Confirmed intermediate reference.** All six totals improve; mean **+0.611%**. Final windows improve on four seeds; seeds2/4 lose153/134tasks. All12runs valid and six controls exact. [Evidence](experiments/construction-20260918/results/flow-graded-six-seed-v39.json). |
| Graded penalties, zero confidence threshold | Seeds0/2: **56,537 /139,796** | **Rejected.** Seed0 final window collapses to5,770tasks despite all complete decisions meeting time/memory limits. Its historical peak stays visible. [Evidence](experiments/construction-20260918/results/flow-graded-turn128-full-v39/comparison.json). |
| Heading/traffic pickup costs, ordinary HRRN | Seeds0/2: **137,508 /137,923**, mean **137,715.5** | Both totals/final windows improve over binary margin50/build128 (**+0.559%**); outstanding agep90 rises. The graded combination is now confirmed on all six seeds. [Evidence](experiments/construction-20260918/results/pickup-flow-full-v41/comparison.json). |
| Heading/traffic pickup costs, direct cost/weight5 | Seeds0/2: **138,242 /49,732** | **Rejected.** Seed2 ends with4,522tasks in its final window; matched disabled controls137,603/136,560 stay healthy. All decisions valid, so this is a policy collapse. [Evidence](experiments/construction-20260918/results/pickup-flow-full-v41/comparison.json). |
| Double orientation-cache allowance |8/16GiB produce the same full trajectories on both binary-margin25 seeds | Saves197/217rebuilds without changing guided/fallback counts or throughput. Retain8GiB. [Evidence](experiments/construction-20260918/results/turn-cache-capacity-full-v39/comparison.json). |
| Graded pickup flow | Six seeds: **138,963 /140,002 /139,677 /138,671 /139,241 /139,773**, mean **139,387.8** | **Confirmed intermediate reference.** All six totals/finalwindows improve, mean+0.317%; empty robot-steps fall1.46–2.15%, agep90 rises7–20steps. All12runs valid; all six controls exactly reproduce. [Evidence](experiments/construction-20260918/results/graded-pickup-flow-six-seed-v41.json). |
| Complete pickup fields16 | Six seeds: **139,697 /141,925 /141,725 /140,734 /140,928 /141,878**, mean **141,147.8** | Confirmed improvement of **1.263%** over graded pickup ON; all six totals/final windows/age p90 improve. Fields64 wins every full total. [Evidence](experiments/construction-20260918/results/pickup-full-six-seed-v42.json). |
| Complete pickup fields32 | Seeds0/2: **70,043 /142,735** | **Rejected.** Seed0 finalwindow5,433 despite valid complete decisions. Historical high retained. [Evidence](experiments/construction-20260918/results/pickup-full-full-v42/comparison.json). |
| Complete pickup fields64 | Six seeds: **141,829 /143,325 /142,988 /141,802 /141,988 /142,917**, mean **142,474.8** | **Confirmed intermediate reference.** Mean **+2.215%** over graded pickup ON; all six totals/final windows/age p90 improve. All 18 paired runs valid, all six disabled controls exactly reproduce. [Evidence](experiments/construction-20260918/results/pickup-full-six-seed-v42.json). |
| Cost-based complete-field discovery | Fields32 seeds0/2: **135,244 /137,131**; fields64: **135,773 /79,601** | **Both rejected.** Fields32 loses 2.248% to the healthy disabled control despite rescuing its collapsed counterpart. Fields64 loses both seeds to original64 and collapses seed2. Empty travel and outstanding ages rise. All four default controls exactly reproduce v42. [Evidence](experiments/construction-20260918/results/pickup-cost-key-full-v43/comparison.json). |
| Complete fields64 + four planning workers | Six seeds: **143,340 /143,767 /143,409 /143,359 /142,868 /139,710**, mean **142,742.2** | **Not promoted.** Mean+0.188%, five wins, but seed5 loses3,207tasks, finalwindow1,933 and agep90 rises53steps. All12runs valid and six controls exactly reproduce. [Evidence](experiments/construction-20260918/results/pickup-full-workers-six-seed-v43.json). |
| Traffic strength with complete fields64 | Strength2 seeds0/2: **141,776 /143,213**; strength6: **141,085 /142,854**; strength8: **139,351 /142,470** | **Retain strength4.** Strength2 has mixed totals and +0.060% mean, with both final windows/ages improving. Strength6/8 loses both full totals. All8runs valid, both controls exact. [Evidence](experiments/construction-20260918/results/pickup-full-flow-strength-full-v43/comparison.json). |
| Complete fields64 + two regional rounds | Six seeds: **144,510 /143,933 /144,107 /143,134 /143,934 /144,029**, mean **143,941.2** | **Prior benchmark reference; superseded by matching64.** All six full totals improve, mean **+1.029%**. Five tails/ages improve; seed1tail−108/age+1. All12 runs valid and six controls exact. [Evidence](experiments/construction-20260918/results/pickup-full-regions-six-seed-v44.json). |
| Complete pickup quotas128/256 | Seeds0/2 exactly reproduce64-field full trajectories:141,829 /142,988 | No observed effect on either tested seed; retain64 fields and four construction threads. All6 runs valid. [Evidence](experiments/construction-20260918/results/pickup-full-quota-full-v46.json). |
| Revealed next-errand scoring | Seeds0/2:143,529 /144,090 versus144,510 /144,107 | **Not promoted.** Both totals and final windows lose; paired mean−0.346%. All20k complete entries valid and controls exact. [Evidence](experiments/construction-20260918/results/next-errand-full-v47/comparison.json). |
| Zero-temperature regional repair | Seeds0/2:143,600 /144,083 | **Not promoted.** Both totals lose; paired mean−0.324%. All20k entries valid and controls exact. Full audit finds only9/10 discarded peaks per40k reference batches. [Evidence](experiments/construction-20260918/results/regional-peak-full-v48/comparison.json). |
| Eight regions / eight threads | Seeds0/2:144,096 /143,553 | **Not promoted.** Both totals and final windows lose despite twice the regional work; paired mean−0.335%. All20k entries valid; four-region controls and cumulative counters exact. [Evidence](experiments/construction-20260918/results/regional-eight-full-v48/comparison.json). |
| Physical-score dominance, scale513 | Seed0 explicit timeout at2230; seed2:142,171 versus144,107 | **Rejected.** The completed seed loses1,936tasks and the other has no full quality result. Both controls exact; all15k completed entries valid. No two-seed mean is reported. [Evidence](experiments/construction-20260918/results/regional-score-scale-full-v48/comparison.json). |

| More regional rounds / less global work | 3M:143,056 /143,421; 2M:seed0 timeout, seed2 143,046 | **Rejected.** 3M mean−0.741%, both final windows lose. 2M fails at step36; no two-seed mean. Both controls exact, all25k completed entries valid. [Evidence](experiments/construction-20260918/results/regional-work-balance-full-v48/comparison.json). |

[Six-seed refresh evidence](experiments/construction-20260918/results/flow-refresh-six-seed-v30.json), [matched three-seed comparison](experiments/construction-20260918/results/flow-margin-matched-controls-v20.json), [complete refresh results](experiments/construction-20260918/results/flow-refresh-full-v30/), [warm-reuse results](experiments/construction-20260918/results/flow-warm-full-v31/), [record provenance](experiments/construction-20260918/results/throughput-progress-provenance.json).

## Latest full experiments and matching coverage

Task-chain pricing V60 changes neither best: resident/imputed144,237/144,023 and
ratio-only144,604/143,946, versus exact144,510/144,107 controls. Paired means
−0.124%/−0.023%; both remain OFF. [Full evidence](experiments/construction-20260918/results/chain-pricing-full-v60/comparison.json).
Four independent global searches before regional repair reaches143,312/144,336,
paired mean−0.336%; retain one global search. [Full evidence](experiments/construction-20260918/results/global-region-workers-full-v60/comparison.json).

The new generic unopened matching layer initially sampled a geographically sparse
holder pool. [1d78892](https://github.com/fywu85/lorr/commit/1d788929db09f8f6a0aeb091548ff8c3ff7aba87)
corrects this with all-resident local searches, retaining4groups x32holders and
2048BFS-node limits. Full regressions pass. The800-step control/matching scores
are18,954/18,919 and18,952/18,944; all valid, exact disabled controls, max927.438ms
and peak8.074GB. These are feasibility results, not records. Its full trial was
cancelled after source review found two new-mode protection defects.
[f189cbf](https://github.com/fywu85/lorr/commit/f189cbf) fixes stale next-primary
tickets and mandatory fair-task registration; new production cases reproduce
the defects and the full suite passes after repair. A fixed matching-quota
comparison is now being prepared. [Design and evidence](experiments/construction-20260918/unopened_matching/README.md).

## Resource cost of the current benchmark reference

The promoted matching64 profile averages **387.3–401.2 ms** per complete step,
maximum **921.361 ms**, RSS **12.105 GB**, CPU **1.727–1.772 cores** of four reserved,
and **33.33–34.47 minutes** per full run. All six candidates are valid under the
shared-host5s development limit. [Verified resources](experiments/construction-20260918/results/match-quota-six-seed-v64.json).

The following measurements describe the preceding regional-only reference.


V50's duplicate deadline-callback optimization now passes full seeds0/2 with
**exactly the existing144,510 /144,107 trajectories and sampled search counters**.
All10,000 complete entries pass; means384.1–406.1ms, max919.2ms, CPU1.72–1.77cores,
peakRSS11.337GB and full wall33.1–34.9min. These are two-run measurements on new
allocations, not a controlled full-run speedup or a new six-seed resource range.
The original six-seed reference resources remain below.
[Exact source cb6a666](https://github.com/fywu85/lorr/commit/cb6a666da8aee461d5a34cab01d7b7ccaa76a201),
[full verification](experiments/construction-20260918/results/deadline-duplicate-full-v50/verification.json).


The current regional-repair reference averages **401.2–419.4 ms** per complete
schedule+plan step and reaches **962.7 ms** maximum. Whole-process CPU averages
**1.726–1.770 cores** of four reserved, peak RSS **13.909 GB**, and full runs take
**34.50–35.93 minutes**. Every entry covers all10,000 robots. All30,000 candidate
entries meet one second. CPU is process user+system seconds divided by wall time,
including startup, simulation and logging. These are measured ranges, not p99s.
[Current resources and evidence](experiments/construction-20260918/results/pickup-full-regions-six-seed-v44.json).

The preceding 64-field reference without regional repair averages **272.4–317.4 ms** per complete
schedule+plan step, with median **241.2–298.6 ms**, p99 **557.3–602.0 ms**, and
maximum **874.2 ms**. Whole-process CPU averages **1.425–1.494 cores** of four
reserved (35.6–37.3% of the allocation), peak RSS **11.934 GB**, and full runs take
**23.93–27.70 minutes**. All 30,000 candidate entries meet one second. Each entry
covers all 10,000 robots. CPU includes startup, simulation and logging and is
user+system seconds divided by wall time; it is not instantaneous usage.
[Six-seed confirmation](experiments/construction-20260918/results/pickup-full-six-seed-v42.json),
[all-step quantiles](experiments/construction-20260918/results/full64-reference-latency-six-seed-v42.json),
[explicit configuration](experiments/construction-20260918/warehouse-reference-variants.json).

The preceding graded pickup-flow reference averages 241.5–284.0 ms, with median
210.1–262.9 ms, p99 529.8–570.6 ms and maximum 872.4 ms. CPU averages
1.171–1.204 cores, peak RSS is 11.901 GB, and full runs take 21.36–24.91 minutes.
All 30,000 entries meet one second.
[Preceding confirmation](experiments/construction-20260918/results/graded-pickup-flow-six-seed-v41.json),
[preceding quantiles](experiments/construction-20260918/results/graded-pickup-reference-latency-six-seed-v41.json).

The preceding graded strength4/scale4, margin25/build128 reference averages
**229.3–277.3ms** per complete scheduling plus planning step. Median latency is
**198.2–257.5ms**, p99 **509.0–567.5ms**, and maximum **874.2ms**. Average process
CPU is **1.174–1.215cores** out of four reserved, peak RSS **11.936GB**, and full
runs take **20.33–24.36minutes**. All30000entries meet the one-second limit.
CPU includes startup, simulator and logging, calculated as process user+system
seconds divided by wall seconds; it is not instantaneous or planner-only usage.
Hosts were exclusive and unthrottled.
[Preceding graded resources](experiments/construction-20260918/results/flow-graded-six-seed-v39.json),
[all-step quantiles](experiments/construction-20260918/results/graded-reference-latency-six-seed-v39.json),
[explicit configuration](experiments/construction-20260918/warehouse-reference-variants.json).

The preceding binary margin25/build128 cohort averages228.0–265.1ms, median
197.3–242.4ms, p99 507.0–557.1ms, max872.0ms, CPU1.183–1.212cores, peak11.933GB
and20.23–23.32minutes. Those were the latest figures in the resource answer before
graded confirmation completed. [Binary-reference quantiles](experiments/construction-20260918/results/margin25-reference-latency-six-seed-v39.json).

The preceding margin50/build128 cohort averages234.8–269.9ms, median206.3–248.6ms,
p99 514.1–554.0ms, maximum873.1ms, CPU1.182–1.210cores, peak12.013GB RSS and
20.80–23.75minutes. Those were the initial resource figures reported before the
new reference finished confirmation. [Preceding quantiles](experiments/construction-20260918/results/reference-latency-six-seed-v39.json).

The earlier margin25/build32 confirmation averages231.7–255.4ms, reaches787.3ms
maximum and14.052GB peak RSS, with1.195–1.215CPU cores. Its full runs take
20.53–22.53minutes. Margin50/build32 averages239.4–258.1ms and reaches13.121GB
peak RSS; a separate audit of that original policy has p99 latency323.8/340.1ms
and a repeated control reached792.9ms maximum. Those quantiles do not describe
the newer policies.
[Margin25 resources](experiments/construction-20260918/results/flow-refresh-margin25-six-seed-v36.json),
[original margin50 resources](experiments/construction-20260918/results/flow-refresh-six-seed-v30.json),
[original latency quantiles](experiments/construction-20260918/results/refresh-dwell-v1.json).

Batch128 with four build threads averages235.1–253.5ms on seeds0/2, uses1.28–
1.30CPU cores and reaches12.621GB peak RSS. Its full trajectories exactly match
batch128 with one build thread, while total wall time falls5.86–6.37%. The six-seed throughput
comparison with the stronger build128 reference loses0.085%, so this policy is not promoted.
[Full serial/parallel evidence](experiments/construction-20260918/results/temporal-table-batch-full-v40/comparison.json),
[six-seed confirmation](experiments/construction-20260918/results/temporal-table-batch-six-seed-v40.json).

Four full-size search workers average271–277ms per decision and2.60–2.63CPU
cores, but add only0.204% throughput on seeds0/2; they are not promoted.
[Parallel-search evidence](experiments/construction-20260918/results/flow-refresh-workers-full-v33/comparison.json).

An independent full-trajectory action audit finds 10.36–11.19 loaded turns and
14.10–15.31 loaded waits per completed task on the 64-field seeds 0/2, compared
with 5.68 turns and 8.41 waits in the local KittyKnight run. Loaded forward travel
also exceeds the unweighted shortest-chain bound by 8.20–9.12 steps versus 4.59.
These are different completed-task cohorts, with unfinished work censored; the
component differences are not causal savings. They motivate the next motion tests.
[Audit and limits](experiments/construction-20260918/LOADED_MOTION.md).

## 2026-09-20 completed ablations

**TRICK — short-task preference:** source [f939b3b](https://github.com/fywu85/lorr/commit/f939b3b1ab44368e83617a9f97300f26c4063210), full runs completed 2026-09-20T06:06:25.746237UTC and independently verified 06:08:33UTC.
Two-seed means: control 144308.5, short preference 135706.5, lanes 146612.5, both 137959.5.
Disabling age preference/oldest admission loses 5.961% without lanes and 5.902% with
lanes. Empty work increases 34.31%/32.83%; unpicked tasks aged≥4000 rise from 0 to
1356–1392. Keep OFF. All eight full runs valid at the shared 5s development budget,
RSS<11.945GB; both full control trajectories match. No generic-frontier change.
[Full evidence](experiments/construction-20260918/results/trick-short-tasks-full-v58-r2/summary.md).

**GENERIC — remaining-flow confirmation:** source [4524843](https://github.com/fywu85/lorr/commit/45248431dc38702ebdef9dd1ab2893697c00e00f), confirmation completed 2026-09-20T06:09:51.571680UTC.
All six paired seed effects: +573/+606/+108/-793/+223/-3. Candidate mean 144060.1667
versus 143941.1667 (+0.08267%); four wins/two losses. The four confirmation seeds
average only +8.25 tasks. Keep default OFF and confirmed regional reference unchanged;
the 145083 single-run record remains visible. All 12 full runs valid at shared 5s;
this is not strict 1s certification or evidence over independent task streams.
[Combined evidence](experiments/construction-20260918/results/remaining-flow-six-seed-v56-shared.json).

## Overnight matching checkpoint, 2026-09-20 08:14 UTC

The bounded matching candidate now protects the actual next primary and registers
mandatory fair admissions. Both fixes have reproducing production regressions.
The earlier V62 full run was cancelled; it supplies no quality score. V64 compares
fixed group quotas 4 and 64 with OFF. Its six 800-step cases are valid and the OFF
trajectories exact, but the differences are too small and early to rank policies.
[Quota screen](experiments/construction-20260918/results/match-quota-screen-v64/comparison.json).

The full general comparison (six cases, jobs 8899381/8899382) and explicitly gated
Warehouse lanes/short-preference/matching factorial (eight cases, jobs
8899390/8899391) are running or queued in parallel. Each case reserves four
physical cores, with the authorized shared-host 5s development deadline and
32GB RSS cap. The trick startup screen reproduces both controls exactly.
[Trick design and evidence](experiments/construction-20260918/tricks/unopened_matching/README.md).
No new full-run record or reference promotion is claimed.

## 2026-09-20 08:40 UTC: matching improves both tracks

**GENERIC:** bounded64-group matching scores144,967/144,511 against exact
144,510/144,107 controls (+0.29832% mean). Both final-1000 rates and agep90 improve;
empty work falls125,579/137,209 robot steps. All six OFF/4/64 runs valid, max928.069ms,
RSS<11.915GB, shared5s development. Four additional full paired seeds are running
under8899409/8899410. Default and confirmed generic reference remain unchanged.
[Full report](experiments/construction-20260918/results/match-quota-full-v64/summary.md).

**TRICK:** static lanes plus matching64 reaches the new overall best147,328,
with seed2 at147,204. Both beat their lane-only controls (+669/+638); both late
rates improve, and empty work decreases. Short preference plus matching recovers
3.61% over short preference alone but remains weaker than lanes-only; keep it OFF.
All eight full cases valid under shared5s/32GB, every control trajectory exact.
Best-run mean315.781ms, maximum1.011861s, RSS11.928GB, average1.858CPU cores of four
reserved, full27.43minutes. These are not exclusive-core or strict1s measurements.
[Full report and exact best](experiments/construction-20260918/results/trick-match-full-v65/summary.md).

The optional mixed pickup grouping full trial8899402/8899408 continues. The next
isolated TRICK experiment changes only paid-forward-extra scoring under the fixed
static lane metric; source0f4183f, all regressions and exact-control startup screen
pass. Full0/2 comparison8899413/8899414 is running/queued. Fable turn34's qualified
review is archived. No prefix is treated as a new full-run record.

## 2026-09-20 09:17 UTC: general confirmation and another TRICK best

Generic matching64 is promoted:144967/144869/144511/143340/144446/144220,
mean144392.17, +451 (+0.3133%) over the exact prior six-seed reference.
All six totals and five tails improve; seed5tail−49, seed3agep90+2 remain visible.
[Confirmation](experiments/construction-20260918/results/match-quota-confirm-v64/summary.md).

Static remaining-potential scoring reaches147422/147502 without matching,
both full totals and tails above lane-only. This sets the new overall TRICK best.
[Full evidence](experiments/construction-20260918/results/trick-potential-full-v68/summary.md).
Mixed pickup grouping loses both full seeds and stays OFF. The combined static
score+matching policy passes all regressions and is in its three-arm startup
screen8899439/8899440. A separate pickupweight1/5 x shortOFF/ON full factorial
continues8899427/8899428; no outcome claimed yet. Fable turn35 is complete in the
same persistent session; native metric work is the next separate motion test.

## 2026-09-20 09:50 UTC: TRICK combination reaches 148,132

Static lanes + remaining potential + matching64 achieves148132/148101 on seeds0/2,
beating both exact single-component controls. Full gains versus score-only are
+710/+599; final1000 gains +140/+157. Short preference stays OFF. All six full
runs valid, exact source c696d5f, shared5s, distinct physical cores, RSS<12GB.
[Full comparison and exact best](experiments/construction-20260918/results/trick-score-match-full-v72/summary.md).
The prior pickupweight5 trial also improves both matching-only controls by
+129/+291, but does not exceed the new best. Its interaction with the combined
score is the next bounded test. No generic reference change.

The native20/200, turn1 metric is committed as06a8258 and passes the full regression
suite, both exact legacy startup controls and the 800-step exact generic control.
Its four-arm full comparison (seeds0/2, 32 bound physical cores) is running under
8899463/8899464. This compound field/scalar/scheduler experiment requires the
explicit Warehouse flag and has no performance claim yet. Fable turn36 reviews
the source in the same persistent session.

## 2026-09-20 10:20 UTC: TRICK best148,440 and a general compute improvement

Pickupweight5 adds241/339tasks to the combined lanes/potential/matching policy,
with both late windows and outstanding-age p90 improving. All four full runs
are valid and controls exact. Short preference stays OFF; generic reference stays
144392.17. [Full result](experiments/construction-20260918/results/combined-pickup-full-v74/summary.md).

The V72 movement replay accounts for300million physical robot steps. Completed
chain lengths are nearly unchanged; the combined policy reduces empty travel and
loaded overhead versus both single components. These are associations across
changing cohorts. [Audit](experiments/construction-20260918/results/combined-motion-audit-v76/summary.md).

A general complete-pickup implementation change removes runtime modulo while
preserving every distance and pop/state count. A controlled one-core kernel test
measures9–23% lower CPU time, not a whole-entry or throughput gain. All regressions
pass and all four native200-step trajectories remain exact. The generic800-step
control continues. [Kernel comparison](experiments/construction-20260918/results/pickup-ring-kernel-v77/summary.md).
Native full cost comparisons continue; their final validated report is pending.

## 2026-09-20 10:30 UTC: native TRICK field reaches151,630

Native20/200forward costs, turn1 and raw remaining-potential scoring improve both
full seeds. Without bands149915/149871; with bands151443/151630. Both totals and
late windows improve substantially over exact147422/147502 controls. All eight
full cases and40,000entries valid, all four controls exact, no failures. Best-run
mean449.312ms/max1.016591s/RSS11.937GB, shared5s; no strict1s certification.
[Full result and best record](experiments/construction-20260918/results/native-metric-full-v73/summary.md).

The best still has matching and short preference OFF. The next predeclared
comparison measures native routing, then matching64, then pickupweight5 for both
fields. Its six-arm startup screen8899506/8899507 is underway; no combined native
quality gain is claimed yet. Fable37 supports this bounded comparison and ranks
explicit short preference on the best native/matched/weighted policy next if a gap
remains. That hypothesis still needs tests and paired full measurement.

## Updating this log

After each completed full evaluation, append any new record with its run-completion timestamp, seed, tested configuration, linked source or source-archive commit, and evidence. Update the confirmation table when additional seeds finish, including regressions and failures. Historical peaks remain visible even when their policy is rejected. Do not use a dirty build’s base HEAD as if it were the exact tested source. Require six seeds before claiming the project’s repeatability goal.
