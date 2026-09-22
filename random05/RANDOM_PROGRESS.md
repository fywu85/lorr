# All RANDOM instances: throughput progress

Started 2026-09-21T02:17:27.230561+00:00. The user expanded the campaign to RANDOM-01..05;
Current selected records are735/1408/2621/2782/4254. Fresh validation qualifies
specific frozen profiles, not every later development improvement.
Large maps are outside active development.

## Comparison rules

- Full archived horizons600/600/800/1000/2000;32 workers on16 physical EPYC9354 cores,1s entry,30s preprocessing,32GB guard.
- Headline reference: **max(NMS, Kitty Knight)** per instance. Published scores are historical; matched maxima require both teams. See [comparison policy](COMPARISON_POLICY.md).
- Throughput primary; retain completed/unfinished waiting statistics for promoted records.
- Keep general algorithm/configuration trials separate from instance-specific tricks. Every trick needs the matching `--trick RANDOM-0N`.
- Apply general variants across all five densities before claiming broad benefit. Selecting different settings by instance name counts as a trick. Geometry-independent density rules remain hypotheses until tested.
- Preserve all original failures. Selected maxima, paired planner-seed means and fresh task/start validation are distinct.
- The five cases share one layout; improvements across densities do not establish transfer to unseen geometry.
- Inputs50001–50012,50015–50020 remain excluded from tuning;50013/14 are reserved for RANDOM-04. Future fresh-input protocols must precede generation.

## Current records

| Instance | Robots | Steps | General profile | Trick profile | Matched max(NMS, KK) | Best vs matched max |
|---|---:|---:|---:|---:|---:|---:|
| RANDOM-01 | 100 | 600 | 727 | 735 | 692 | +6.21% |
| RANDOM-02 | 200 | 600 | 1397 | 1408 | 1256 | +12.10% |
| RANDOM-03 | 400 | 800 | 1634 | 2621 | 2359 | +11.11% |
| RANDOM-04 | 700 | 1000 | 1616 | 2782 | 2649 | +5.02% |
| RANDOM-05 | 800 | 2000 | 2226 | 4254 | 3172 | +34.11% |

Both local baselines are complete. Selected-best gains versus matched
max(NMS,KK) are +6.21%/+12.10%/+11.11%/+5.02%/+34.11%.
Published historical maxima688/1260/2334/2547/3050 use different inputs/hardware;
see [PILOT_PROGRESS.md](../PILOT_PROGRESS.md) for that separate comparison.
[Matched evidence](NMS_KK_COMPARISON.md).

The initial records come from the [frozen transfer](GENERALIZATION.md). The
RANDOM-05 trick record includes subsequent development; its complete history
continues in [RANDOM05_PROGRESS.md](../RANDOM05_PROGRESS.md). Configurations,
hashes and evidence are pinned in [random-frontiers.json](random-frontiers.json).

## Earlier RANDOM-05 4,000-task milestone validation

The **4,000 milestone is independently verified**. Archived record **4,011**,
source [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2), finished
2026-09-21T03:50:40.471914+00:00 and repeated exactly. It uses explicit
`--trick RANDOM-05` guidance and horizon cutoff, planner seed0, K16320/B18,
move bias3 and cutoff mix1. Mean534ms, maximum622ms, RSS559MB.

| New task/start seed | Frozen candidate | Previous3,990 configuration | NMS repetitions | Gain over stronger NMS |
|---|---:|---:|---|---:|
| 50009 | 3,912 | 3,901 | 3,095 / 3,090 | +26.40% |
| 50010 | 4,047 | 4,023 | 3,177 / 3,190 | +26.87% |

