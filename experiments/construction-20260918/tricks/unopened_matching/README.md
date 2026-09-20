# TRICK: Warehouse unopened matching interaction

This explicitly flagged experiment asks whether bounded rematching can reduce the
extra empty travel caused by short-task preference. It keeps the CGAR planner and
uses the existing NMS-derived static lane field. It is not a generic result.

Activation requires `--trick WAREHOUSE`. `CGAR_TRICK_LANES=1` is fixed in the
factorial; `CGAR_TRICK_SHORT_TASKS` and `CGAR_TRICK_UNOPENED_MATCH` each vary 0/1.
Matching uses `CGAR_REASSIGN_MATCH_GROUPS=64` when enabled. The generic matching
selector is still rejected under TRICK. Missing the CLI flag is rejected even for
an explicitly set zero-valued trick environment selector.

Matching preserves started tasks, primary/next-primary protection, recovery,
commitments, and applicable fair admissions. It may retarget each unopened task
once; cooldown 20, interval 10, group size 32, BFS limit 2,048 per group. It uses
resident complete oriented fields without extra table builds or random draws.
Static lanes activate the metric after timestep zero. Short-task preference can
defer long unpicked tasks indefinitely, so a completed cohort can change even
though each matching pass preserves its currently held task set.

Source [5a8a51a](https://github.com/fywu85/lorr/commit/5a8a51a),
[exact build binding](../../build-provenance/v65/exact-source-commit.json),
[regression checks](../../build-provenance/v65/regression-result.txt).
The [200-step screen](../../results/trick-match-screen-v65/comparison.json)
passes all four cases and exactly reproduces the earlier lane and lane+short
controls. This is feasibility evidence only. The full paired comparison uses
`variants.json`, seeds 0/2 and the complete 5,000-step horizon.

`analyze.py` freezes its support and independently verifies all source/test hashes,
binary, explicit trick components and field hash, simulator validity, every entry
deadline, RSS, actual disjoint physical-core bindings, prescribed search work,
matching bounds, exact full controls, task totals, tails, ages and empty travel.
Shared hosts and the 5-second development budget are labelled explicitly; no
partial result from a failed case qualifies as a score.
