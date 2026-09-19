# WAREHOUSE throughput progress

Updated: 2026-09-19 14:55:12 UTC.

**Best observed full run: 136,149 completed tasks** — direct-cost dispatch with pickup weight5, seed0. **This policy is not promoted:** seed2 collapses to52,444 despite valid decisions. The two-owner transaction pass reaches135,357, but its completed six-seed mean loses0.288%; it is also not promoted. Ordinary refresh remains the configuration confirmed on **all six seeds (0–5)**: **134,590.3 mean**, range **134,061–134,966**, with no deadline or memory failures. The throughput target remains unmet.

The local KittyKnight reference is **152,981**, so the experimental peak is **11.00% below** it. That reference used 38.858 GB RSS; our limit is 32,000,000,000 bytes per planner. This is a local comparison, not an official or equal-resource SoTA claim.

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

† Some early binaries were built between commits. These links identify the commit containing their **frozen source snapshot/patch**, not a claim that the commit’s working tree exactly matches the binary. Unmarked commits were checked against **every recorded production-source SHA-256**. The source link and binary hash in the evidence distinguish experiments archived in the same commit.

“Legacy timing” means that the original run predates complete entry-time instrumentation. Its configured deadline, explicit timeout result, simulator validity and measured RSS are retained, but it does not supply the later per-decision timing evidence. CPU models and core counts also vary; this table tracks task completions, not runtime speedups.

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
| Unopened pool exchange | Seeds0/2:135,409 /134,762, mean135,085.5 | +0.424%; final-window throughput improves, but outstanding age p90 rises to702/722. No six-seed promotion. [Evidence](experiments/construction-20260918/results/pool-exchange-full-v37/comparison.json). |

[Six-seed refresh evidence](experiments/construction-20260918/results/flow-refresh-six-seed-v30.json), [matched three-seed comparison](experiments/construction-20260918/results/flow-margin-matched-controls-v20.json), [complete refresh results](experiments/construction-20260918/results/flow-refresh-full-v30/), [warm-reuse results](experiments/construction-20260918/results/flow-warm-full-v31/), [record provenance](experiments/construction-20260918/results/throughput-progress-provenance.json).

## Resource cost of the leading configuration

Across the six-seed confirmation, mean complete scheduling plus planning latency
is **239.4–258.1 ms**, with a maximum of **776.3 ms**.
The independent seeds 0/2 latency audit measured p99 at **323.8 / 340.1 ms**.
Average CPU use is **1.195–1.211 cores** with four physical cores reserved
per instance (about 30% average utilization). Peak process RSS reaches **13.121 GB**,
and full runs take **21.19–22.75 minutes**.
CPU is process user plus system time divided by wall time, not an instantaneous
sample. Hosts were exclusive and unthrottled. Preparation uses multiple workers;
much of the remaining work is serial. A separate repeated control reached 792.9 ms,
so 776.3 ms is the maximum of this confirmation, not of every repeated run.
[Six-seed resource evidence](experiments/construction-20260918/results/flow-refresh-six-seed-v30.json),
[independent latency quantiles](experiments/construction-20260918/results/refresh-dwell-v1.json).

Four full-size search workers average 271–277 ms per decision and 2.60–2.63 CPU
cores, but add only 0.204% throughput on seeds 0/2; they are not promoted.
[Parallel-search evidence](experiments/construction-20260918/results/flow-refresh-workers-full-v33/comparison.json).

## Updating this log

After each completed full evaluation, append any new record with its run-completion timestamp, seed, tested configuration, linked source or source-archive commit, and evidence. Update the confirmation table when additional seeds finish, including regressions and failures. Historical peaks remain visible even when their policy is rejected. Do not use a dirty build’s base HEAD as if it were the exact tested source. Require six seeds before claiming the project’s repeatability goal.
