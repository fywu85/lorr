# Current-reference forward-blocker audit

Verified 2026-09-20T01:18:14.835419UTC. Full GRID runs8899240 and analysis8899241
completed. V50 plus the existing read-only audit option reproduces the confirmed
seed0/2 trajectories **144,510 /144,107** exactly, including all sampled search
counters. This is a diagnostic result, not a throughput improvement.
[Complete audit and verification](../results/current-conflict-audit-full-v50/audit.json),
[collector](collect_current_conflicts.py), [configuration](current-conflict-audit-variants.json).

All22 source/test hashes match exact commit
`cb6a666da8aee461d5a34cab01d7b7ccaa76a201`; binary hash, concurrent disjoint physical
bindings, absence of CPU quotas, 10,000 complete entry timings and50million robot
steps per run pass verification. Mean entries406.30/384.27ms, maxima928.438/884.379ms,
peak RSS11.334GB, average CPU1.691/1.747 cores of four reserved, full wall35.40/33.59min.
These runs do not establish a controlled runtime speedup.

## What the final plan still blocks

The audit samples the final post-repair reservation plan every67steps,74frames per
seed. It considers nonfixed, positive-power robots whose selected first step stays
in their current cell. Such robots include useful rotations, not only waits.

The table classifies the easiest first-forward candidate that improves the
heuristic travel/turn potential by at least one unit after removing the operation
preference term. "Physical" in the raw report names this score decomposition;
it is still a heuristic with published guidance, not measured future travel saved.
Units below are robots per snapshot, over15samples in the final1,000steps.

| Candidate / blocker category | Seed0 | Seed2 |
|---|---:|---:|
| All eligible stationary roots | 808.67 | 827.07 |
| No qualifying forward candidate | 319.93 | 322.27 |
| Qualifying candidate with no other reservation owner | 2.80 | 3.33 |
| One movable owner | 322.93 | 332.20 |
| Two movable owners | 140.33 | 141.00 |
| Three or more movable owners | 22.60 | 28.27 |
| Protected owner | 0.07 | 0.00 |

Every root belongs to exactly one category. Counts after the initial1,000steps
are similar: approximately2.5–3.1 unblocked,325.7–335.7 one-owner,140.0–143.1
two-owner and23.0–27.5 many-owner roots per sample. The full74frame series and
three score classifications are preserved in the audit.

By contrast, accepting *any scalar improvement* gives39.1/40.1 unblocked roots
in the last window. Most of those apparent opportunities disappear after removing
the operation-preference contribution and requiring a full potential unit.
Do not equate raw scalar-score improvements with physical progress.

## Consequences and limits

The current policy leaves few immediately conflict-free, first-forward candidates
with a full potential improvement. That weakens the case for merely adding a
cheap pass that picks such free moves. Most qualifying candidates conflict with
one or more other reservations. "Movable" only means not protected; it does not
show that the owner can be displaced, that a complete joint repair improves the
objective, or that taking it improves long-run throughput.

A discriminating motion probe would search complete joint alternatives on the
same frozen states, goals, five-slot candidate sets and protected/outside
reservations, and record what the existing one-owner constructor cannot realize.
It should distinguish same-size random groups from obstruction-selected groups
at prescribed work. The earlier extra two-owner transaction pass already lost
on six seeds; these counts alone are not grounds to reintroduce it.

This audit does not include failed-attempt traces or region-boundary membership.
It cannot establish that geometric boundaries or the five-slot horizon bind, nor
provide a causal upper/lower bound on the remaining throughput gap. The pending
KK weight/rematching factorial will help choose between traffic guidance,
assignment interaction and a larger motion intervention.
