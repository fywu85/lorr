Please perform an independent, read-only technical review of the CGAR/PIBT migration for the 2024 League of Robot Runners competition in this repository. The user explicitly requested Fable 5.1 with maximum effort through Claude Code CLI.

Goal: Diagnose the remaining throughput gap and propose concrete improvements that do NOT exploit specific maps. No map-name dispatch, hard-coded coordinates, handcrafted lanes, special aisle orientations, or per-map weights. Algorithms may use the graph, task data, observed traffic, and workload size uniformly across all maps.

Review the source yourself before relying on previous conclusions. Use Read, Grep, and Glob only; do not edit files, run programs or benchmarks, create other agents, inspect credentials or unrelated files, or publish anything. Return the review in your final response; the calling process will save it.

Primary sources to inspect:
- cgar/cgar_planner/cgar.cpp and cgar.hpp, especially scheduling, distance caching, task/stop epochs, primary selection, recovery, and action conversion.
- cgar/src/TaskScheduler.cpp, MAPFPlanner.cpp, Entry.cpp, and cgar/tests/cgar_regression.cpp as needed.
- experiments/throughput-20260917/build_probe.py and probe-v2.patch: the isolated fallback sampling and hybrid scoring experiments. Production sources have NOT received these experimental changes.
- experiments/throughput-20260917/summary.json, results/*.json, initial-cohorts.json, trajectory-summary.json, metadata.json, and validation.txt: compact records from fourteen completed single-run diagnostics.
- experiments/throughput-20260917/README.md and independent-review.md: earlier interpretations, to be challenged rather than treated as authoritative.
- benchmarks/mr24-20260917/summary.md: the preceding full-horizon ten-instance comparison.
- Selected competition sources, if useful: kk and nms are relative links to the vendored KittyKnight and No Man's Sky submissions. Inspect only relevant routing/task-scheduling code; distinguish code that actually executes from unused implementation patterns.

Please address:
1. Which findings are supported causally by the experiments, and which remain hypotheses? Examine the fallback-only gains and the combined hybrid branch's RANDOM-01/CITY-01 regressions. Preserve the single-run, short-horizon, wall-clock/cache, and initialization confounders.
2. Any concrete bugs or weaknesses missed in the earlier diagnosis, particularly flaws in the proposed sampling implementation, scheduling fairness/coverage, time budgets, exact-vs-approximate distance use, and recovery progress. Give source paths and line numbers, severity, a reproducible reasoning example when possible, and the practical consequence. Do not invent experiments or performance numbers.
3. A prioritized implementation plan: first safe production patch, stronger generic scheduler design, and possible routing improvements. Separate independently testable changes, and name validation or ablations needed before adoption.
4. Whether the observed loss can plausibly be recovered while preserving the corrected persistent-primary and validated-witness mechanisms. Do not treat simulator success as an end-to-end liveness proof or claim a full-suite win against the leaders without evidence.

Please produce a clear review in roughly 1,200-1,800 words, leading with the most consequential findings and identifying where you agree or disagree with the earlier report. No source modifications are requested.
