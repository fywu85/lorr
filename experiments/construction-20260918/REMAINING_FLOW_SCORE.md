# Generic remaining-flow score experiment

`CGAR_TEMPORAL_REMAINING_FLOW=1` omits the paid forward-toll extra from the
temporal candidate objective. It keeps the same learned remaining-distance
field, physical turn price, operation preference, scheduling, work quotas,
protected primary and complete-decision deadline. Default0 retains the existing
score. No map name, imported lane field or coordinate pattern is used.

This changes the surrogate deliberately. A tolled edge can receive more local
progress credit, including when its route has regret smaller than the omitted
toll. This may help robots leave costly regions or may encourage opposing
traffic. Neither outcome is established. Paid-progress interpretations do not
apply to the experimental score.

The first experiment requires temporal planning, orientation guidance and learned
flow. It rejects static tricks, guide routes, next-errand scoring, paid-progress
audits and branching transactions. It cannot activate a map-specific policy.
Those still require `--trick <map-instance-name>`; this candidate is GENERIC.

The certified PIBT pass retains its paid metric, and protected robots keep their
certified choices. The ablation only changes the later temporal objective. Paid
turn extras are deliberately retained even when a non-unit turn price is selected.
The benchmark holds turn parameters fixed at the reference values.

Regressions independently replay legal five-action macros using Bellman-derived
distances, test first-service toll cutoff and neutral/prepublication equivalence,
and exercise serial/parallel preparation, protected primary, recovery and capacity.
An active-toll fixture also requires a changed production decision, valid actions
in both arms and service of both initial primary goals.
Replacement build8899285 passed the complete regression suite at04:18:55UTC.
It verified25,884 independent macro scores,3,840 neutral production actions,
11,520 active-field collision checks, service of both fixture primary goals,
4,800 regional actions and1,920 serial/parallel worker decisions. The active
fixture changed decisions on116 steps. All26 frozen source/test hashes match.
Original build8899283 was cancelled before execution to strengthen coverage.
The build used four bound physical cores; the archived allocation-change receipt
records a request that raced job start. This build makes no performance claim.
The warehouse benchmarks continue to require exclusive hosts.

After a valid200-step feasibility screen, compare full seeds0/2 against unchanged
controls on isolated EPYC9354 allocations, four physical cores per process,
one second per complete entry and32-billion-byte process RSS. Record full totals,
per1000windows, task age, empty work and loaded turns/waits. Only full valid gains
can justify six-seed confirmation. No new result or promotion is claimed.

The preceding traffic replay rejects simple short-chain pooling as the next
candidate. Full static-lane TRICK results have little late-rate gain, weakening
an explanation based only on the learned field; coupled assignment/scoring
effects remain possible. [Fable review and qualifications](fable-flow-session/turn26/assessment.md).
