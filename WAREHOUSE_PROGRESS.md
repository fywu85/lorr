# WAREHOUSE throughput progress

Updated: 2026-09-19 12:15:34 UTC.

**Best observed full run: 134,519 completed tasks** — refresh interval 512, seed 2. The same policy reaches **134,511** on seed 0, for a two-seed mean of **134,515**. It recovers the previously failing seed; six-seed confirmation is queued. **Best mean confirmed over three seeds remains 109,225** — no-flow fixed 4M (109,244 / 109,249 / 109,182).

The local KittyKnight reference is **152,981**, so the experimental peak is **12.07% below** it. That reference used 38.858 GB RSS; our limit is 32,000,000,000 bytes per planner. This is a local comparison, not an official or equal-resource SoTA claim.

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

† Some early binaries were built between commits. These links identify the commit containing their **frozen source snapshot/patch**, not a claim that the commit’s working tree exactly matches the binary. Unmarked commits were checked against **every recorded production-source SHA-256**. The source link and binary hash in the evidence distinguish experiments archived in the same commit.

“Legacy timing” means that the original run predates complete entry-time instrumentation. Its configured deadline, explicit timeout result, simulator validity and measured RSS are retained, but it does not supply the later per-decision timing evidence. CPU models and core counts also vary; this table tracks task completions, not runtime speedups.

## Confirmation and reversals

| Policy | Full-run evidence | Current interpretation |
|---|---|---|
| No-flow fixed 4M | Seeds 0/1/2: 109,244 / 109,249 / 109,182; mean **109,225** | Stable current three-seed control; six-seed confirmation outstanding. |
| Frozen margin-50 flow | Seeds 0/1/2: 122,896 / 122,195 / 70,171; mean **105,087.3** | Historical peak retained, but **not promoted**: seed 2 collapses late and the mean loses to its matched control. |
| Refresh every 512 | Seeds 0/2: **134,511 / 134,519**; mean **134,515**; final windows 28,215 / 28,214; max entry <=0.765267 s; RSS <11.89 GB | Both improve over frozen and no-flow controls. Six-seed paired confirmation **8898647/8898648** queued; no six-seed claim yet. |
| Refresh every 1,024 | Seeds 0/2: 133,652 / 131,316; mean 132,484 | Both recover steady final-window throughput, but interval 512 wins both full totals. |
| Frozen flow with warm reuse | Seeds 0/2: 123,373 / 123,251; mean 123,312 | Recovers seed 2 without updating the field; below refresh512 on both tested seeds. |
| Gentler frozen-flow penalties | Scale4 seeds0/2: 121,812 /119,927; scale8: 119,337 /117,897 | Both recover seed2, but lower healthy-seed throughput and remain below refresh512. |

[Matched three-seed comparison](experiments/construction-20260918/results/flow-margin-matched-controls-v20.json), [complete refresh results](experiments/construction-20260918/results/flow-refresh-full-v30/), [warm-reuse results](experiments/construction-20260918/results/flow-warm-full-v31/), [record provenance](experiments/construction-20260918/results/throughput-progress-provenance.json).

## Resource cost of the current leading configuration

Refresh512, seeds0/2, all5,000steps: mean complete scheduling+planning latency
**237.3 /256.8ms**, maximum **765.3 /762.0ms**; average CPU **1.216 /1.197cores**
with four physical cores reserved per instance (about30% average utilization).
Peak process RSS **11.838 /11.888GB**; elapsed full run **20.99 /22.65minutes**.
CPU is measured user+system time divided by process wall time, not an instantaneous
sample. Hosts were exclusive and unthrottled. Preparation uses multiple workers;
much of the remaining work is serial. Allocation does not imply four busy cores.
[Machine-readable resource evidence](experiments/construction-20260918/results/flow-refresh-resource-summary-v30.json).

## Updating this log

After each completed full evaluation, append any new record with its run-completion timestamp, seed, tested configuration, linked source or source-archive commit, and evidence. Update the confirmation table when additional seeds finish, including regressions and failures. Historical peaks remain visible even when their policy is rejected. Do not use a dirty build’s base HEAD as if it were the exact tested source. Require six seeds before claiming the project’s repeatability goal.
