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
The user subsequently authorized shared GRID hosts and a5-second complete-entry
deadline for development, with fixed work and the32-billion-byte process limit.
The best candidate will receive a later exact one-second validation.

The shared200-step screen passed on both arms, with maxima898.487/893.222ms;
the unchanged control reproduces its exact reference trajectory and all first128
steps match before publication. Full seeds0/2 × OFF/ON completed under job8899305, verification8899306 (results below). Both baseline and candidate use the
same frozen binary/source4524843 and sixteen distinct physical cores. Record full
totals, per1000windows, task age, empty work and loaded turns/waits. The screen is
feasibility evidence only; the full exploratory result below remains unpromoted.
[Screen verification](results/remaining-flow-screen-v56-shared/comparison.json).

The preceding traffic replay rejects simple short-chain pooling as the next
candidate. Full static-lane TRICK results have little late-rate gain, weakening
an explanation based only on the learned field; coupled assignment/scoring
effects remain possible. [Fable review and qualifications](fable-flow-session/turn26/assessment.md).

## Full exploratory result

Verified2026-09-20T05:29:41UTC. Both unchanged controls reproduce their historical
full trajectories exactly; all26 source/test hashes and the frozen executable match
4524843. All20,000complete entries are valid under the5s development budget.

| Seed | OFF | ON | Difference | Final1000 difference | Age p90 difference |
|---|---:|---:|---:|---:|---:|
|0|144510|145083|+573|+72|−2|
|2|144107|144215|+108|+112|−4|

The paired mean gain is0.235953%.145,083 is the new generic single-run record,
completed2026-09-20T05:28:00.961539UTC, but the policy is not promoted yet. Both ON
runs reduce loaded waiting; turns rise and empty work rises, particularly on seed2.
These component associations do not establish an additive causal saving.
Candidate mean latency428.778–437.432ms, observed max1004.262ms, peakRSS11.340GB,
CPU1.605–1.663cores/4 reserved, full duration36.85–37.57minutes. Shared-host results
do not certify the1s competition limit.

The remaining seeds1/3/4/5 × OFF/ON run concurrently as8899325, held analysis8899327,
same V56 binary and source. No reference-profile change occurs until all six seeds
are assessed. [Full pair and provenance](results/remaining-flow-full-v56-shared/comparison.json).

## Full six-seed outcome

All 12 full candidate/control runs are independently valid at the authorized shared
5s budget, with exact old control trajectories. Candidate totals at seeds 0..5 are
145083 / 144539 / 144215 / 142341 / 144157 / 144026. Paired differences are
+573 / +606 / +108 / -793 / +223 / -3. The mean is 144060.1667 versus 143941.1667:
+119 tasks (+0.08267%), four wins and two losses. The four confirmation seeds add
only 8.25 tasks on average; seed 3 loses 417 completions in its final 1000 steps.

Keep OFF. The selected 0/2 improvement did not generalize convincingly to the four
remaining planner seeds. The 145083 single-run record remains valid and visible,
but it does not justify replacing the confirmed regional policy. These seeds use
one fixed instance/task stream, not six independent workloads. [Combined report](results/remaining-flow-six-seed-v56-shared.json),
[confirmation](results/remaining-flow-confirm-v56-shared/comparison.json).
