# Bounded temporal displacement transactions, v36

The expanded audit finds about 171–175 stationary robots per sampled late-run
step whose easiest full-unit forward improvement touches two other robots. That
is an opportunity count, not a throughput ceiling or proof that both can move.
The existing temporal search follows one blocking owner at a time.

The optional transaction pass works on a separate complete reservation table.
It considers stationary roots and forward choices that strictly improve the
scalar score and improve its physical component by at least one distance/turn
unit after removing operation-index terms. A candidate may displace at most one
or two non-fixed owners. Their relocations are solved together by deterministic
backtracking. Placed robots are locked within that transaction; pending robots
have no reservations and must all be reinserted before acceptance.

An undo journal restores selections, active state, locks and reservations after
rejected alternatives. Score snapshots avoid rollback drift. Acceptance occurs
only after all displaced robots are assigned and the complete scalar score
strictly improves. This does not imply a throughput gain or a non-decreasing
physical-only score for the whole fleet. The final result is reconstructed by
the existing reservation validator and independently checked by the adapter.
Pinned primary, recovery, pocket and supporting robots remain unchanged.

The input plan is immutable. A callback exception propagates and no experimental
result is published. The entry layer still rejects a complete decision that
misses its deadline; it never selects a result according to elapsed time.

`CGAR_TEMPORAL_BRANCH_WORK` defaults to 0 (off), with an allowed range 0–2,000,000
candidate inspections. `CGAR_TEMPORAL_BRANCH_OWNERS` is 1 or 2 (default 2).
Enabled passes inspect at most 256 eligible roots, with at most 2,048 candidates
per root and eight involved robots per transaction. A deterministic rotating
cursor chooses the starting point. Exhausted branches are rolled back completely.
All settings are generic; no map category or geometry template selects behavior.

Build **8898706** passes all regression checks. Independent fixtures require
backtracking after one relocated sibling blocks the other's only escape, reject
aggregate-score losses, test three work cutoffs, protect fixed owners, reject a
missing escape, propagate an injected exception, and replay all vertex/swap
constraints. Production fixtures cover 9,600 actions across serial/parallel
preparation, regional repair and protected primaries. The source archive's patch
reconstructs every requested SHA-256.

Screen **8898715** passes all three 200-step cases. Disabled mode exactly
reproduces the previous complete prefix. Maximum entries are 0.771 / 0.784 /
0.772 seconds for disabled / one-owner / two-owner modes. Logged work stays
within 65,536 candidates and eight involved robots; complete scores never fall.
The changed short prefixes are not used to rank throughput.

Full **8898717 / 8898718** compares disabled, one-owner and two-owner modes on
seeds 0/2, all 5,000 steps. Six independent instances run on exclusive research50
with 24 physical EPYC9354 cores and 72 GiB aggregate reservation. Each planner
uses four disjoint cores, complete one-second decisions and its own 32 GB RSS
validity limit. All six full outcomes are now independently analyzed and valid.

| Owners | Seed0 | Seed2 | Mean | Versus disabled |
|---|---:|---:|---:|---:|
| Disabled | 134,511 | 134,519 | 134,515 | — |
| One | 134,661 | 135,074 | 134,867.5 | +0.262% |
| Two | 134,584 | 135,357 | 134,970.5 | +0.339% |

Both disabled trajectories exactly reproduce v30. Two-owner versus one-owner
adds only0.0764% to the paired mean and loses on seed0. One-owner itself adds
sibling backtracking, recursive relocation and terminal acceptance, so its
contrast with disabled is not solely an owner-count comparison. Sampled passes
use the complete65,536-candidate allowance and commit roughly0.88–2.28 roots
per step. Mean sampled pass cost is3.59–4.25ms. These samples do not count every
step. See [comparison](results/temporal-transaction-full-v36/comparison.json).

All entries stay below0.780seconds and RSS below11.78GB. The new peak135,357
is from exact production source45ebea2, finished14:13:43UTC. Full confirmation
on seeds1/3/4/5 is running as8898725 /8898726, eight concurrent cases with the
matched disabled control. No six-seed improvement claim yet.

[Source archive](build-provenance/v36/),
[regression and screen evidence](results/temporal-transaction-screen-v36/),
[full configuration](temporal-transaction-variants.json),
[preceding audit](FORWARD_CONFLICT_AUDIT.md).
