# WAREHOUSE throughput progress

Updated: 2026-09-19 18:05:49 UTC.

**Current six-seed reference: 139,387.8 mean tasks**, graded strength4/scale4 with margin25, turn-build128 and heading/traffic-aware pickup selection, range **138,671–140,002**. All six full totals and final1,000-step windows improve over the preceding graded reference (**+0.317%** in the mean); empty robot-steps fall on every seed. Outstanding agep90 rises7–20steps, with the existing fairness policy retained. Complete pickup fields improve further on the first two seeds:16fields mean140,711 (+0.998%) and64fields mean142,408.5 (+2.217%) against their matched pickup-ON control; six-seed confirmation is running. The32-field policy collapses seed0 and is rejected. The absolute high is **142,988**, a two-seed candidate result. The target remains unmet.

The local KittyKnight reference is **152,981**. The confirmed mean is **8.89% below** it. That reference used38.858GB RSS; our limit is32,000,000,000bytes per planner. This is a local comparison, not an official or equal-resource SoTA claim.

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
| 2026-09-19 17:42:42 | 141,725 | Complete pickup fields16 / 2 | [4872d04](https://github.com/fywu85/lorr/commit/4872d048dd8155927074ac2a4d27a55756266fd5) | [data](experiments/construction-20260918/results/pickup-full-full-v42/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v42/) · two seeds; confirmation running |
| 2026-09-19 17:43:24 | 142,735 | Complete pickup fields32 / 2 | [4872d04](https://github.com/fywu85/lorr/commit/4872d048dd8155927074ac2a4d27a55756266fd5) | [data](experiments/construction-20260918/results/pickup-full-full-v42/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v42/) · rejected: seed0 collapses |
| 2026-09-19 17:43:34 | 142,988 | Complete pickup fields64 / 2 | [4872d04](https://github.com/fywu85/lorr/commit/4872d048dd8155927074ac2a4d27a55756266fd5) | [data](experiments/construction-20260918/results/pickup-full-full-v42/run-summaries.json) · [source](experiments/construction-20260918/build-provenance/v42/) · two seeds; confirmation running |

† Some early binaries were built between commits. These links identify the commit containing their **frozen source snapshot/patch**, not a claim that the commit’s working tree exactly matches the binary. Unmarked commits were checked against **every recorded production-source SHA-256**. The source link and binary hash in the evidence distinguish experiments archived in the same commit.

“Legacy timing” means that the original run predates complete entry-time instrumentation. Its configured deadline, explicit timeout result, simulator validity and measured RSS are retained, but it does not supply the later per-decision timing evidence. CPU models and core counts also vary; this table tracks task completions, not runtime speedups.

## Confirmed reference milestones

Each row uses all six full warehouse seeds. Completion is the last candidate run finishing; these are policy comparisons, not runtime-speedup claims.

| Completed (UTC) | Mean tasks | Range | Policy | Exact source | Evidence |
|---|---:|---:|---|---|---|
| 2026-09-19 15:28:50 | 136,426.2 | 135,838–136,888 | Binary margin25, build32 | [45ebea2](https://github.com/fywu85/lorr/commit/45ebea2680e393a1e3ae060f29194b494bedc254) | [data](experiments/construction-20260918/results/flow-refresh-margin25-six-seed-v36.json) |
| 2026-09-19 15:40:34 | 137,073.3 | 136,841–137,271 | Binary margin50, build128 | [5c7f0fa](https://github.com/fywu85/lorr/commit/5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27) | [data](experiments/construction-20260918/results/turn-build-limit-six-seed-v39.json) |
| 2026-09-19 16:34:42 | 138,104.2 | 137,149–139,037 | Binary margin25, build128 | [5c7f0fa](https://github.com/fywu85/lorr/commit/5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27) | [data](experiments/construction-20260918/results/flow-turn-interaction-six-seed-v39.json) |
| 2026-09-19 16:47:34 | 138,948.0 | 138,304–139,716 | Graded strength4/scale4, margin25/build128 | [5c7f0fa](https://github.com/fywu85/lorr/commit/5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27) | [data](experiments/construction-20260918/results/flow-graded-six-seed-v39.json) |
| 2026-09-19 17:57:50 | 139,387.8 | 138,671–140,002 | Graded margin25/build128 + pickup flow | [eb5b382](https://github.com/fywu85/lorr/commit/eb5b3827b9daf9f91aba88c78bef7be0d17cd07c) | [data](experiments/construction-20260918/results/graded-pickup-flow-six-seed-v41.json) |

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
| Graded pickup flow | Six seeds: **138,963 /140,002 /139,677 /138,671 /139,241 /139,773**, mean **139,387.8** | **Current benchmark reference.** All six totals/finalwindows improve, mean+0.317%; empty robot-steps fall1.46–2.15%, agep90 rises7–20steps. All12runs valid; all six controls exactly reproduce. [Evidence](experiments/construction-20260918/results/graded-pickup-flow-six-seed-v41.json). |
| Complete pickup fields16 | Seeds0/2: **139,697 /141,725**, mean140,711 | +0.998% above graded pickup ON; both totals/final windows/agep90 improve. Six-seed confirmation running. [Evidence](experiments/construction-20260918/results/pickup-full-full-v42/comparison.json). |
| Complete pickup fields32 | Seeds0/2: **70,043 /142,735** | **Rejected.** Seed0 finalwindow5,433 despite valid complete decisions. Historical high retained. [Evidence](experiments/construction-20260918/results/pickup-full-full-v42/comparison.json). |
| Complete pickup fields64 | Seeds0/2: **141,829 /142,988**, mean142,408.5 | +2.217% above graded pickup ON; both totals/final windows/agep90 improve. Six-seed confirmation running. [Evidence](experiments/construction-20260918/results/pickup-full-full-v42/comparison.json). |

[Six-seed refresh evidence](experiments/construction-20260918/results/flow-refresh-six-seed-v30.json), [matched three-seed comparison](experiments/construction-20260918/results/flow-margin-matched-controls-v20.json), [complete refresh results](experiments/construction-20260918/results/flow-refresh-full-v30/), [warm-reuse results](experiments/construction-20260918/results/flow-warm-full-v31/), [record provenance](experiments/construction-20260918/results/throughput-progress-provenance.json).

## Resource cost of the current benchmark reference

The current graded pickup-flow reference averages **241.5–284.0ms** per complete
schedule+plan step, with median **210.1–262.9ms**, p99 **529.8–570.6ms**, and
maximum **872.4ms**. Whole-process CPU averages **1.171–1.204cores** of four
reserved, peak RSS **11.901GB**, and full runs take **21.36–24.91minutes**.
All30000candidate entries meet1second. CPU includes startup/simulation/logging
and is user+system seconds divided by wall time; it is not instantaneous usage.
[Six-seed confirmation](experiments/construction-20260918/results/graded-pickup-flow-six-seed-v41.json),
[all-step quantiles](experiments/construction-20260918/results/graded-pickup-reference-latency-six-seed-v41.json),
[explicit configuration](experiments/construction-20260918/warehouse-reference-variants.json).

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

The unconfirmed64-field candidate averages285.1–287.1ms, uses1.466–1.473CPU
cores, reaches11.898GB RSS, and takes24.98–25.17minutes on its two full seeds.
Its peak entry is874.2ms; these are candidate resources, distinct from the
six-seed confirmed reference above. [Candidate evidence](experiments/construction-20260918/results/pickup-full-full-v42/comparison.json).

## Updating this log

After each completed full evaluation, append any new record with its run-completion timestamp, seed, tested configuration, linked source or source-archive commit, and evidence. Update the confirmation table when additional seeds finish, including regressions and failures. Historical peaks remain visible even when their policy is rejected. Do not use a dirty build’s base HEAD as if it were the exact tested source. Require six seeds before claiming the project’s repeatability goal.
