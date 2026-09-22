# RANDOM-01/04 horizon diagnosis and user-requested pause

Source [e30c20b1](https://github.com/fywu85/lorr/commit/e30c20b1ccc8291c83a73066835c0db6c49f3e5b) adds an optional, default-off suppression fingerprint and a read-only trace probe. It changes no planning objective, decision arithmetic, search budget or task assignment. Full regression passes in64.05seconds, including diagnostic on/off action and checkpoint equivalence.

Both full instrumented controls pass strict1,000ms entries,30,000ms preparation,32 decimal GB and independent movement/assignment/task-event replay on16 bound physical EPYC9354 cores/32 workers. All six complete trace fields exactly match the previously selected controls. Every native per-step suppression count and active-mask fingerprint agrees with offline reconstruction; phase/action accounting also matches the independent replay. No fresh input is used.

| Case | Tasks | Matched max(NMS, KK) | Lead | Mean / max entry ms | Peak RSS MB | Masks checked |
|---|---:|---:|---:|---:|---:|---:|
| RANDOM-01 | 745 | 692 | +7.66% | 70.618 / 113.104 | 371.683 | 600 |
| RANDOM-04 | 2799 | 2649 | +5.66% | 463.378 / 665.535 | 690.373 | 1000 |

These are diagnostic exact repetitions, not new throughput records or frontier replacements. Original source/settings/timestamps in the selected record manifest remain unchanged.

## Observations

RANDOM-01 finishes with8opened orders unfinished;7were ever suppressed. All112suppressed loaded robot-steps concern orders whose optimistic physical remaining work already exceeds the remaining time. No eventually completed order was suppressed. Only565active loaded robot-steps in the entire run concern orders unfinished at the end. The final100steps contain3117idle robot-steps. This provides little support for loosening the existing suppression rule as the main next improvement; it does not prove an inherent ceiling.

RANDOM-04 finishes with119opened orders unfinished;115were ever suppressed. Of the terminal119,21have at most5physical actions remaining and24have at most10; the median is44. The run spends17158active loaded robot-steps on orders unfinished at the end. Another9343loaded robot-steps on unfinished orders are suppressed:4259have a fitting physical lower bound and5084exceed it. Six eventually completed orders were briefly suppressed, for26robot-steps total. At step900,39of44suppressed opened orders have a physical lower bound that fits the remaining100steps. This distinguishes potentially over-conservative cutoff decisions from traffic delays, but does not quantify recoverable throughput.

The physical bound ignores every other robot and permits zero-time repeated waypoints. A fitting bound is necessary, not sufficient, for completion. The retrospective outcome labels describe this exact trajectory; changing any decision also changes traffic and later assignments. No causal gain, task-recovery count or throughput ceiling is claimed.

## Completed diagnostic checkpoint

The user requested a wrap-up and stop; all diagnostic work is complete. A subsequent request specifically authorizes more planner seeds to estimate throughput standard deviations. That bounded statistical measurement precedes the final pause; no algorithm development or parameter search is resumed. All selected bests remain745/1408/2646/2799/4302 against matched maxima692/1256/2359/2649/3172. Earlier failures and incomplete fresh/runtime qualifications remain recorded.

[Independent audit](audit.json), [exact controls](control-equivalence.json), [all1600 mask checks](triage-verification.json), [frozen protocol](../../experiments/random14-triage-diagnostic-v170-protocol.json), [build proof](../build-v170/regression.json).
