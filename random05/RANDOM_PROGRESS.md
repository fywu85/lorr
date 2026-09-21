# All RANDOM instances: throughput progress

Started 2026-09-21T02:17:27.230561+00:00. The user expanded the campaign to RANDOM-01..05;
RANDOM-05 now reaches4,175 on the archived input; the preceding4,011 configuration
repeated exactly and passed frozen fresh validation. New validation is pending.
Large maps are outside active development.

## Comparison rules

- Full archived horizons600/600/800/1000/2000;32 workers on16 physical EPYC9354 cores,1s entry,30s preprocessing,32GB guard.
- Throughput primary; retain completed/unfinished waiting statistics for promoted records.
- Keep general algorithm/configuration trials separate from instance-specific tricks. Every trick needs the matching `--trick RANDOM-0N`.
- Apply general variants across all five densities before claiming broad benefit. Selecting different settings by instance name counts as a trick. Geometry-independent density rules remain hypotheses until tested.
- Preserve all original failures. Selected maxima, paired planner-seed means and fresh task/start validation are distinct.
- The five cases share one layout; improvements across densities do not establish transfer to unseen geometry.
- Inputs50001–50012 remain excluded from tuning. Future fresh-input protocols must precede generation.

## Current records

| Instance | Robots | Steps | General profile | Trick profile | Matched NMS32 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 | 100 | 600 | 726 | 729 | 649 |
| RANDOM-02 | 200 | 600 | 1376 | 1401 | 1228 |
| RANDOM-03 | 400 | 800 | 1582 | 2602 | 2359 |
| RANDOM-04 | 700 | 1000 | 1558 | 2777 | 2580 |
| RANDOM-05 | 800 | 2000 | 2226 | 4175 | 3172 |

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