Frozen protocol [e46f57b](https://github.com/fywu85/lorr/commit/e46f57b) preceded
generation. Aggregate **+26.63% versus NMS**, and **+0.442% versus the previous
configuration**, positive on both new inputs. All eight original runs pass the
source/input/allocation/deadline/memory audit and independent full replay.
Candidate means573/560ms, maxima624/644ms, RSS below560MB. The fresh4,047
belongs to a different task stream and does not replace the archived4,011 record.

The four-core record remains separately **3,770 versus NMS4's2,914**. The
four-core V4 fresh comparison is+25.42%; it uses different inputs/configuration.
The longest completed archived-record order is1944steps; fresh candidates'
maxima are1961/1980. Every run still leaves initial orders unfinished at2000,
so eventual maximum latency remains censored.

[Milestone requirements audit](results/completion-audit-4000/audit.json),
[complete validation](FRESH_VALIDATION_V5.md),
[fresh waiting metrics](results/fresh-validation-v5/WAITING.md),
[all RANDOM-05 commit/timestamp records](../RANDOM05_PROGRESS.md).

## First declared experiments

Source79 (`9e9dbfa`, build79) runs23 full cases. Three general variants use
noise50, dispersion0, or both, identically across all five densities. They test
whether RANDOM-05 exploration and crowd-spreading settings overshoot lower
densities. These are general configuration experiments, not asserted gains.

Separately, RANDOM-01..03 get a horizon-only trick with guidance still off.
RANDOM-04 receives individual tests of contrast1.6, dispersion0.4, noise100,
move bias2 and a larger K24480 search. Other settings are frozen. Each such
run has its explicit instance flag. The RANDOM-05 move-bias experiments continue.

At this initial checkpoint a windowed multi-step optimizer had not yet been
added. The later sections document its implementation and measured gains.

## Record evidence

| Finished UTC | Instance | Profile | Tasks | Source | Evidence |
|---|---|---|---:|---|---|
| 2026-09-21T01:23:43.251178+00:00 | RANDOM-01 | general | 647 | [233f5bf](https://github.com/fywu85/lorr/commit/233f5bf) | [Full run](results/generalization-random-generic-split-full-v69/pipeline-generic-random-01/summary.json) |
| 2026-09-21T01:24:20.856209+00:00 | RANDOM-02 | general | 1079 | [233f5bf](https://github.com/fywu85/lorr/commit/233f5bf) | [Full run](results/generalization-random-generic-split-full-v69/pipeline-generic-random-02/summary.json) |
| 2026-09-21T01:26:22.425368+00:00 | RANDOM-03 | general | 1582 | [233f5bf](https://github.com/fywu85/lorr/commit/233f5bf) | [Full run](results/generalization-random-generic-split-full-v69/pipeline-generic-random-03/summary.json) |
| 2026-09-21T01:30:14.383662+00:00 | RANDOM-04 | general | 1558 | [233f5bf](https://github.com/fywu85/lorr/commit/233f5bf) | [Full run](results/generalization-random-generic-split-full-v69/pipeline-generic-random-04/summary.json) |
| 2026-09-21T01:30:30.707793+00:00 | RANDOM-01 | trick | 592 | [6b40cd5](https://github.com/fywu85/lorr/commit/6b40cd5) | [Full run](results/generalization-random-frozen-trick-split-full-v78/pipeline-frozen-trick-random-01/summary.json) |
| 2026-09-21T01:31:00.205776+00:00 | RANDOM-02 | trick | 1122 | [6b40cd5](https://github.com/fywu85/lorr/commit/6b40cd5) | [Full run](results/generalization-random-frozen-trick-split-full-v78/pipeline-frozen-trick-random-02/summary.json) |
| 2026-09-21T01:32:52.388346+00:00 | RANDOM-03 | trick | 2171 | [6b40cd5](https://github.com/fywu85/lorr/commit/6b40cd5) | [Full run](results/generalization-random-frozen-trick-split-full-v78/pipeline-frozen-trick-random-03/summary.json) |
| 2026-09-21T01:36:45.596504+00:00 | RANDOM-04 | trick | 2456 | [6b40cd5](https://github.com/fywu85/lorr/commit/6b40cd5) | [Full run](results/generalization-random-frozen-trick-split-full-v78/pipeline-frozen-trick-random-04/summary.json) |
| 2026-09-21T01:46:49.544530+00:00 | RANDOM-05 | general | 2226 | [233f5bf](https://github.com/fywu85/lorr/commit/233f5bf) | [Full run](results/generalization-random-generic-split-full-v69/pipeline-generic-random-05/summary.json) |
| 2026-09-21T01:47:52.619760+00:00 | RANDOM-05 | trick | 3928 | [6b40cd5](https://github.com/fywu85/lorr/commit/6b40cd5) | [Full run](results/generalization-random-frozen-trick-split-full-v78/pipeline-frozen-trick-random-05/summary.json) |
| 2026-09-21T01:50:46.795715+00:00 | RANDOM-05 | trick | 3941 | [acdbfd7](https://github.com/fywu85/lorr/commit/acdbfd7) | [Full run](results/move-bias-followup-split-full-v77/32-move-bias3-b18-seed5/summary.json) |
| 2026-09-21T02:16:03.845916+00:00 | RANDOM-05 | trick | 3978 | [acdbfd7](https://github.com/fywu85/lorr/commit/acdbfd7) | [Full run](results/move-bias-refinement-split-full-v77/32-move-bias3-b18-seed0/summary.json) |
| 2026-09-21T02:18:11.721419+00:00 | RANDOM-01 | trick | 628 | [9e9dbfa](https://github.com/fywu85/lorr/commit/9e9dbfa) | [Full run](results/random-density-first-split-full-v79/trick-random-01-horizon-only/summary.json) |
| 2026-09-21T02:18:46.354333+00:00 | RANDOM-02 | general | 1106 | [9e9dbfa](https://github.com/fywu85/lorr/commit/9e9dbfa) | [Full run](results/random-density-first-split-full-v79/general-random-02-dispersion0/summary.json) |
| 2026-09-21T02:24:01.656278+00:00 | RANDOM-01 | general | 651 | [9e9dbfa](https://github.com/fywu85/lorr/commit/9e9dbfa) | [Full run](results/random-operations-split-full-v79/general-random-01-operations-k512/summary.json) |
| 2026-09-21T02:27:08.813026+00:00 | RANDOM-04 | trick | 2462 | [9e9dbfa](https://github.com/fywu85/lorr/commit/9e9dbfa) | [Full run](results/random-density-first-split-full-v79/trick-random-04-k24480/summary.json) |
| 2026-09-21T02:43:54.755684+00:00 | RANDOM-01 | general | 664 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-split-full-v80/general-random-01-window15-iters24/summary.json) |
| 2026-09-21T02:43:55.001681+00:00 | RANDOM-01 | general | 685 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-split-full-v80/general-random-01-window15-unit-cost/summary.json) |
| 2026-09-21T02:46:48.999994+00:00 | RANDOM-01 | general | 699 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-work-split-full-v80/general-random-01-window-iters128/summary.json) |
| 2026-09-21T02:46:51.882986+00:00 | RANDOM-02 | general | 1172 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-work-split-full-v80/general-random-02-window-iters128/summary.json) |
| 2026-09-21T02:47:02.214642+00:00 | RANDOM-01 | general | 714 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-work-split-full-v80/general-random-01-window-cost2-iters512/summary.json) |
| 2026-09-21T02:47:12.974653+00:00 | RANDOM-02 | general | 1314 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-work-split-full-v80/general-random-02-window-cost2-iters512/summary.json) |
| 2026-09-21T02:47:18.203032+00:00 | RANDOM-01 | general | 718 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-work-split-full-v80/general-random-01-window-h20-cost2-iters512/summary.json) |
| 2026-09-21T02:53:12.803104+00:00 | RANDOM-02 | general | 1323 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-followup-split-full-v80/general-random-02-window-selected-seed0/summary.json) |
| 2026-09-21T02:53:13.881987+00:00 | RANDOM-02 | general | 1325 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-followup-split-full-v80/general-random-02-window-selected-seed4/summary.json) |
| 2026-09-21T02:53:18.408890+00:00 | RANDOM-01 | general | 726 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-followup-split-full-v80/general-random-01-window-selected-seed4/summary.json) |
| 2026-09-21T02:54:29.136533+00:00 | RANDOM-02 | general | 1351 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-followup-split-full-v80/general-random-02-window-h15-iters2048/summary.json) |
| 2026-09-21T02:56:07.783992+00:00 | RANDOM-02 | general | 1370 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-window-followup-split-full-v80/general-random-02-window-h20-iters2048/summary.json) |
| 2026-09-21T03:06:09.957953+00:00 | RANDOM-05 | trick | 3985 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/move-bias-seed0-split-full-v80/32-move-bias3-seed0-k20160/summary.json) |
| 2026-09-21T03:05:40.755316+00:00 | RANDOM-04 | trick | 2486 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random04-search-split-full-v80/trick-random-04-depth10/summary.json) |
| 2026-09-21T03:10:39.990229+00:00 | RANDOM-03 | trick | 2182 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random-crowded-seeds-split-full-v80/trick-random-03-selected-seed6/summary.json) |
| 2026-09-21T03:26:06.798792+00:00 | RANDOM-05 | trick | 3990 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/record3978-triage-split-full-v80/32-record3978-triage-mix0.75/summary.json) |
| 2026-09-21T03:50:40.471914+00:00 | RANDOM-05 | trick | 4011 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/record3990-coupling-split-full-v80/32-record3990-mix1/summary.json) |
| 2026-09-21T03:52:44.850465+00:00 | RANDOM-03 | trick | 2343 | [4b70a80](https://github.com/fywu85/lorr/commit/4b70a80) | [Full run](results/random03-window-work-split-full-v81/trick-random-03-window-keep10-iters2048/summary.json) |
| 2026-09-21T03:57:59.092064+00:00 | RANDOM-04 | trick | 2500 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random04-depth-coupling-split-full-v80/trick-random-04-depth12/summary.json) |
| 2026-09-21T04:00:21.635220+00:00 | RANDOM-03 | trick | 2354 | [4b70a80](https://github.com/fywu85/lorr/commit/4b70a80) | [Full run](results/random-window2343-followup-split-full-v81/trick-random-03-window2343-horizon15/summary.json) |
| 2026-09-21T04:01:36.001191+00:00 | RANDOM-02 | general | 1376 | [4b70a80](https://github.com/fywu85/lorr/commit/4b70a80) | [Full run](results/random-window2343-followup-split-full-v81/general-random-02-window-h20-i2048-keep10/summary.json) |
| 2026-09-21T04:01:59.631782+00:00 | RANDOM-03 | trick | 2439 | [4b70a80](https://github.com/fywu85/lorr/commit/4b70a80) | [Full run](results/random-window2343-followup-split-full-v81/trick-random-03-window2343-keep14/summary.json) |
| 2026-09-21T04:10:48.680394+00:00 | RANDOM-04 | trick | 2505 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random04-depth12-followup-split-full-v80/trick-random-04-depth12-seed0/summary.json) |
| 2026-09-21T04:11:29.934441+00:00 | RANDOM-04 | trick | 2516 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random04-depth12-followup-split-full-v80/trick-random-04-depth12-seed3/summary.json) |
| 2026-09-21T04:28:57.667303+00:00 | RANDOM-04 | trick | 2551 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random04-field-depth-split-full-v80/trick-random-04-depth16-k14400/summary.json) |
| 2026-09-21T05:05:35.466739+00:00 | RANDOM-04 | trick | 2565 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | [Full run](results/random04-depth16-validation-split-full-v80/trick-random-04-depth16-k14400-seed4/summary.json) |
| 2026-09-21T05:30:41.179219+00:00 | RANDOM-03 | trick | 2466 | [f60f254](https://github.com/fywu85/lorr/commit/f60f254) | [Full run](results/random03-window-budget-split-full-v86/trick-random-03-window-budget-group4-iterations4096/summary.json) |
| 2026-09-21T05:33:41.589621+00:00 | RANDOM-03 | trick | 2469 | [f60f254](https://github.com/fywu85/lorr/commit/f60f254) | [Full run](results/random03-window-budget-split-full-v86/trick-random-03-window-budget-iterations4096/summary.json) |
| 2026-09-21T05:33:47.702672+00:00 | RANDOM-03 | trick | 2479 | [f60f254](https://github.com/fywu85/lorr/commit/f60f254) | [Full run](results/random03-window-budget-split-full-v86/trick-random-03-window-budget-group4-iterations8192/summary.json) |
| 2026-09-21T05:47:24.158096+00:00 | RANDOM-03 | trick | 2483 | [49f9724](https://github.com/fywu85/lorr/commit/49f9724) | [Full run](results/random03-cooperative-split-full-v88/trick-random-03-cooperative-rounds4/summary.json) |
| 2026-09-21T05:47:36.178484+00:00 | RANDOM-03 | trick | 2514 | [49f9724](https://github.com/fywu85/lorr/commit/49f9724) | [Full run](results/random03-cooperative-split-full-v88/trick-random-03-cooperative-rounds2/summary.json) |
| 2026-09-21T06:01:02.811463+00:00 | RANDOM-03 | trick | 2545 | [dabfcc7](https://github.com/fywu85/lorr/commit/dabfcc7) | [Full run](results/random03-cooperative-triage-split-full-v89/trick-random-03-cooperative-triage-triage100-guided/summary.json) |
| 2026-09-21T06:01:05.809867+00:00 | RANDOM-03 | trick | 2551 | [dabfcc7](https://github.com/fywu85/lorr/commit/dabfcc7) | [Full run](results/random03-cooperative-triage-split-full-v89/trick-random-03-cooperative-triage-triage100/summary.json) |
| 2026-09-21T06:14:53.457614+00:00 | RANDOM-03 | trick | 2552 | [de0b763](https://github.com/fywu85/lorr/commit/de0b763) | [Full run](results/random03-anneal-split-full-v91/trick-random-03-anneal-temperature4/summary.json) |
| 2026-09-21T06:18:03.840528+00:00 | RANDOM-04 | trick | 2634 | [ac5cbc9](https://github.com/fywu85/lorr/commit/ac5cbc9) | [Full run](results/random04-forecast-split-full-v90/trick-random-04-forecast-mutation01/summary.json) |
| 2026-09-21T07:11:31.028236+00:00 | RANDOM-04 | trick | 2641 | [fa98fd1d](https://github.com/fywu85/lorr/commit/fa98fd1d) | [Full run](results/random04-matching-refine-split-full-v96/trick-random-04-matching-refine-keep0/summary.json) |
| 2026-09-21T07:18:46.167813+00:00 | RANDOM-03 | trick | 2562 | [db2d9bc5](https://github.com/fywu85/lorr/commit/db2d9bc5) | [Full run](results/random03-window-merge-split-full-v99/trick-random-03-window-merge-merge4/summary.json) |
| 2026-09-21T07:19:24.927145+00:00 | RANDOM-03 | trick | 2580 | [db2d9bc5](https://github.com/fywu85/lorr/commit/db2d9bc5) | [Full run](results/random03-window-merge-split-full-v99/trick-random-03-window-merge-merge6/summary.json) |
| 2026-09-21T07:48:41.498854+00:00 | RANDOM-03 | trick | 2602 | [02bccaa7](https://github.com/fywu85/lorr/commit/02bccaa7) | [Full run](results/random03-merge-coupling-split-full-v102/trick-random-03-merge-coupling-triage0875/summary.json) |
| 2026-09-21T08:24:36.112951+00:00 | RANDOM-04 | trick | 2661 | [fa98fd1d](https://github.com/fywu85/lorr/commit/fa98fd1d) | [Full run](results/random04-triage-refine-split-full-v96/trick-random-04-triage-refine-mix075/summary.json) |
| 2026-09-21T09:40:28.766893+00:00 | RANDOM-04 | trick | 2698 | [aa171530](https://github.com/fywu85/lorr/commit/aa171530) | [Full run](results/random04-admission-split-full-v116/trick-random-04-admission-cap560/summary.json) |
| 2026-09-21T09:58:17.479292+00:00 | RANDOM-04 | trick | 2701 | [aa171530](https://github.com/fywu85/lorr/commit/aa171530) | [Full run](results/random04-admission-refine-split-full-v116/trick-random-04-admission-refine-cap580/summary.json) |
| 2026-09-21T09:58:25.614128+00:00 | RANDOM-04 | trick | 2738 | [aa171530](https://github.com/fywu85/lorr/commit/aa171530) | [Full run](results/random04-admission-refine-split-full-v116/trick-random-04-admission-refine-triage1/summary.json) |
| 2026-09-21T10:14:35.787464+00:00 | RANDOM-04 | trick | 2741 | [aa171530](https://github.com/fywu85/lorr/commit/aa171530) | [Full run](results/random04-active-cutoff-split-full-v116/trick-random-04-active-cutoff-scalep875/summary.json) |
| 2026-09-21T10:34:13.678386+00:00 | RANDOM-04 | trick | 2762 | [487a35da](https://github.com/fywu85/lorr/commit/487a35da) | [Full run](results/random04-admission-coupling-split-full-v119/trick-random-04-admission-coupling-dispersion04/summary.json) |
| 2026-09-21T10:43:18.044735+00:00 | RANDOM-04 | trick | 2772 | [487a35da](https://github.com/fywu85/lorr/commit/487a35da) | [Full run](results/random04-admission-dispersion-split-full-v119/trick-random-04-admission-dispersion-seed5/summary.json) |
| 2026-09-21T10:45:33.558405+00:00 | RANDOM-04 | trick | 2776 | [487a35da](https://github.com/fywu85/lorr/commit/487a35da) | [Full run](results/random04-admission-guidance-split-full-v119/trick-random-04-admission-guidance-contrast2p2/summary.json) |
| 2026-09-21T11:26:13.386904+00:00 | RANDOM-04 | trick | 2777 | [caedcce7](https://github.com/fywu85/lorr/commit/caedcce7) | [Full run](results/random04-triaged-credit-split-full-v124/trick-random-04-triaged-credit-horizon16-price32/summary.json) |
| 2026-09-21T15:05:17.296885+00:00 | RANDOM-01 | trick | 698 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random12-resume-horizon-split-full-v132/trick-random-01-resume-horizon-1p25/summary.json) |
| 2026-09-21T15:05:18.281354+00:00 | RANDOM-01 | trick | 726 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random12-resume-horizon-split-full-v132/trick-random-01-resume-horizon-1/summary.json) |
| 2026-09-21T15:05:19.829832+00:00 | RANDOM-01 | trick | 729 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random12-resume-horizon-split-full-v132/trick-random-01-resume-horizon-0p75/summary.json) |
| 2026-09-21T15:07:45.778090+00:00 | RANDOM-02 | trick | 1383 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random12-resume-horizon-split-full-v132/trick-random-02-resume-horizon-1/summary.json) |
| 2026-09-21T15:07:51.607813+00:00 | RANDOM-02 | trick | 1392 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random12-resume-horizon-split-full-v132/trick-random-02-resume-horizon-0p75/summary.json) |
| 2026-09-21T15:13:35.509421+00:00 | RANDOM-02 | trick | 1395 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random12-resume-scheduling-split-full-v132/trick-random-02-resume-scheduling-late4/summary.json) |
| 2026-09-21T15:20:50.685874+00:00 | RANDOM-02 | trick | 1401 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random12-resume-validation-split-full-v132/trick-random-02-resume-validation-seed2/summary.json) |
| 2026-09-21T15:23:12.914641+00:00 | RANDOM-05 | trick | 4090 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random05-resume-transfer-split-full-v132/trick-random-05-resume-cap760/summary.json) |
| 2026-09-21T15:23:22.109240+00:00 | RANDOM-05 | trick | 4175 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random05-resume-transfer-split-full-v132/trick-random-05-resume-cap680/summary.json) |
| 2026-09-21T15:54:03.792427+00:00 | RANDOM-02 | trick | 1408 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random12-resume-cutoff-split-full-v132/trick-random-02-resume-cutoff-0p875/summary.json) |
| 2026-09-21T16:00:46.717567+00:00 | RANDOM-02 | general | 1385 | [05fd4b76](https://github.com/fywu85/lorr/commit/05fd4b76) | [Full run](results/random123-window-progress-split-full-v136/general-random-02-window-progress-early/summary.json) |
| 2026-09-21T16:15:26.435917+00:00 | RANDOM-02 | general | 1390 | [05fd4b76](https://github.com/fywu85/lorr/commit/05fd4b76) | [Full run](results/random02-window-progress-qualification-split-full-v136/general-random-02-record1385-seed4/summary.json) |
| 2026-09-21T16:24:44.415657+00:00 | RANDOM-05 | trick | 4197 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random05-record4175-coupling-split-full-v132/trick-random-05-record4175-coupling-triage1/summary.json) |
| 2026-09-21T16:52:42.436655+00:00 | RANDOM-03 | trick | 2606 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random03-field-price-split-full-v132/trick-random-03-field-price-physical0p25/summary.json) |
| 2026-09-21T17:07:01.380356+00:00 | RANDOM-02 | general | 1394 | [611aa2a6](https://github.com/fywu85/lorr/commit/611aa2a6) | [Full run](results/random123-repair-orders-split-full-v141/general-random-02-repair-orders-morework/summary.json) |
| 2026-09-21T17:14:39.399476+00:00 | RANDOM-02 | general | 1397 | [611aa2a6](https://github.com/fywu85/lorr/commit/611aa2a6) | [Full run](results/random02-repair1394-comparison-split-full-v141/general-random-02-repair1394-order2-seed0/summary.json) |
| 2026-09-21T17:28:48.985827+00:00 | RANDOM-05 | trick | 4236 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | [Full run](results/random05-record4197-startup-split-full-v132/trick-random-05-record4197-startup-rankp125-steps250/summary.json) |
| 2026-09-21T17:35:08.379917+00:00 | RANDOM-04 | trick | 2778 | [88551e69](https://github.com/fywu85/lorr/commit/88551e69) | [Full run](results/random45-progress-triage-split-full-v144/trick-random-04-progress-triage-mixp5-span64/summary.json) |
| 2026-09-21T17:35:31.545879+00:00 | RANDOM-04 | trick | 2782 | [88551e69](https://github.com/fywu85/lorr/commit/88551e69) | [Full run](results/random45-progress-triage-split-full-v144/trick-random-04-progress-triage-mixp25-span32/summary.json) |
| 2026-09-21T18:11:42.053845+00:00 | RANDOM-05 | trick | 4242 | [88551e69df5b6f5ee14600dfe3a7ae8fe586783c](https://github.com/fywu85/lorr/commit/88551e69df5b6f5ee14600dfe3a7ae8fe586783c) | [Full run](results/random05-startup-progress-split-full-v144/trick-random-05-startup-progress-mixp125/summary.json) |
| 2026-09-21T19:18:56.863316+00:00 | RANDOM-03 | trick | 2614 | [1a3076420bc245d5f56839936d7814eb9087b050](https://github.com/fywu85/lorr/commit/1a3076420bc245d5f56839936d7814eb9087b050) | [Full run](results/random123-heuristic-weight-split-full-v153/trick-random-03-heuristic-weight1p2/summary.json) |
| 2026-09-21T19:27:17.690073+00:00 | RANDOM-03 | trick | 2615 | [1a3076420bc245d5f56839936d7814eb9087b050](https://github.com/fywu85/lorr/commit/1a3076420bc245d5f56839936d7814eb9087b050) | [Full run](results/random03-record2614-split-full-v153/trick-random-03-record2614-weight1p3/summary.json) |
| 2026-09-21T19:27:34.092146+00:00 | RANDOM-03 | trick | 2620 | [1a3076420bc245d5f56839936d7814eb9087b050](https://github.com/fywu85/lorr/commit/1a3076420bc245d5f56839936d7814eb9087b050) | [Full run](results/random03-record2614-split-full-v153/trick-random-03-record2614-seed3/summary.json) |
| 2026-09-21T20:04:17.853489+00:00 | RANDOM-01 | general | 727 | [353f09e6bdf2909aef1df9b26463c97f05b9497f](https://github.com/fywu85/lorr/commit/353f09e6bdf2909aef1df9b26463c97f05b9497f) | [Full run](results/random123-component-repair-split-full-v156/general-random-01-component-repair-mode2/summary.json) |
| 2026-09-21T20:31:22.937556+00:00 | RANDOM-03 | general | 1634 | [f80f7caa09906b2ee14e63e2d652c3a33103c23c](https://github.com/fywu85/lorr/commit/f80f7caa09906b2ee14e63e2d652c3a33103c23c) | [Full run](results/random12345-auction-split-full-v157/general-random-03-auction-eps0p125/summary.json) |
| 2026-09-21T20:35:51.119225+00:00 | RANDOM-04 | general | 1567 | [f80f7caa09906b2ee14e63e2d652c3a33103c23c](https://github.com/fywu85/lorr/commit/f80f7caa09906b2ee14e63e2d652c3a33103c23c) | [Full run](results/random12345-auction-split-full-v157/general-random-04-auction-eps0p01/summary.json) |
| 2026-09-21T20:35:53.375565+00:00 | RANDOM-04 | general | 1577 | [f80f7caa09906b2ee14e63e2d652c3a33103c23c](https://github.com/fywu85/lorr/commit/f80f7caa09906b2ee14e63e2d652c3a33103c23c) | [Full run](results/random12345-auction-split-full-v157/general-random-04-auction-eps0p125/summary.json) |
| 2026-09-21T21:11:09.447875+00:00 | RANDOM-04 | general | 1595 | [f80f7caa09906b2ee14e63e2d652c3a33103c23c](https://github.com/fywu85/lorr/commit/f80f7caa09906b2ee14e63e2d652c3a33103c23c) | [Full run](results/random34-general-auction-check-split-full-v157/general-random-04-auction-check-seed1-eps0p125/summary.json) |
| 2026-09-21T22:47:03.801161+00:00 | RANDOM-04 | general | 1597 | [1bd28feed8368b2a61b2fc9a08867a518b2d0c9f](https://github.com/fywu85/lorr/commit/1bd28feed8368b2a61b2fc9a08867a518b2d0c9f) | [Full run](results/random12345-face-cycles-split-full-v158/general-random-04-face-cycles16/summary.json) |
| 2026-09-21T22:49:58.306922+00:00 | RANDOM-04 | general | 1616 | [1bd28feed8368b2a61b2fc9a08867a518b2d0c9f](https://github.com/fywu85/lorr/commit/1bd28feed8368b2a61b2fc9a08867a518b2d0c9f) | [Full run](results/random12345-face-cycles-split-full-v158/general-random-04-face-cycles8/summary.json) |
| 2026-09-21T23:30:21.892751+00:00 | RANDOM-01 | trick | 732 | [4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad](https://github.com/fywu85/lorr/commit/4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad) | [Full run](results/random12345-physical-deadline-split-full-v162/trick-random-01-physical-deadline1/summary.json) |
| 2026-09-21T23:35:12.917204+00:00 | RANDOM-01 | trick | 733 | [4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad](https://github.com/fywu85/lorr/commit/4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad) | [Full run](results/random01-record732-split-full-v162/trick-random-01-record732-seed1-feasible1/summary.json) |
| 2026-09-21T23:36:22.952126+00:00 | RANDOM-03 | trick | 2621 | [4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad](https://github.com/fywu85/lorr/commit/4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad) | [Full run](results/random12345-physical-deadline-split-full-v162/trick-random-03-physical-deadline1/summary.json) |
| 2026-09-21T23:45:46.330230+00:00 | RANDOM-01 | trick | 735 | [4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad](https://github.com/fywu85/lorr/commit/4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad) | [Full run](results/random12-anneal-split-full-v162/trick-random-01-anneal-temp0p25/summary.json) |
| 2026-09-22T00:21:04.224019+00:00 | RANDOM-05 | trick | 4254 | [4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad](https://github.com/fywu85/lorr/commit/4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad) | [Full run](results/random345-flow-iterations-split-full-v162/trick-random-05-flow-iterations80/summary.json) |


## September21: first development comparisons

All29 original runs are strict full-horizon successes and independently replayed.
Their source is [9e9dbfa](https://github.com/fywu85/lorr/commit/9e9dbfa), build79.
General settings are algorithmic variants, not an automatic map-to-configuration
selector. Choosing settings by instance name will require an explicit trick flag.

| General variant | RANDOM-01 | RANDOM-02 | RANDOM-03 | RANDOM-04 | RANDOM-05 |
|---|---:|---:|---:|---:|---:|
| Frozen pipeline | 647 | 1079 | 1582 | 1558 | 2226 |
| Priority noise50 | 640 | 1066 | 1577 | 1495 | 1930 |
| Dispersion0 | 646 | 1106 | 1573 | 1459 | 2024 |
| Both | 646 | 1105 | 1560 | 1444 | 1978 |
| Three-action policy, K128 | 645 | 989 | 1084 | not run | not run |
| Three-action policy, K512 | 651 | 1038 | 1186 | not run | not run |

The action-policy trial reaches651 at100robots but loses at200/400. Removing
crowd spreading improves RANDOM-02 modestly, while harming denser cases. Neither
change establishes broad improvement. A new optional rolling-window LNS planner
is under regression testing to address the lower-density routing gap.

Explicit horizon-only tricks on RANDOM-01/02/03 score628/1063/1667. The current
RANDOM-05 cutoff scale is unhelpful for the two sparse cases. On RANDOM-04,
contrast1.6/dispersion0.4/noise100/move bias2/K24480 score2267/2421/2358/2436/2462,
versus2456 for the frozen trick recipe and2580 for matched NMS. The six-task
K24480 gain is a selected result, not a replicated effect.

Source, input hashes, allocation, deadlines, RAM, action replay and completed /
unfinished order latency are stored in the
[density audit](results/random-density-first-split-full-v79/audit.json) and
[action-policy audit](results/random-operations-split-full-v79/audit.json).


## Windowed search and cutoff follow-up (declared before execution)

Source[a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) passed35.39s of regression tests. Frozen experiment files are in`experiments/random-window-full-v80.json` and`experiments/random04-triage-full-v80.json`. Window15/keep6/32islands runs8or24iterations per island at every density; RANDOM-01..03 also get unit turn/wait costs. A* has a5,000-expansion cap per repair; failed repairs preserve the full incumbent. RANDOM-04 independently tests cutoff scales0.75/1.5, cutoff off and hop-only estimation, all with its explicit trick flag. These are hypotheses, not reported improvements.


### Initial windowed results

All13 full runs are valid and independently replayed. RANDOM-01 improves to
664 with24iterations and685 with unit turn/wait costs (+5.5% against matched
NMS649). The685 run averages9.87ms per entry, maximum25.66ms, RSS285.5MB.
These are selected development records, not fresh-input or paired-seed claims.

| Window setting | RANDOM-01 | RANDOM-02 | RANDOM-03 | RANDOM-04 | RANDOM-05 |
|---|---:|---:|---:|---:|---:|
| 8iterations, turn0.6/wait0.5 | 623 | 147 | 48 | 4 | 2 |
| 24iterations, turn0.6/wait0.5 | 664 | 298 | 47 | 6 | 2 |
| 24iterations, unit costs | 685 | 823 | 162 | not run | not run |

This first windowed mode gridlocks at higher densities. It is not a replacement
for the crowded reactive solver. With substantial time headroom, the next
RANDOM-01..03 comparisons increase repairs to128/512iterations, test turn/wait2,
and extend the window to20. The same four variants run at each of these densities.
Original dense failures remain in the [full audit](results/random-window-split-full-v80/audit.json).

The first RANDOM-04 horizon-off case failed configuration validation because its
directional triage mix was still0.5. Preserve that original exit125. A separately
named `no-horizon-configfix` attempt sets the inactive mix to0; no solver fix or
outcome substitution is involved.


### Stronger window search and planner-seed checks

All12 work-scaling runs and11 follow-ups passed independent replay and strict
resource checks. Forward moves cost2 in the unguided graph; “turn/wait1” and
“turn/wait2” below refer to those two action types specifically.

| General window variant | RANDOM-01 | RANDOM-02 | RANDOM-03 |
|---|---:|---:|---:|
| H15,128iterations,turn/wait1 | 699 | 1172 | 145 |
| H15,512iterations,turn/wait1 | 710 | 1289 | 149 |
| H15,512iterations,turn/wait2 | 714 | 1314 | 1237 |
| H20,512iterations,turn/wait2 | 718 | 1314 | 1270 |
| H15,2048iterations,turn/wait2 | 717 | 1351 | not run |
| H20,2048iterations,turn/wait2 | 722 | 1370 | 1372 |

Fixed512-iteration configurations on planner seeds5/0/3/4 give718/717/714/726
on RANDOM-01 (H20,mean718.75) and1314/1323/1319/1325 on RANDOM-02
(H15,mean1320.25). These are repeated planner seeds on the same archived input,
not fresh tasks/starts. Their selected maxima are726 and1370, respectively,
versus matched NMS649/1228 (+11.9%/+11.6%). The1370 record uses2048iterations.
No horizon cutoff or map-tuned guidance is enabled in these runs.
[Audited work scaling](results/random-window-work-split-full-v80/audit.json),
[audited follow-ups](results/random-window-followup-split-full-v80/audit.json),
[exact repeated settings](results/random-window-followup-split-full-v80/repeated-configurations.json).

RANDOM-03 window guidance (H20/512iterations,cutoff off) gives1149 with simple
lanes,1797 with archived KK400 weights,1809 with flow contrast1.6 and1839 with
flow contrast2.4. All have explicit `--trick RANDOM-03`; all are below the frozen
reactive solver's2171. The unmodified KK asset and its MIT notice are retained.
Further reactive guidance experiments include an unchanged2171control, six
flow seeds and KK/NMS fields. These are declared trick searches.

RANDOM-04 cutoff scales0.75/1.5 and hop-only give2428/2452/2447 versus2456
at scale1.25/directional mix0.5. Correctly disabling the cutoff gives2298,
so it accounts for158 tasks in this pair (+6.9%). The original incompatible
configuration failure remains archived. Broader root portfolios (B4/8/10/14)
and depths6/10 are now under full evaluation with the same frozen guidance.

Source81 adds optional general blocker-based repair groups and equal-cost path
exploration. Both default off; regression passed34.67s. Its full comparisons
will include exact source80 controls before any performance promotion.


### Blocker groups, equal-cost exploration, and crowded follow-ups

All16 source81 ablations are strict/replayed; four controls exactly reproduce all
six trajectory fields from source80. AtH20/512iterations, control/blockers/equal/
both score718/723/718/718 on RANDOM-01,1314/1318/1331/1318 on RANDOM-02,
1270/1090/1291/1105 on unguided RANDOM-03, and1839/1865/1886/1910 on guided
RANDOM-03. There is no all-density gain: blockers hurt the unguided medium case.
The best guided window remains below the2171 reactive record. Keep both flags
optional. [Complete audit](results/random-window-blockers-split-full-v81/audit.json).

Reactive RANDOM-03 guidance seeds0–5 score2039/2055/2035/2078/2040/2024;
KK400 and NMS files both score2001 with different trajectories. The unchanged
2171 control is exact in all six fields. None beats the existing profile.
RANDOM-04 branch counts4/8/10/14 give2441/2384/2417/2426. Depth6 loses at2258;
depth10 gives a new audited2486 record, still3.6% below matched NMS2580.
[Reactive guidance audit](results/random03-reactive-guidance-split-full-v80/audit.json),
[RANDOM-04 search audit](results/random04-search-split-full-v80/audit.json).

A source82 initialization-portfolio prototype failed the dense-mobility regression
before any benchmark. The failed source patch, log and build metadata are retained.
Source83 tests progress-based initialization ranking against the same regression
and unchanged one-start controls. It has not been promoted.


### Additional crowded planner seeds

The frozen RANDOM-03 reactive profile on planner seeds0/3/4/6 scores
2182/2158/2157/2182, versus2171 onseed5. The earliest2182 is recorded; tied
scores are not new frontiers. RANDOM-04's K24480/depth8 profile gives
2439/2441/2431/2470 on those seeds, versus2462 onseed5. All eight full runs are
strict successes with independent replay and order-latency audits. The newer
2486 RANDOM-04 record instead usesK16320/depth10/seed5.
[Seed audit](results/random-crowded-seeds-split-full-v80/audit.json).

The revised source83 initialization portfolio passes the original dense-mobility
threshold and worker/cache/checkpoint checks (22.76s); all35compiled/test inputs
match its frozen build. Full experiments will include one-start controls. This
passes regression, not a throughput claim. Two additional RANDOM-05 runs test
the3985record's K20160 budget on planner seeds5/7; four earlier horizon variants
still use the3978/K16320 recipe. No fresh50009+ input has been generated.


### Initialization portfolio rejected; RANDOM-05 reaches3,990

All16 source83 full runs are strict and independently replayed; the four one-start
controls exactly match source81 in all six trajectory fields. Extra16/64 starts
atnoise30, or16 atnoise100, score716/717/716 on RANDOM-01 versus718;
1312/1309/1316 on RANDOM-02 versus1314;1148/1124/1144 on unguided RANDOM-03
versus1270; and916/877/1063 on guided RANDOM-03 versus1910. Keep
`R05_WINDOW_STARTS=1`. Passing safety/regression checks did not establish a
throughput gain. A possible cause is repeatedly choosing fresh pipeline seeds,
whose first action cannot move, in place of a retained plan. This remains a
hypothesis; a retained-prefix portfolio would test it.
[Audit and controls](results/random-window-starts-split-full-v83/audit.json).

RANDOM-05's explicit cutoff refinement reaches3,990 atK16320/seed0/mix0.75,
versus3,978 for its mix0.5 parent. This is5 above the intervening K20160 record.
Mean513ms,max565ms,RSS559MB; independent replay and all84frontier audits pass.
Waiting maximum among completed orders is1945steps;130 initial orders remain
unfinished and91unopened. The4,000 target remains10tasks away. Two K20160
planner-seed runs and three cutoff/budget combinations are pending.

The complete eight-seed move-bias assessment is31,103 versus30,609 (+1.61%,
five positive pairs). This supersedes the initial four-seed +2.60% estimate;
both histories are retained. The seeds include selected development cases and
are not independent task/start validation.
[All eight paired outcomes](results/move-bias-seeds-split-full-v77/paired-eight-seeds.json).

### RANDOM-05 crosses4,000; validation frozen

The audited4,011 record usesK16320/seed0/bias3 and directional cutoff mix1.
Mean534ms,max622ms,RSS559MB; +26.5%versus matched NMS3172. The exact repeat
and V5 fresh-input validation are required before declaring the goal complete.
Both retained-prefix initialization prototypes failed the unchanged crowded
mobility test (39/38 moves, required>150). Their exact patches/logs remain in
results/build-v84 and build-v85; source was restored to f217eee, with no full
benchmark on either failed build. RANDOM-03/04 controlled work continues.

### Controlled RANDOM-03/04 gains, 2026-09-21 03:58 UTC

RANDOM-03 guided window search, source4b70a80, starts at1,910 with window20,
keep6 and512iterations. Raising iterations to2048 gives2,194; keep10 alone
gives2,126; together they give **2,343**, just16below matched NMS2,359. The
best has mean282/max353ms and453MB RSS. Guidance is an explicit RANDOM-03
trick; the horizon is off. All four runs pass strict full-run replay, including
the weaker horizon-enabled1,922 result. This is a configuration interaction,
not proof that either adjustment wins at every density.
[Full audit](results/random03-window-work-split-full-v81/audit.json).

RANDOM-04 reaches **2,500** by increasing reactive look-ahead depth from10to12,
withK16320/seed5 unchanged. Mean669/max717ms, fully replayed. Increasing only
Kto20160 gives2,482; depth10 with mix0.75 gives2,486; move bias3 gives2,468.
Further depth/seed comparisons are declared separately.
[Full audit](results/random04-depth-coupling-split-full-v80/audit.json).

RANDOM-05's last two work/cutoff combinations finish3,983 (K20160/mix0.75)
and3,987 (K20160/mix1), below4,011 atK16320/mix1. All three are independently
audited. V5 validation is frozen in commit e46f57b before generation.

### RANDOM-03 exceeds NMS; another general RANDOM-02 gain

With window20 and2048iterations fixed, keeping14steps scores **2,439** on
RANDOM-03, **+3.4%** versus matched NMS2,359. The guided field remains an
explicit trick, with no known-horizon rule. Keeping10steps at window15 scores
2,354. Keeping10/window20 on seeds5/0/3/4 gives2,343/2,357/2,344/2,352
(mean2,349), so the2,439 gain is a configuration change, not merely a new
planner seed. Its repeated-seed and longer-window tests remain pending.
The same window20/2048iterations/keep10 general configuration gives720/1376
on RANDOM-01/02, compared with722/1370 for keep6 on the same planner seed5.
The RANDOM-02 record is **1,376 (+12.1% versus NMS1,228)**. On unguided
RANDOM-03 this common setting gives1,253, below the previous general1,582;
do not claim a universal benefit. More islands at keep10 give2,351, also
below keep14's2,439, despite using more compute.
[Full comparisons](results/random-window2343-followup-split-full-v81/audit.json).

### Confirmation and high-density transfer

Keeping14 rather than10 steps improves RANDOM-03 on all three paired planner
seeds5/0/3:2439/2415/2420 versus2343/2357/2344. The aggregate gain is3.27%;
input/binary hashes match and retained length is the only environment change.
This includes the selection seed, so it remains a development comparison.
[Paired evidence](results/random03-window2439-followup-split-full-v81/paired-keep-lengths.json).
Keep18/H20, keep18/H24 and keep14/islands64 give2436/2439/2435; none raises
the selected record. All five follow-ups independently replay.

The same window profile remains weaker at high density: guided512/2048 repairs
give1383/1425 on RANDOM-04 and2034/2110 on RANDOM-05. Unguided2048 gives995/1527.
All six are full strict successes with independent replay. They support keeping
the reactive planner for crowded cases; increasing repair work has not closed
that architectural gap.
[Transfer audit](results/crowded-window-transfer-split-full-v81/audit.json).

Reactive depth12 on RANDOM-04 scores2500/2505/2516 on seeds5/0/3. The selected
**2516** remains64below NMS2580. Depth14 gives2487; depth12/mix1 gives2470.
Four explicitly flagged guidance-pattern trials and a bounded depth16/K14400
comparison are now declared separately. No new source change is involved.

RANDOM-05 repeats4011exactly. Fresh frozen candidate/baseline results are
3912/3901 and4047/4023; NMS and the final protocol audit remain pending.

### 2026-09-21: frozen RANDOM-05 validation passed; RANDOM-04 reaches2,551

All eight V5 attempts are complete and independently replayed. The frozen
candidate beats the stronger NMS repeat on each input and the previous
configuration in aggregate, satisfying the declared criteria. See the current
RANDOM-05 section above for the full comparison and resource/waiting metrics.

RANDOM-04 reaches **2,551** atdepth16/K14400/seed5, mean831/max902ms and436MB
RSS, with all1,000steps independently replayed. NMS remains29tasks ahead. Four
alternate guidance-pattern seeds0–3 atdepth12 score2304/2302/2395/2361, below
the existing field; all attempts remain recorded. The new depth16 follow-ups
test loop gain thresholds2/3, scheduling length weight0.5, and rank-weighted
look-ahead0.25, all explicitly flagged RANDOM-04 tricks. The R05 candidate is
unchanged.
[Field/depth audit](results/random04-field-depth-split-full-v80/audit.json).

## September21 04:51UTC: RANDOM-04 follow-up and declared next batch

The four depth16 follow-ups finish2523/2526/2546/2531 tasks for loop
threshold2/3, scheduler length0.5 and rank power0.25. All runs complete strictly;
all four also pass independent replay. None exceeds the audited 2,551 record.
The candidate remains unchanged.

The next batch freezes four additional planner seeds0/3/4/6 for the selected
depth16/K14400 configuration, and depth18/K13056 plus depth20/K12240 atseed5.
This tests seed sensitivity and longer look-ahead under a similar work budget.
It is development on the archived input, not fresh-input validation. Every run
uses explicit `--trick RANDOM-04`, sourcea2ff2b2 and the full1000-step horizon.
The completed RANDOM-05 fresh comparison remains frozen.

### 2026-09-21 04:56 UTC: final 4,000-task milestone audit

All ten requirement checks pass: archived record and exact repeat, frozen
fresh comparisons, full independent replay, actual input/trace consistency,
source and test provenance, strict timing and resources, explicit trick gates,
separate four-core records, waiting metrics, and preservation of failed attempts
and held-out inputs. The audit verifies all 85 timestamped frontier rows against
the already audited history. Each of the ten selected record/repeat/fresh runs
has 2,000 timing samples; NMS uses its native `plannerTimes` field and our
simulator uses `entryComputeTimes`.

[Final requirement matrix](results/completion-audit-4000/audit.json),
[reproducible audit](tools/audit_4000_milestone.py),
[completed RANDOM-04 follow-ups](results/random04-throughput-followup-split-full-v80/audit.json).

### 2026-09-21 05:03 UTC: PILOT naming and timing failures

The family is now named **PILOT**, Pipelined Look-ahead with Task matching.
The user's authoritative RANDOM-05 history remains root `RANDOM05_PROGRESS.md`;
the earlier request for this internal density log was a typo. New
[`PILOT_PROGRESS.md`](../PILOT_PROGRESS.md) provides the general competition
overview, including five non-RANDOM placeholders, matched local NMS comparisons,
and source/timestamp links. This file retains the detailed density experiments.

Three original RANDOM-04 follow-ups fail: depth16 seed0 times out at step1
(1197ms), seed3 at step217 (1007ms), and depth20/K12240 is rejected during
initialization because the work count is not divisible into complete screening
groups (K must be a multiple of96 here). These are retained unsuccessful attempts
and do not change the frontier. The other three runs remain pending. Timing
variation is observed, but a host or algorithmic cause has not been established.

### 2026-09-21 05:09 UTC: all current runs closed; RANDOM-04 reaches 2,565

The full depth16/K14400 run on planner seed4 reaches **2,565 tasks**, 15 below
matched local NMS2,580. Mean832ms, maximum949ms, RSS440MB; sourcea2ff2b2.
It passes source/input/resource checks and independent movement, assignment,
task-event and waiting audits. Seed6 gives2,536; depth18/K13056/seed5 gives2,525.
The two deadline failures and invalid depth20 configuration remain retained.
All six original attempts are terminal and audited; no active job remains in
this batch. This gain is a selected planner seed, not fresh-input validation.

The new record is reflected in the manifest and root PILOT_PROGRESS.md. The
registered RANDOM-05 4,000-task goal was marked complete after its final evidence
and dashboard were committed and pushed in a87f097. The primary RANDOM-05 log
remains root RANDOM05_PROGRESS.md.
[Full follow-up audit](results/random04-depth16-validation-split-full-v80/audit.json).

### 2026-09-21 05:56 UTC: cooperative search reaches RANDOM-03 2,514

Fixed two-round sharing across32 LNS islands reaches2514 (+6.57% versus
matchedNMS2359), mean544/max714ms; full800steps, all source/resource/input,
independent replay and waiting checks pass. Source[49f9724](https://github.com/fywu85/lorr/commit/49f9724).
It improves2479 at the same8192 repairs/island. Four/eight rounds give2483/2469.
The exact independent/storage-reuse controls both reproduce2479 in all six
trace fields. [Full cooperative audit](results/random03-cooperative-split-full-v88/audit.json).

R04 lowerworkB10/K8064 yields2501 withmean506/max575ms; no throughput record,
but useful timing headroom. More aggressiveK14400 still has timeout attempts,
so the2565frontier must not be called robust. [Cheaper-search audit](results/random04-cheaper-search-split-full-v80/audit.json).
The new campaign still needs2595/2838, exact repeats, three-seed timing checks
and newly frozen fresh-input validation. Held-out50001–10 remain excluded.

### 2026-09-21 06:13 UTC: RANDOM-03 2,551, runtime work continues

Adding explicit horizon800/triagescale1 to the cooperative2514profile gives
2551 (+8.14% overmatchedNMS2359), mean520/max698ms, full800steps and
independent replay. Source[dabfcc7](https://github.com/fywu85/lorr/commit/dabfcc7).
[All six cutoff outcomes](results/random03-cooperative-triage-split-full-v89/audit.json).
The dashboard now derives each selected horizon/guidance assumption from its
manifest, so a new horizon-enabled record cannot inherit stale horizon-off prose.

R04 routing/cost changes remain below2565. Compact-cache and thread-binding
controls, fixed-work annealed R03repair, and targeted R04forecast/layout checks
are running. No new robust-timing or ten-percent qualification is claimed.

### 2026-09-21 06:49 UTC: RANDOM-04 2,634 and exact runtime improvement

Rootmutation0.1 reaches2634 onR04 (+2.09% vsNMS2580), source[ac5cbc9](https://github.com/fywu85/lorr/commit/ac5cbc9).
Mean495/max532ms. Same configuration on plannerseeds0/3 gives2584/2530;
these qualify its archived timing across three seeds, not a10%throughput claim.
The source[b4b4abe](https://github.com/fywu85/lorr/commit/b4b4abe) shared immutable
cache preserves the entire2634trajectory atmean447/max485ms with1.56GB RSS
(2048MiB cache cap), versus489/540ms for its exact source92control.
[Full equivalence](results/random04-shared-rankings-split-full-v92/control-equivalence.json),
[replay audit](results/random04-shared-rankings-split-full-v92/audit.json).

R03's selected maximum is2552 atannealingtemperature4, onlyone task above2551;
no substantial annealing gain is established. The2551configuration repeats
exactly across source revisions and remains below1s on plannerseeds0/3/5.
Larger budgets/windows still have preserved timeout attempts; startup work
schedules and exact A* optimizations are being evaluated.
[Runtime quantiles and qualification limits](results/random34-runtime/REPORT.md).

## Resumed all-density campaign, 2026-09-21 15:03 UTC

The user resumed work and requested maximum throughput on RANDOM-01 through
RANDOM-05. Continue general improvements and explicitly flagged tricks, keeping
strict1s steps, full horizons,32GB and bound32-worker allocations. The archived
726/1376/2602/2777/4011 records and all fresh-input evidence are preserved.
The existing RANDOM-03/04 qualification remains a milestone, not a stopping cap.

The first declared wave uses frozen source027df4d9/build132 (regression38.30s),
with five unchanged best-profile controls. Eight sparse-instance cases test
exact storage/heap settings, more complete window repairs, smaller neighborhoods,
and cooperative merging. Six separate horizon-only tricks test cutoff0.75/1/1.25
on the strongest windowed RANDOM-01/02 configurations. These do not inherit the
older reactive planner's negative horizon results as an assumption.

RANDOM-03 separately tests8192repairs, and a24-step window with16 retained steps
at5120repairs. RANDOM-04 compares6/14/18common futures against10, preserving the
complete declared work atK8064/8000/8064. RANDOM-05 tests active-order caps600/680/760
(all robots stay movable), and dispersion0.4. None is a performance claim.
The28full runs are independent GRID jobs, selected before outcomes. No held-out
input is used and no map-specific behavior bypasses its explicit trick flag.

### Sparse scheduling follow-up declared before outcomes

The original726-task RANDOM-01 trace spends1118of60000robot-steps waiting
(1.86%); completed orders incur1224extra loaded forward cells above the unweighted
shortest-path bound. More window repairs alone has not improved its first results.
RANDOM-02 has4994waits of120000steps (4.16%) and4970completed loaded detour cells.
These are observations, not fully recoverable-throughput estimates.

Twelve further strict full source132 cases keep the horizon0.75 windowed
configuration on RANDOM-01/02. Each density tests matching length weights0/0.5/1/2,
and late-horizon pairing weights4/16 with optional idle price32. This tests task
selection and end-of-run feasibility independently of movement work. All remain
explicit tricks. No new independent input or geometry is used.

### Pairing model and first-step work follow-ups

Eight full source132 cases keep the horizon0.75 windowed profile on RANDOM-01/02
and separately test oriented whole-chain matching, predicted positions, or keep
bonus0/2. These are existing bounded mechanisms, not new source changes. They
retain the explicit horizon trick and default task-length weight0.25.

Source133's one-proposal16-component repaired run exceeded the real first-step
deadline:1075.667ms, including514.920ms assignment and452.253ms look-ahead.
The original failure is retained without attributing a host cause. A separate
full case declares FIRST_K2048 instead of4032, with unchanged laterK8064; every
step still completes all its declared work or fails. Other component cases continue.

### First audited resumed records and declared planner-seed checks

The full replay audit confirms RANDOM-01=729 with horizon600/cutoff0.75,
mean64.557ms/max100.085ms, and RANDOM-02=1395 after adding late pairing weight4
and optional idle price32. These are selected individual trick records; the
original general records726/1376 stay separate. All six initial horizon cases
and twelve scheduling follow-ups pass strict full-run replay. Other matching
length weights lose. Exact trace comparisons confirm unchanged726/1376controls
and their fast-storage/heap versions; equal1376 group4/cooperative variants
follow different trajectories.

Fourteen new full runs declare an exact repeat plus six other planner seeds
per selected profile. RANDOM-01 seeds0/1/2/3/5/6 supplement selected4;
RANDOM-02 seeds0/1/2/3/4/6 supplement selected5. This is planner-seed selection
and timing qualification on the same archived input, not fresh-input validation.

All eight sparse general trials are audited. RANDOM-01 more-work/group4/cooperative
scores720/721/722, versus exact726. RANDOM-02 group4/cooperative both1376;
its4096-repair/group8 run fails at step348,1037.377ms/exit124. No partial score.
RANDOM-03's8192-repair and24-step-window cases give2578/2533; RANDOM-04's6/14/18
future cases give2691/2732/2763. All five latter runs replay and pass strict timing,
but none replaces2602/2777. Preserve all failures and lower-scoring alternatives.

### RANDOM-03 guidance follow-up declared before outcomes

The selected2602trace completes129071shortest-path loaded cells but travels
187609loaded forward cells on those completed orders (58538detour cells,45.35%).
Congestion avoidance and turns make this an observational route gap, not a
recoverable upper bound. More repairs and a longer window just lost, so nine
full source132 cases test routing with the same mature window solver:
flow seeds1/7/15/19/31 with no local edge flips; opposing-direction contrasts
0.6/1.2/1.8 on the selected field; and one general uniform-cost, horizon-off
case. The15/no-flip case distinguishes layout choice from the selected edge
mutation. All tuned guidance/horizon cases use --trick RANDOM-03. No test input
50001–50012 is accessed. Full1000ms limits and800-step runs remain mandatory.

### RANDOM-05 admission transfer reaches4,175

Caps600/680/760 give4104/4175/4090, all independently replayed and strict-valid.
The680cap record has mean559.233/max872.171ms and peakRSS491.549MB. The4,090 and
4,175 chronological records are added to RANDOM05_PROGRESS.md and both best
manifests. Dispersion0.4 loses at3878. Exact repeats and additional planner seeds
are next; no fresh-input performance is attributed to this new configuration.

### Lower-density admission transfer and continued crowded checks

Seven full source132 cases test the active-order mechanism below the crowded
regime. RANDOM-01 caps80/90 and RANDOM-02 caps160/180 apply only for the first150
steps, then restore full admission. RANDOM-03 tests persistent caps340/360/380.
All robots remain movable and opened assignments remain protected; each case
uses its explicit instance trick flag. These are new density-transfer hypotheses,
not assumed gains from the successful RANDOM-04/05 results.

The4,175profile receives an exact repeat and seeds1/2/3, nearby caps640/660/700/720,
and a separately declared FIRST_K6144runtime variant. Source1acea28a/build135
passes39.87s of full regression for idle alignment, with direct two-quarter-turn,
no-invented-order, active-goal-preservation and dense cache/worker/restore checks.
Ten full cases compare alignment off/on at all five selected profiles. Source134
failed to compile its new test because it called a nonexistent accessor; the
corrected test reads the existing checkpoint API. Preserve that failure; no
source134 benchmark was launched. The engine binary is identical after the
135test-only fix. No performance claim is made for alignment before full audit.

### Retaining the selected guidance mutation and refining current admission

On the matured RANDOM-03 solver, removing its selected single edge reversal
reduces2602to2550. The first five alternative layouts and three weaker contrast
values all lose. A nested search therefore keeps that first reversed edge and
adds the next1/2/3/4 edges from the same deterministic shuffle (total flips2–5),
on RANDOM-03 and RANDOM-04. This is an explicit map-specific guidance trick,
selected by archived-input performance; no claim of unseen-layout transfer.

RANDOM-04 also retests active caps540/550/570/580 on the current2777 combination
of contrast2.2,dispersion0.4,horizon pairing16 and price32. Older cap sweeps used
different guidance/scoring combinations. These four current-profile interaction
checks remain full strict runs, with no changed work budget or selected seed.

### Resumed experiment closures and audit checkpoint, 15:45 UTC

All seven lower-density admission cases lose: RANDOM-01 cap80/90 until150
scores690/708 against729; RANDOM-02 cap160/180 until150 scores1334/1361
against1401; RANDOM-03 persistentcap340/360/380 scores2467/2536/2546 against2602.
Every result completes its full horizon and passes independent replay/timing.

RANDOM-03 alternate fieldseeds1/7/15/19/31 without flips score2435/2506/2550/2422/2489;
contrasts0.6/1.2/1.8 score2551/2584/2581. Its general uniform-field/horizon-off
case fails the first entry at1313.366ms. This is preserved as a timeout, with
no truncated performance score or inferred host cause.

Source133 components give control2777; two-proposal/groups16/repair0=2760;
groups32/repair0=2777; groups32/repair1=2723. The equal2777 component run follows
different trajectories. One-proposal/groups16/repair1 fails timestep0 at1075.667ms;
its separately declared FIRST_K2048 alternative completes2739. Legacy zero-component
2773 and default-off2777 controls are identical in all six trace fields to their
archived references. Keep the new component feature disabled.

Twelve trace comparisons are archived in results/resume-control-equivalence-v135.
Unchanged source132 controls2602/2777/4011, source133 controls2777/2773, resumed
729/1395 repeats, and source135 controls729/1401/2602/2777 match all six fields.
The newly selected1401 therefore has an exact full repeat. Source135 idle alignment
loses719/1395/2545/2756 onR01–04; finalR05 results and full batch audit are pending.

The full RANDOM-05 progress audit passes87historical frontier rows. Its reader
now obtains executed steps from the original full trace when an early summary
lacks actual_path_steps; it never substitutes makespan. Original summaries remain
unchanged. All six existing horizon tests pass, and the root PILOT dashboard and
waiting report are regenerated from audited evidence.

### Exact shared-cache transfer and sparse cutoff refinement, declared 15:51 UTC

Four full RANDOM-05 cases test the existing immutable shared task/pose ranking
cache on the selected4175profile. The2048MiB exact-cache control retains
FIRST_K7968/K16320; a separately declared FIRST_K6144 case reserves startup
headroom. Two more cases retain that startup work and increase later fixed work
toK19584/K23040. Shared order-only storage automatically uses full score entries
when movement bias needs them; no approximation or elapsed-time exit is introduced.
Only full six-field equality can qualify the cache as an exact optimization.

Six full RANDOM-01/02 cases refine cutoff0.5/0.625/0.875 around the selected0.75.
All other current729/1401 settings and seeds stay fixed. The only changed choice
is explicitly gated by --trick RANDOM-01/02. These are development selections,
not fresh-input claims. Published records and every original failure are retained.

### Source136 hypothesis: prefer earlier progress among equal window plans

The window solver currently compares total action-plus-terminal cost, then
terminal remaining work. Complete plans with equal values can still delay progress
until late in the window, and equal-cost repair acceptance can move between them.
The new optional R05_WINDOW_PROGRESS_TIE=1 adds accumulated remaining-chain cost
as a tertiary comparison. It never knowingly trades worse primary or terminal
cost for this preference. A* work, collision reservations, task assignments and
hard deadlines are unchanged. Default0 preserves the old comparator exactly.

This is a general mechanism with no map name, horizon knowledge or selected edge
embedded. Full paired controls on the three windowed densities will decide whether
it helps. Regression checks cover worker counts, caches, heap layouts, checkpoint
restoration, simulated annealing and invalid configuration combinations. No gain
is claimed before compilation, regression and full-run audits.

Source136/05fd4b76 compiles and passes45.53s of full regression (45.54s total).
Ten declared full cases compare progress tie-breaking off/on: general and selected
horizon-trick profiles onR01/R02, and the selectedR03profile. Same work budgets,
seeds, source and settings within each pair; the direct comparison will measure
both throughput and extra evaluation time. No parameter is selected from held-out
input performance. Build SHA2b8345852259c9842525484d12613677976baefd43fb8ed741aec6c869fe277e.

### RANDOM-05 repeat qualification and frozen fresh validationV6

All nine record4175follow-ups and all ten idle-alignment pairs are now independently
audited. Cap680seeds0/1/2/3 give4175/4151/4171/4120; qualificationmax816.066ms,
peakRSSbelow491MB. Original4175max872.171ms remains preserved. Exact source132repeat
and source135default-offcontrol match all six archived trace fields. Idle alignment
loses at every density (719/1395/2545/2756/4130), so keep it disabled.

FreshV6protocol1502afa3412d06e3f27d388653a4c89008fac4c8 froze candidate4175,
baseline4011 and two unmodified NMS runs for each new seed50015/50016 before
input generation. Both PILOT roles use source132; only cap680/fastadmission1 differ.
The older50001–50012 remain held out, and50013/50014 remain ungenerated R04reserves.
No fresh result may change this frozen candidate or serve as a tuning target.

### Current-profile search interactions and sparse record repeat, 16:00 UTC

The six cutoff refinements pass full audits. RANDOM-01 scales0.5/0.625/0.875
score725/728/729; RANDOM-02 scores1400/1394/**1408**. The latter replaces1401
as a selected seed2 record. An exact repeat and seeds0/1/3 are declared on the
new profile before outcomes. Independent paths, assignments and task events
are required in addition to throughput equality.

RANDOM-04 declares two larger complete fixed-work budgetsK10240/K12288, retaining
FIRST_K4032, and four root-mutation rates0.05/0.15/0.2/0.3 on the current2777
configuration. Previous mutation/work tests predated the cap, guidance and late
matching combination; these are interaction tests, not assumed gains. All retain
strict full1000-step entries, fixed work,32workers and explicit --trick RANDOM-04.

### Source137 hypothesis: goal-local lane costs in the window solver

RANDOM-03's completed routes have substantial observed detours. A local guidance
taper already exists for the reactive policy but was deliberately barred from
windowed A*, whose action evaluator used global edge prices. Source137 makes
all three window action-price consumers (complete-path score, bounded A*, blocker
guides) use the current waypoint's exact edge price, then permits that existing
option in window mode. The map-specific guidance choice still requires the
explicit --trick flag. With localmix0 the edge price is exactly the old value.

An independent exhaustive finite-horizon dynamic program checks a single-agent
multi-waypoint/repeated-goal repair against the exact optimum. Dense full-trace
fixtures exercise workers, cache modes, heaps and checkpoint restoration. This
addresses a cost-model compatibility limitation; it is not a measured throughput
gain. After regression, fullR03controls and radius/mix pairs will test the idea.

### Admission and scheduling interactions on frozen development input, 16:05 UTC

Six fullR05source132cases change one logical factor from the4175profile: cutoff
scale1/1.5 (current1.25), directional cutoffmix0.75 (current1), late pairing
weight4/16 with optional idleprice32, and keepbonus0 (current0.5). The late-pairing
pair tests the same compound mechanism previously measured onR04. These cases
use only the archived development input. The freshV6candidate remains frozen
at4175regardless of their outcomes; no held-out input is used for selection.

Source137/73134b7c passes40.28s regression, including the independent dynamic
programming optimum. BuildSHA2de14db1773bd6b94412ccc6726944b663fd611c9137c361d8d39b8814e41c6a.
Seven fullR03cases declare an unchanged2602control and local guidance radii1/2/4
crossed with mix0.25/0.5. Work, seed and other settings stay fixed. All are
explicitR03tricks and all must pass the same strict full-run audit.

Source136's complete ten-case batch is audited. The optional progress tie-break
loses onR01(719general/723trick) andR03(2556), but improves generalR02to1385.
The horizonR02case ties1401 on different trajectories. All five disabled controls
match their archived trajectories in all six fields. The new general1385record
receives an exact repeat and six additional planner seeds; a separate off/on
pair tests its interaction with the now-selected1408horizon profile.

The1408repeat is exact. Seeds0/1/2/3=1396/1391/1408/1393; all are audited and
strict-valid. This remains seed-selected performance on the archived input.

### Source138 hypothesis: nonlinear assignment prices (explicit trick)

The scheduler minimizes a sum of linear approach-plus-chain costs. An increasing
concave transform may favor more immediately cheap pairings, while a convex one
may distribute trips more evenly. R05_MATCH_POWER defaults to1 (exact identity).
Other powers in[0.25,2] require the explicit instance trick flag. The transform is
32*((1+c/32)^p-1)/p, applied before the existing keep bonus; optional idle prices
use the same transform so their unbonused acceptance threshold stays unchanged.
Opened tasks remain locked and mandatory admission caps are unchanged. This is
an assignment hypothesis, not a theorem about throughput; adverse fairness is
possible and completed/censored waiting statistics remain mandatory.

Regression fixtures check a real matching decision change, protection of opened
orders, unchanged single-pair idle thresholds in both matching backends, exact
fast-dummy/worker/cache/checkpoint equivalence, and trick/range guards. Only after
regression will powers0.5/1/2 run on all five selected archived profiles.

Source138/8fe070ec passes40.74s regression (40.75s total), binarySHA
0a6bfe88634888e1a2d5e1e89b718242c7bb28435aabf764ceb489c994d72be5.
Fifteen frozen full cases compare matching powers1/0.5/2 on all five selected
profiles729/1408/2602/2777/4175. Power1 controls must match the existing traces;
other powers remain explicitly flagged assignment tricks. All other work/settings
are fixed within each density, and all original attempts will be retained.

R04search interaction audit is complete: K10240/12288 gives2751/2754; mutation
0.05/0.15/0.2/0.3 gives2703/2690/2679/2634. More search does not improve2777.

### Source139 hypothesis: retain selected guidance edges and vary another

The earlier nested flip2–5 comparisons all include the same second edge. Their
negative results do not distinguish an unhelpful second edge from all other
possible additions. R05_FLOW_EXTRA_FLIPS/EXTRA_FLIP_SEED keeps the entire existing
selected prefix and independently shuffles its unused suffix; the requested extra
edges are distinct and cannot undo the retained ones. Default0 preserves the
original layout exactly. The option requires flow guidance, a retained prefix and
an explicit instance trick. Physical edges and their pairwise prices remain intact.

Regression checks verify exact retained/extra edge counts, preservation of each
selected edge and all edge prices, worker/checkpoint reproducibility and the trick
gate. Following regression, R03/R04will test one additional edge with seeds0–3,
plus controls, on their frozen2602/2777profiles. No held-out input participates.

### Audited4197record and negative closures,16:31 UTC

R05cutoffscale1 improves4175to4197; mean569.038/max802.896ms, RSS490.828MB,
longest completed order1977steps and150initial orders still unfinished. This
selected result now updates both32-worker best manifests and both progress logs.
Its exact repeat and planner-seed checks are next; frozen freshV6still tests4175.
All six coupling and all four shared-cache runs are audited. Shared caching
preserves4175and4135exactly but is slower and uses more RAM; largerK19584/23040
scores4073/4162. Keep the uncached fixedK16320profile.

All seven source137R03local-guidance cases are audited. Control2602 is exact;
radii1/2/4 with mix0.25/0.5 score2543/2564,2573/2561,2558/2563. Leave it off.
Source136generalR02plannerseeds0..6 score1378/1374/1382/1376/1390/1385/1380.
Promote the selected general1390; the1385seed5repeat is exact. Adding progress
ties to the1408horizon profile loses1399; its disabled1408control is exact.
The independent oriented route-efficiency diagnosis covers all five selected
PILOT traces and matchedNMS traces; see results/oriented-efficiency-20260921/REPORT.md.

Source139/fd1a724d passes44.82s regression (44.83s total), binarySHA
3dcfbffe50b75d414c0d9c7db7029b7f04ad27e8e83c48e0eb39e79c9d90a6b7.
Ten full cases now compare controls and extra-edge seeds0–3 onR03/R04.
The entire selected first edge remains fixed; only the independent additional
edge changes. No new run length, task/start input or work budget is selected.

### Frozen freshV6 passes,16:35 UTC

The immutable4175candidate scores4182/4177 against baseline4001/3941 and NMS
3154/3155,3178/3149. Aggregate gains are31.9912%over the stronger NMS repetition
per input and5.2506%over the preceding configuration; both inputs improve.
All eight runs pass independent full replay and protocol/source/resource/timing
checks. Candidatemax774.0/798.8ms, peakRSSbelow490MB. FRESH_VALIDATION_V6.md records
full tables and waiting metrics. Seeds50015/50016 remain held out permanently.
The new4197cutoff is not qualified by this earlier frozen protocol.

### Sparse window-length follow-up and nonlinear matching closure,16:38 UTC

Four full source132cases extend the currentR01/R02planning windows from20to24/32,
retaining their old prefix lengths, repair counts, seed and scheduling settings.
These are window lengths, not changes to the known600-step simulation horizon.
The new generalR02record1390(seed4, source136) also receives an exact full repeat.

Source138nonlinear matching loses at every density. Powers0.5/2 score723/721,
1395/1401,2565/2546,2710/2734,4122/4047 against729/1408/2602/2777/4175 controls.
All runners complete strictly; full independent batch audit is in progress.
Keep the feature disabled. The direct benefit of nonlinear cost preferences is
not established by their algebra or the successful regression fixture.

All fifteen source138matching runs now pass independent full replay and resource
checks. Its five power1controls match729/1408/2602/2777/4175in all six fields.
Default identity is preserved; neither nonlinear alternative improves throughput.

### R04route-proposal transfer and forecast aging,16:44 UTC

Five declared fullsource132cases test move-proposal bias1/2/3 on the current2777
profile, a full-score cache control with bias0, and forecast age updates alone.
R05uses movement bias3; R04currently uses0, and earlier frozen-density transfer
used a different planner/search/admission combination. Biased ranking needs full
scores, so the existing compact-cache setting falls back to its exact full-score
representation. The bias0/full-score control isolates that storage change.
No real timing deadline is relaxed and all fixed work remains complete-or-fail.

R01planningwindow24/32gives727/722. R02window32fails its first entry at2007.141ms;
preserve the original failure without a partial score or assumed host cause.
The generalR02selected1390repeat completes; its independent audit is next.

### R03guidance price decomposition,16:45 UTC

The independent route diagnosis finds109,234extra loaded steps above the relaxed
oriented bound in the2602trace, including substantial detours. Altering individual
edge directions has not improved it. The prior R03manifests all kept the field's
load multiplier at0.75. Seven new full source132cases therefore vary that multiplier
0/0.25/0.5/1, or blend all action edges0.125/0.25/0.5 toward physical unit-action
cost2. The latter recomputes exact shortest paths from the changed positive edges;
it is not an inconsistent mixture of heuristic distance tables. The selected base
layout, edge flip, seed, work and horizon stay fixed. Every case remains an explicit
R03guidance trick. This tests routing prices separately from further layout search.

Source139independent extra-edge seeds0–3 give2546/2589/2561/2556 onR03 and
2746/2739/2756/2750 onR04, against controls2602/2777. No improvement; audits follow.
R02window24scores1382; window32failed its first entry at2007.141ms. Keep window20.

### Declared 2026-09-21 16:56 UTC: complete field-cost follow-ups

R03 coherent physical-edge blend0.25 finished2606 versus2602; independent replay is underway. Freeze an exact repeat, plannerseeds0/3, and nearby mixes0.1875/0.3125/0.375 at unchanged full work. Transfer physical-edge mixes0.125/0.25 and traffic-assignment betweenness0.5/1 to the current R04=2777 and R05=4197 profiles. All other selected parameters, inputs, full horizons and strict resource budgets remain unchanged. These are explicit map/density tricks and development data, not fresh validation. Specs: `random03-record2606-full-v132.json`, `random45-record-field-cost-full-v132.json`. No new source is needed; source027df4d9/build132.

### Completed follow-ups, 2026-09-21 17:01 UTC

- R05=4197 repeats all six trajectory fields exactly. Plannerseeds0/1/2/3 score4197/4168/4179/4143; all four full runs independently replayed, largest entry870.036ms, RSS<491MB. Their aggregate16687 exceeds the preceding4175profile's16617 by0.421%, positive on each paired planner seed. This is development replication, not a fresh-input validation of the new cutoff.
- GeneralR02=1390 repeats exactly in all six fields, mean321.415/max486.246ms. The source139 default controls exactly preserve2602/2777; extra independently flipped edges lose at both densities. All ten full runs replayed.
- Longer windows do not improve sparse records: R01H24/H32=727/722; R02H24=1382; R02H32 fails the first-step deadline at2007.141ms. Preserve the failure, no partial score.
- R03 current2606 uses coherent physical-edge blend0.25. Full800steps pass replay, mean504.663/max731.223ms. Load weights0/.25/.5/1 yield2537/2516/2542/2559; physical mixes.125/.5 yield2562/2572. The four-task record is provisional as an improvement until paired seeds complete; it is now correctly logged as a selected best.
- R04 move-bias1/3 and forecast-age yield2760/2749/2737, all below2777. Full-score cache control and bias2 fail at timestep0,1050.888/1053.616ms. The measured startup cost includes512/522ms matching and471ms look-ahead. Do not infer a host cause from this alone. All successes/failures are archived and audited; keep these options off.

### General repair-order experiment, 2026-09-21 17:01 UTC

Sourcef0647771/build140 adds optional `R05_WINDOW_REPAIR_ORDERS=2`: repair each selected neighborhood in both the sampled priority order and its reverse, against identical outside reservations, then choose the better complete plan. Every declared order finishes or its bounded search fails and rolls back; no wall-clock truncation. Source611aa2a6/build141 preserves the original one-order fast path to avoid charging default controls for extra allocations/restoration. Both builds include legality, worker/cache/heap/storage independence, checkpoint, annealing, bounded-failure and configuration tests. Build/test results are pending; no throughput gain is claimed. This mechanism has no map/horizon dependency and needs no trick flag by itself. Evaluation will distinguish equal-work (half as many neighborhoods, twice the orders) from explicitly larger work.

### Exact matching storage optimization, 2026-09-21 17:06 UTC

Source1509f24e/build142 replaces packed `vector<bool>` visited flags with bytes and reuses the Hungarian scratch arrays across augmentations. Cost arithmetic, scan order, tie-breaking, dummy preconditions and matching policy are unchanged. Full regression passes, including an independent exhaustive rectangular-assignment optimum test and existing exact dummy-prefix equivalence tests. Five unchanged selected full profiles are frozen for six-field trace comparison; no speed improvement is claimed until measured. Binary SHAcdb9de68a67c8a2d1afed31398da44fc6a099c83d0b75ba8e075796832ff6da4.

### Dense-search transfer declared 2026-09-21 17:07 UTC

Prior manifests contain no R04 case with active admission and K>=16000. Freeze the complete R05=4197 depth8/B18/K16320/first7968 search on R04, using plannerseed4 and the1000-step horizon, at caps560/595/630 (595 preserves the selected85% active share). Separately transfer only that search to the current R04=2777 scheduler/guidance/cap560 profile, with movebias0/3 and shared rankings disabled. This tests a coordinated search-regime change, with five full original attempts, source027df4d9. All map/density/horizon choices remain explicitly trick-gated. No new task streams or new guidance layouts are used.

### Follow-ups declared 2026-09-21 17:09 UTC

The completed general R02 paired-order1536-neighborhood run reports1394 versus1390 at single-order2048; audit is pending. This uses50% more potential single-agent repair calls. To separate the effect of priority orders from more work, freeze all seven plannerseeds0..6 for one order/3072 neighborhoods and two orders/1536 neighborhoods, source611aa2a6. Same input, no guidance or horizon trick. In parallel, test R05=4197 with the earlier positive startup rank weighting (powers.125/.25, durations125/250) now combined with cap680. No prior admission-enabled R05 manifest included this feature. All four are source027df4d9, full2000steps, explicit `--trick RANDOM-05`; every other current setting stays frozen.

### R03=2606 qualification completed 2026-09-21 17:12 UTC

All six follow-ups pass independent replay/source/input/resource checks. Exact seed5 repetition matches all six trajectory fields. Paired seeds5/0/3 give2606/2572/2590 versus2602/2548/2566 (+0.674%, positive on all three); qualificationmax766.554ms. Nearby mixes.1875/.3125/.375 lose2576/2583/2601. This validates development replication only; V1fresh data applies to the preceding2602recipe. Proof: `results/random03-record2606-split-full-v132/control-equivalence.json`.

### Repair-order first batch audited 2026-09-21 17:16 UTC

All15original attempts are audited. Four successful default controls match all six full trace fields. R01 variants and R02trick variants lose; R03paired variants2594/2600 lose2606. Its unchanged source141control failed t0 at1073.309ms with exactly the source132search counters; preserve without attributing a cause. GeneralR02=1394 is promoted with full replay; seed0follow-up reports1397 and is awaiting its own audit. Freeze five horizon-transfer cases for that general seed0 profile: cutoff.5/.75/.875/1 plus cutoff.875with latepair4/idleprice32. They require explicit `--trick RANDOM-02`; no claim that the added trick helps yet.

### General seed-component merging declared 2026-09-21 17:17 UTC

Source3bb71976/build143 adds default-off `R05_WINDOW_SEED_MERGE=1`. Before LNS, combine independent collision components of the retained and fresh complete plans, choosing only components whose complete-path cost improves without increasing primary cost. This preserves potentially useful commitments while replacing stale routes elsewhere; both parents and the mixed plan are validated. No map/horizon dependency, no additional search iterations. Ten full on/off cases cover generalR01=726, generalR02selectedseed0=1397, tricksR01=729/R02=1408/R03=2606. Tests cover multi-start seeds, zero-repair plans, full simulator legality, worker/cache/heap/storage independence and checkpoint restoration. Build tests are still pending; no gain claimed.

### Observed-progress horizon model declared 2026-09-21 17:27 UTC

Source88551e69/build144, regression42.47s, binarydca1dcb14741e98eb0cebf8cd5393fa612667b9e0b6a3c7000fe7252b4fa1ccf. Optional `R05_TRIAGE_PROGRESS_MIX` redistributes the existing total remaining-work estimate using task-specific exponentially smoothed progress in the oriented chain potential. It requires a mature history, bounds relative speed corrections, falls back on the fleet prior with insufficient/nonpositive progress, and resets on task replacement. Known-horizon policy remains explicitly trick-gated; opened assignments remain locked. History is checkpointed and copied into shadow forecasts. Independent conservation/fast-vs-stalled/task-reset/legacy-replay/cache/worker/flag tests pass. Freeze R04control plus mixes.25/.5/.75 at spans32/64; R05control plus.25/.5 at spans32/64. All full horizons, unchanged selected profiles otherwise. This is a hypothesis, no throughput improvement claimed.

### Audited checkpoint 2026-09-21 17:29 UTC

Selected records729/1408/2606/2777/4197. General726/1397/1582/1558/2226. GeneralR02two-order/I1536 seeds0..6 score1397/1392/1385/1391/1394/1390/1390, versus one-order/I3072 at1382/1389/1387/1391/1382/1389/1384. Total9739vs9704 (+0.3607%, five positive/one tie/one negative); versus the prior one-order/I2048 recipe9665, +0.7657% and positive on all seven. Same potential agent searches do not imply identical actual expansions. All14audited; largest entry738.478ms. Selected1397repeats exactly in source143's default control; the earlier1394also repeats exactly.

Source143seed-component mixing loses every profile: generalR01=718 vs726, trickR01=724 vs729, generalR02=1385vs1397, trickR02=1403vs1408, R03=2603vs2606. All10audited; all five defaults match all six fields. Leave seed merging off. Source142matching storage preserves all five selected traces exactly, not just scores. Means64.109/325.035/515.273/468.207/570.257ms; maxima109.036/484.837/786.174/739.297/768.160ms. All independently audited. These uncontrolled host timings do not establish a causal speedup.

CurrentR04/R05field-cost transfer loses or ties: R04mix.125/.25 andload.5/1=2688/2669/2704/2777; R05=4088/3895/4080/3954. The second2777is a tie, not a record. CompleteR05search-to-R04 transfer caps560/595/630=2567/2577/2499; retaining the R04scheduler/field with broad shallow search andbias0/3=2716/2719. All13runs independently replayed; no new setting promoted.

GeneralR02=1397with horizon cutoffs.5/.75/.875 gives1405 each; cutoff1=1400; latepair4/idleprice32=1399. All five audited, none beats the selected1408trick recipe. R05startup weighting is still running; power.25/duration250failed at t0,1097.496ms. Preserve the failed original. No new fresh-input or unseen-layout claim.

### R05new4236 follow-ups declared 2026-09-21 17:31 UTC

Startup rankpower.125 for250steps finishes4236, mean563.658/max817.827ms, versus4197. Independent audit is in progress. Freeze exactrepeat and seeds1/2/3; separately nearby powers.0625/.1875 at250steps and durations375/500 atpower.125. All source027df4d9/build132; identical full2000step input and strict resources. These are explicit task-priority/horizon/guidance/admission tricks. FreshV6remains validation only of4175. The failed.25/250 original remains retained, no partial score.

### Audited R05frontier4236, 2026-09-21 17:33 UTC

Full2000steps independently replayed, mean563.658/max817.827ms, peakRSS490.156MB. New best+39tasks versus4197, +33.54%versusmatchedNMS3172; startup rankpower.125/250steps combined withcap680. Completed waitmax1994, initial unfinished148/unopened107, oldest unfinished censored2000. Exactrepeat/three plannerseeds and four nearby startup variants are running. No fresh-input claim. Source027df4d9; binary175466cf2a3ee9bcb2e62905f1fffa8173dc66a2bab9c4d71f11914f22fe4801.

### Sparse initial plans and R04startup weighting declared 2026-09-21 17:37 UTC

Freeze starts4/16withnoise30 on both general/trick currentR01/R02profiles, plusR01trick retained-prefix0/2/10/14. The oldv83R01starts tests usedseed5 (control718), not currentseed4; oldR02starts tests used I512/keep6/seed5, unlike currentI1536pairedorders orI2048/keep10. Prior negative starts16/64 remain recorded. Separately test R04selectedcap560 with startup rankpowers.0625/.125 for125/250steps, motivated by the positive R05admission+startup combination; the previous broad-shallow search transfer failed. All source1509f24e/build142, strict full horizons and existing explicit trick gates.

### R04observed-progress first records, 2026-09-21 17:40 UTC

All seven R04rows are independently audited; the same mixed batch still has five pending R05runs. The first audit invocation saved the valid seven-row partial report then exited because I omitted `--allow-pending`; no benchmark check failed. Re-run the full audit when R05completes. R04first improves2777->2778(mix.5/span64,17:35:08.379917UTC), then2782(mix.25/span32,17:35:31.545879UTC), mean478.686/max739.553ms. Both chronological selected records are retained. Source144off-control matches the old2777 in all six fields. Freeze exact2782repeat/seeds0/3/5, nearby mixes.125/.375 and spans16/48, plus R03control/mixtures.125/.25 atspan32. No freshR04inputs have been generated; target2838still56tasks away.

### 2026-09-21 17:49 UTC: sparse initialization trials close

All12 current-profile starts/prefix cases are independently audited. GeneralR01 starts4/16=726/722, trick=729/728; generalR02=1391/1396, trick=1408/1388. RANDOM-01 retained prefixes0/2/10/14=719/722/722/725. None exceeds its selected reference; keep the existing settings. These are full600step comparisons, not prefix estimates. Evidence: results/random12-current-starts-split-full-v142/.

### 2026-09-21 17:52 UTC: progress correction and startup separation

All12 source144progress cases and all4 R04startup cases are fully audited. Both144off-controls reproduce the preceding2777/4197 trajectories in all six fields. R05progress mixtures .25/span32,.25/span64,.5/span32,.5/span64 score4205/4199/4189/4190 on the preceding4197profile. They do not displace the already earlier4236selected record. R04startup ranks .0625/.125 x125/250 all lose2738/2719/2727/2747. Freeze four full2000step cases combining4236startup with progress correction: exactdefaultcontrol, mixes.125/.25 span32 and.25 span64. Explicit RANDOM-05trick, same fixed work, source144. No gain is assumed.

### 2026-09-21 17:56 UTC: RANDOM-04 progress replication and mixed repair groups

RANDOM-04 progress-correction seeds0/3/4/5 score2749/2741/2782/2781 versus2726/2742/2777/2771: aggregate+0.336%, three positive and one negative. All11 follow-up runs pass full replay and resource/time checks. Selected2782 repeats in all six fields; the RANDOM-03 off-control also exactly reproduces2606. Four nearby R04 settings all lose2766/2761/2768/2775; progress correction on R03 loses2605/2601. Keep R03 off. Evidence: results/random34-progress-followup-split-full-v144/.

Source2ab87ed9/build145 introduces optional general `R05_WINDOW_GROUP_MIX=1`: cycle floor(N/2),ceil(N/2),N,2N robot groups across four repairs. The mean requested group size equals N before team-size clipping. Iteration count is fixed; actual search expansions can differ. Larger groups can change interacting routes together, with small repairs between them. No map or horizon information enters this mechanism. Tests cover worker/cache/heap/storage/checkpoint determinism, team-size clipping, bounded failed repairs and configuration validation. Build/regression pending, no performance claim.

### 2026-09-21 17:57 UTC: startup record repeats, but the seed mean falls

The selected4236 run repeats in all six trajectory/scheduling/task fields. Planner seeds0/1/2/3 score4236/4051/4172/4096, total16555, versus4197/4168/4179/4143, total16687: **-0.791%**, one positive and three negative. All four full2000step runs pass independent replay, source/input/resource checks, with maximum entry827.104ms. Keep4236 as the requested selected-seed frontier and preserve4197 as the better four-seed aggregate. FreshV6 qualifies4175 only. Evidence: [paired comparison](results/random05-record4236-split-full-v132/paired-comparison.json).

### 2026-09-21 18:01 UTC: completion-time objective declared

Source5cb554e5e60fb07130b630a1e4a2361548a1cb8e/build146 adds optional `R05_WINDOW_COMPLETION_PRICE`. Each action while an assigned visible chain remains unfinished incurs the declared price; completed agents do not. The planner still scores the complete fixed window and exact terminal route potential. A horizon-clipped remaining-hop bound supplies an admissible heuristic for the extra charge; repeated waypoints can weaken but cannot invalidate the bound. This task-priority change requires an explicit --trick RANDOM-0N. Tests compare with exhaustive dynamic programming on complete/incomplete/repeated chains and check worker/cache/heap/checkpoint/failure invariants. Build pending; no benchmark gain claimed.

All four4236startup refinements are audited and lose: power.0625/.1875 for250steps=3961/4142, power.125 for375/500steps=4153/4075. Full original runs and failures remain retained.

### 2026-09-21 18:10 UTC: exact buffer reuse and startup admission

Source888c1d51/build147 passes44.91s regression, binary53bf672d2262a48b1f81e597eb6172a78c601f15a80fbd9e22d83dab21da2828. `R05_WINDOW_PATH_REUSE=1` reuses per-island old/replacement path buffers and swaps accepted paths, preserving arithmetic, random draws, search order and work counts. Tests cover mixed group sizes, both repair orders, worker/cache/heap/checkpoint and bounded-failure behavior. Ten full on/off runs cover general/trickR01/R02andtrickR03; full trace identity and runtime remain to be checked.

Source9cce7453/build148 passes45.17s regression, binaryfd40a45ddf4ea596ed9f8d5489bc477f68d347fff1c3be9d0973d0c9d9a539e3. Explicit `R05_INITIAL_ACTIVE_CAP`/`R05_INITIAL_ACTIVE_STEPS` returns to the ordinary cap at a fixed step; all robots stay movable and opened assignments stay locked. Tests cover initial/steady/expiry boundaries, opened tasks above cap, exact/greedy matching, fast dummy shortcut, worker/checkpoint and trick gates. Eleven full comparisons freeze current2782/4236controls, smaller initialcaps400/480 onR04 and480/560 onR05 for100/250steps, plus the stable4197R05control. All current profile settings otherwise stay unchanged. No gain is claimed.

All ten source145group-mix attempts are audited. General/trickR01 loses717/725 versus726/729; general/trickR02 loses1389/1381 versus1397/1408. R03control andmixed groups fail atstep8/77,1031.163/1038.732ms; no score is counted and no hardware cause is established. The eight successful sparse runs remain strict-valid.

### 2026-09-21 18:16 UTC: sparse-density fresh validation frozen

Protocolf74286e6 was committed before generating new seeds50017/50018 for RANDOM-01 and RANDOM-02. Sixteen full600step comparisons freeze selected trick729/1408, general726/1397, and two unmodified32-thread NMS repeats on each stream. General and trick profiles differ in several settings, so their comparison is not a single-option ablation. All new seeds are permanently excluded from tuning at any density. Report each input, aggregates and failures; no after-the-fact threshold. Existing R04seeds50013/50014remain ungenerated. These are new starts/tasks on the same map, not new layouts.

### 2026-09-21 18:18 UTC: RANDOM-05 selected frontier4,242

The startup/progress combination is independently audited at4242, mean589.171/max780.487ms, RSS489.562MB. Source88551e69/build144, seed0, progressmix.125/span32; other4236settings unchanged. Control4236matches all six fields. Mix.25atspan32/64scores4230/4230. This six-task selected gain has not yet received seed/fresh-input qualification. Completed waitmax1983,147initialordersunfinished/110unopened, oldest unfinishedcensored2000. Freeze exactrepeat andseeds1/2/3 on the same binary and full2000step input.

### 2026-09-21 18:23 UTC: bounded development seed extension

Freeze16 full development-input runs: existing selected R01/R02profiles on plannerseeds7..10 (source132), and current R04progress profile on seeds1/2/6/7/8/9/10/11 (source144). This completes the R04profile's declared0..11 planner-seed range, with existing0/3/4/5results retained. These are seed variations, not new algorithmic mechanisms; selected maxima and paired aggregates remain distinct. No fresh task/start seed is involved or used to choose settings. Sources are declared in experiments/random124-seed-extension-sources.json.

### 2026-09-21 18:25 UTC: unchanged-path cost reuse

Source0c80c10076da196d30c165ceebf1f490fa7ef71c/build149 passes45.52s regression, binary0a62da80e20fd949f5f3e9657eb5f421fc9b4ed40f73bb0ccc96e1c7c93965cd. Optional `R05_WINDOW_COST_REUSE=1` uses the stored complete cost when a repaired route is identical to the incumbent. Assignment, starting stage and scoring weights are fixed during this call, including secondary/tertiary fields. No floating-point sums, random draws or work counts are altered. Cached-hit counters expose reuse frequency. Full on/off comparisons cover five current window profiles; performance remains unmeasured.

Source147path-buffer reuse passes all ten complete six-field reference comparisons, with mixed timing: R03off/on mean516.248/529.674ms,max752.221/797.388ms; R02trickmean309.784/332.675ms. Keep it off pending convincing speed evidence. Source146completion prices all lose onR01/R02/R03; its three off-controls are exact. All12runs audited; pricesstayoff.

### 2026-09-21 18:28 UTC: current scheduler costs and admission at medium density

Freeze14 full source132 comparisons on the selected development profiles. R01/R02each test task-length weight.125/.375. R03tests.125/.375/.5/1, full oriented-chain matching, keep-bonus0/2, late-horizonweight4withidleprice32, andactivecaps360/380withfast exact matching. This separates scheduling choices from the existing window/guidance search; every per-instance variant carries its explicit trick flag. These archived-input trials do not use the newly generated fresh inputs. No improvement is assumed.

### 2026-09-21 18:33 UTC: sparse fresh validation passes

All16 original runs pass frozen-protocol/source/binary/input/resource checks and independent complete replay. R01selected730/702, general727/698, NMS645/654 and630/630: selected+11.526%, general+10.981%aggregate versus stronger repeats. R02selected1382/1348, general1376/1338, NMS1213/1203 and1198/1182: selected+13.231%, general+12.567%. Every individual input is positive. These two task/start streams share the existing geometry; scores do not replace the development frontiers. See RANDOM01_FRESH_VALIDATION_V1.md and RANDOM02_FRESH_VALIDATION_V1.md. Seeds50017/50018remain excluded from tuning.

### 2026-09-21 18:35 UTC: local coupling around the current dense profiles

Freeze16 source144 full development runs. Around R04=2782: steadycaps540/580, dispersion.2/.6, fieldcontrast2.1/2.3, cutoff.8125/.9375. Around R05=4242: steadycaps660/700, dispersion.6/1, contrast2.2/2.6, cutoff.875/1.125. All preserve the current observed-progress correction and, for R05, startup weighting. Each case changes one value and retains its explicit instance trick. No new input or partial horizon is used; no gain claimed before full audit.

### 2026-09-21 18:44 UTC: equal-cost matching exploration

Source1d1022fa/build150 passes45.81s regression, binary9676644c51f9c31e034eba41802f1e44b7bb1b0552d9190f549999b0e3075394. Optional general `R05_MATCH_FREE_TIES=1` prefers an unoccupied column only when reduced-cost distances are exactly equal. The Hungarian optimum cost is preserved; the selected optimal pairing may change, so throughput needs new comparisons. Independent exhaustive rectangular optima, all-zero scan-count reduction, thousands of exact dummy-prefix equivalences, configuration guards and worker/checkpoint tests pass. Fourteen full off/on cases cover general/trickR01/R02plusselectedR03/R04/R05. No performance gain claimed.

### 2026-09-21 18:44 UTC:4,242 qualification complete

All four full runs are independently audited. Exact repetition matches all six fields. Seeds0/1/2/3=4242/4028/4183/4128, total16581, versus the previous4236profile16555 (+0.157%,three positive/one negative); versus4197profile16687 (-0.635%). Largest entry782.28ms. The selected best remains4242; no fresh qualification is implied. [Paired comparison](results/random05-record4242-split-full-v144/paired-comparison.json).

### 2026-09-21 18:46 UTC: completed negative trials and runtime checks

Source149unchanged-cost reuse passes all ten full six-field comparisons, but R03mean532.298/536.234ms does not show a gain. Source148initial caps all lose; all11cases and three full default controls are audited/exact. The16seed extensions add no record; R04eightpairedseeds improve0.3402%aggregate,6positive/2negative,while extensionseed11fails the first-step deadline at1050.759ms. Current-scheduler132all14attempts are audited and lose orfail; R03keep2fails t0at1102.232ms. All these failures remain excluded, without an inferred host cause. Detailed arrays and proofs remain in their results directories.


### 2026-09-21 19:25 UTC: faster RANDOM-03 proposal search reaches2,614

Source1a3076420bc245d5f56839936d7814eb9087b050/build153 passes49.27s regression.
Heuristic priority1.2 gives a fully audited2,614 tasks, mean438.994/max640.097ms,
RSS432.021MB. Originalweight1 still gives2,606; weights1.01/1.05 give2,583/2,603.
All20 full comparisons are audited. OnR01/R02, weights1.01/1.05 preserve the
selected scores;1.2 loses. Complete group acceptance still uses the original
unweighted objective; this is a proposal search change, not a partial budget.
The2,614 exact repeat, planner seeds0/3, weights1.1/1.15/1.3/1.5/2, and higher
fixed budgets7,168/8,192 are running. Prior freshV1 continues to qualify2,602 only.

Source150free-column ties preserves all12 successful full traces;R03/R04on
fail first entries1,080.753/1,043.654ms. Source151optional-column compression
passes all six full runs;5 declared reference comparisons are exact. R04startup
matching falls from522ms to477ms; maximum full-run entries off/on are838/777ms
(seed4) and814/767ms(seed0), but average runtime is mixed. R02 stays1,408.
Source152query caching also preserves all10 successful traces but slowsR03:
mean555ms off,580/592ms at512/2048queries. GeneralR02cache512fails timestep2
at1,003.501ms. Keep query reuse off. Full failures and resource checks are archived.

The waiting-front diagnostic is observational: unassigned robots make up15.6%
of stationary fronts faced by assigned waiters onR04, versus24.0% of robot-time;
R05figures are9.9% and15.0%. This does not support treating unassigned robots as
the dominant local obstacle, and does not infer intended moves or recoverable tasks.

Sourcea1f66070ee2d42eba95408564e2d3ebec48eded6/build154 passes49.56s regression.
The explicit field-jitter trick varies undirected road costs smoothly, keeping
both directional costs in proportion and restoring their mean before any optional
physical-edge mixture. Twenty-two fullR04/R05comparisons freeze strengths.025/.05/.1
onR04 and.05/.1 onR05, seeds0..3, with controls. R04uses the verified exact compact
idle-column option to reduce startup cost. No field-search gain is claimed yet.


### 2026-09-21 19:40 UTC: revisit matching policy on the current dense profiles

Six source144 full comparisons change only `R05_HUNGARIAN`:0 for global greedy
matching,256 or512 to use exact matching once the eligible robot set is small.
The selected R04/R05 profiles use1000 throughout. Earlier greedy observations
predate the current task-admission caps and progress correction. This tests both
assignment behavior and the half-second startup matching cost without new source.
All pair costs, admission rules and opened-task locks are preserved; greedy is
not assumed to optimize the same total pairing cost as Hungarian. No improvement
is claimed before complete strict runs and independent replay.


## 2026-09-21 20:03 UTC: component salvage experiment

Source353f09e6/build156 passes the complete regression suite in52.25s.
Binarybc972cde1cc51754fa6aa79752068f71bec8d6825aa62cab98d8e28517f88358.
The15 declared full runs compare modes0/1/2 on selected general/trick R01/R02
and trick R03. Each preserves its scheduler, field, work budget and planner seed.
Mode1 selects independently improving components after a complete repair;
mode2 also recovers compatible components when a later repair fails. Every
returned group remains complete and legal. Results pending; feature default off.


## 2026-09-21 20:06 UTC: qualification and negative results

R03record2620 repeats in all six trace fields. Five paired planner seeds total
13016versus12974 (+0.324%,three gains/two losses), tempering the earlier
three-seed+1.03% observation. FreshV2 gives2617/2609 versus2612/2620 for the
previous search (-0.115%aggregate). All eight full runs are independently audited.
Its mean runtime improves, but these inputs do not establish a throughput gain
versus the previous search. Kitty Knight is missing from that fresh protocol;
see [V2 report](RANDOM03_FRESH_VALIDATION_V2.md) for the NMS-only controls.

All22source154smooth-field trials are audited; both controls reproduce2782/4242
in all six fields and no perturbation improves either record. All15source155
next-pickup trials are audited; all five disabled controls are exact and every
forecast variant loses. All12source153weighted-budget attempts are audited;
none sets a record, and the R02I2560 first-entry1191.068ms and R03W1.5/I12288
timestep1entry1080.396ms failures remain preserved. Keep these options off.

The six source144 matching-threshold runs finish2754/2767/2766 onR04 and
4090/4164/4159 onR05, below current2782/4242. Independent audit is finishing.
The comparison policy now always usesmax(NMS,KK) for headline references;
legacy NMS-only measurements retain their explicit historical meaning.


## 2026-09-21 20:12 UTC: general RANDOM-01 reaches727

All15source156component-repair attempts are audited. Mode2 improves the general
R01record726->727 (publishedmax(NMS,KK)688: +5.67%). The selected trick record729
is unchanged. All five mode0 controls reproduce their complete archived traces.
R02/R03variants lose; R02general mode2 fails at t508,1034.193ms, and is excluded.
The new general727is a selected development run, not a cross-seed mean claim.
Declare its exact seed4repeat and four paired seeds0..3, plusI256/768/1024
alternatives. All12runs remain600steps/strict1s/32workers without tricks.


## 2026-09-21 20:27 UTC: all-density auction experiment

Sourcef80f7caa/build157 passes the full regression suite in52.77s. Binary
3e5043135255284e77d88b6ab7adfd2a8b3e67fc5e7b37cba58438ae5557650f.
Declare40 full cases: general and trick profiles at each RANDOM density,
epsilon0/.01/.125/.5,128bids per eligible row, preserving every other selected
setting. Default0 is the exact Hungarian control. Grouped optional/mandatory
idle objects preserve slot capacities; completed assignments certify their
price tolerance. Fixed-bid exhaustion invokes complete Hungarian fallback.
No gain is assumed. Publishedmax(NMS,KK) remains the headline reference until
new matched local baselines are fully audited.

### 2026-09-22 00:31 UTC: full guidance comparison and RANDOM-05 record

RANDOM-05 reaches4,254, +34.11% over matched max(NMS,KK)=3172. The80-update
guidance field adds12tasks to4242 with unchanged per-step work; mean579.781ms,
max874.301ms, RSS490.107MB. All12guidance cases and three exact controls pass
independent audit. Other03/04guidance iteration counts lose. Seven frozen05repeat
and paired-seed runs are submitted; no fresh-input qualification is implied.
[Full evidence](results/random345-flow-iterations-split-full-v162/REPORT.md).
