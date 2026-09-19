Review a bounded new CGAR experiment independently. Use only Read, Grep, Glob, no commands, edits, delegation, or secret inspection. The user explicitly requested Fable 5.1 max through Claude Code CLI.

Read the frozen build v11 sources in runs/cgar-temporal-build-v11-20260918/sources/cgar/, especially:
- cgar_planner/temporal_regions.hpp (new parallel regional repair)
- cgar_planner/temporal_pibt.hpp (selected-state import, root subsets, region candidate restriction)
- cgar_planner/temporal_adapter.hpp (protected intent integration and complete-result publication)
- cgar_planner/temporal_geometry.hpp (weighted-turn scoring)
- cgar_planner/cgar.cpp and cgar.hpp (TurnDistanceOracle and configuration)
- tests/cgar_regression.cpp (independent weighted Dijkstra, score bounds, regional serial/parallel equivalence, boundary/fixed paths, timeout injection).
The preceding frozen kernel is under runs/cgar-temporal-build-v8-20260918/sources/cgar/. You may read experiments/construction-20260918/RESULTS.md and fable/assessment.md for context. Do not recursively scan unrelated large outputs or binaries.

Contract: generic map-independent policies; full 5000-step warehouse evaluation; <=1 second per complete decision, timeout rejects the entire decision; <32,000,000,000 bytes measured process memory. No elapsed-time partial result selection. Protected CGAR primary/pocket/transaction/capacity behavior must remain intact. Finite regression checks are not a liveness proof.

Weighted turns use integer reverse Dijkstra with a bucket queue and apply the extra cost consistently to real and terminal-wait rotations in temporal scores. Turn cost 1 must preserve preceding trajectories. Costs 2 and 4 passed 50-step deadline-only checks; cost 8 failed explicitly. Full results are pending, so no performance claim for weighted turns.

Regional repair starts from one complete plan. Each round partitions by dimensions only. Any input path crossing a boundary is frozen for that round. Active roots and candidates stay inside one region; every search sees full input reservations and freezes all outside robots. Seeds and work counts are fixed, all workers join, any exception propagates. Selected changes are merged only after workers finish, and complete reservations are rebuilt to detect conflicts. Boundaries shift between complete rounds. No region profile has a full performance result yet.

Prioritize concrete correctness defects, races/lifetime issues, safety gaps, nondeterminism, incorrect action-cost scoring, deadline failures returning partial success, and important missing adversarial tests. Check boundary edges and geometric wrap, selected-state import, immutable candidates/masks, rollback and merge semantics, score accounting, and weighted-table consistency. Provide file/line references, precise reasoning or counterexamples, and minimal fixes where needed. Also assess whether this experiment can make productive use of multiple cores, without claiming it improves throughput before measurements. Distinguish proven defects from hypotheses. Do not propose map-specific lane templates. End with the most informative next bounded validation, if needed.
