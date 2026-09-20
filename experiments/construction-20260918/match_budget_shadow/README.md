# Read-only matching-budget diagnostic

This experiment changes no assignment policy. `CGAR_MATCH_BUDGET_AUDIT_STRIDE=10`
requires diagnostics and enabled unopened matching. Every10ticks, after the real
matching pass, reuse the same bounded groups and Hungarian/cycle evaluator with
its own cursor, counters and witness ledger. Include holders excluded only by the
one-retarget-per-task budget. Keep primary/next-primary, recovery, commitment,
cooldown, fairness and started-task exclusions. Read only complete resident tables.

The shadow returns before the real commit block, and a copied proposal must remain
identical. It never writes simulator tasks, real assignments, cooldowns, commitment
state, real budget records, cache recency or RNG state. Its extra fixed work must
complete inside the development deadline; a timeout fails the whole entry.

Count only accepted cycles containing at least one budget-protected task. Accept
a witness into the diagnostic ledger only if NONE of its task IDs has appeared
in a previous counted witness; then mark ALL participating tasks. Overlapping
cycles are skipped in full. This is a declared chronological sample of disjoint
witnesses, not a global optimum or a complete estimate of lost opportunities.
Task IDs are unique for the run. The audit keeps its ledger for that entire run.

Report cumulative exposed/eligible/group work, ordinary and budget-containing
accepted cycles, duplicates, unique witnessed rows, protected rows, and savings.
V84 also separates wholly protected witnesses from mixed cycles; a mixed cycle's
entire saving cannot be attributed to its protected rows alone.
Savings are native guidance units; divide by20 only to obtain forward-base units,
never physical robot steps. Compare unique excluded S2 with actually accepted S1
at the SAME sample tick, both per real assignment including the initial10000.
At stride10 a ratio is only descriptive guidance accounting; matching cadence
does not remove bounded-group coverage, chronological deduplication, or the fact
that no shadow cycle is executed. It is neither complete nor an upper bound.
V83 stride100 archives are sparse snapshot sums; do not use their ratio as a
matched exposure rate. New reports omit the ratio for stride100. Exact unshadowed trajectories are mandatory.

Planned checks: production regressions with a nonzero second-cycle witness,
primary/started/cooldown protection, task-level deduplication, and two128tick
closed-loop pairs with matching+short ON. Then four native200-step shadow profiles
(shortOFF/ON xweight1/5) must match V81 exactly. The refactored real matching path
also receives its generic800-step reference check with the audit OFF.

A full diagnostic, if warranted after those checks, uses only the two weight5
profiles at seeds0/2: four concurrent processes,4boundphysicalcores each,32GB RSS,
shared5s development. Their exact references are the already-frozen V80 full short
benchmark using source85faca5. Its final result hashes are still pending and will
be frozen into the verifier once available. No diagnostic run enters the frontier.
Budget2 is NOT implemented or promoted. It remains conditional on full short-arm
motion/chain results and substantial excluded opportunity.

V84 full-cadence plan declared before submission: use the stride10 variant files,
with the protected-subset counter, and require exact V81 startup/V80 full controls.
V83 stride100 evidence remains archived unchanged. No production policy changes.
