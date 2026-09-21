# All RANDOM instances: throughput progress

Started 2026-09-21T02:17:27.230561+00:00. The user expanded the campaign to RANDOM-01..05;
RANDOM-05 still targets4,000. Large maps are outside active development.

## Comparison rules

- Full archived horizons600/600/800/1000/2000;32 workers on16 physical EPYC9354 cores,1s entry,30s preprocessing,32GB guard.
- Throughput primary; retain completed/unfinished waiting statistics for promoted records.
- Keep general algorithm/configuration trials separate from instance-specific tricks. Every trick needs the matching `--trick RANDOM-0N`.
- Apply general variants across all five densities before claiming broad benefit. Selecting different settings by instance name counts as a trick. Geometry-independent density rules remain hypotheses until tested.
- Preserve all original failures. Selected maxima, paired planner-seed means and fresh task/start validation are distinct.
- The five cases share one layout; improvements across densities do not establish transfer to unseen geometry.
- Inputs50001–50008 remain excluded from tuning. Future fresh-input protocols must precede generation.

## Current records

| Instance | Robots | Steps | General profile | Trick profile | Matched NMS32 |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 | 100 | 600 | 726 | 628 | 649 |
| RANDOM-02 | 200 | 600 | 1370 | 1122 | 1228 |
| RANDOM-03 | 400 | 800 | 1582 | 2171 | 2359 |
| RANDOM-04 | 700 | 1000 | 1558 | 2462 | 2580 |
| RANDOM-05 | 800 | 2000 | 2226 | 3978 | 3172 |

The initial records come from the [frozen transfer](GENERALIZATION.md). The
RANDOM-05 trick record includes subsequent development; its complete history
continues in [RANDOM05_PROGRESS.md](../RANDOM05_PROGRESS.md). Configurations,
hashes and evidence are pinned in [random-frontiers.json](random-frontiers.json).

## First declared experiments

Source79 (`9e9dbfa`, build79) runs23 full cases. Three general variants use
noise50, dispersion0, or both, identically across all five densities. They test
whether RANDOM-05 exploration and crowd-spreading settings overshoot lower
densities. These are general configuration experiments, not asserted gains.

Separately, RANDOM-01..03 get a horizon-only trick with guidance still off.
RANDOM-04 receives individual tests of contrast1.6, dispersion0.4, noise100,
move bias2 and a larger K24480 search. Other settings are frozen. Each such
run has its explicit instance flag. The RANDOM-05 move-bias experiments continue.

Further work will consider a windowed multi-step optimizer for sparse/medium
traffic if tuning the reactive policy leaves a structural gap. No such optimizer
has been added yet.

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
