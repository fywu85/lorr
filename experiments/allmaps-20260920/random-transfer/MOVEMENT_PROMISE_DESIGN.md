# Next bounded general transfer: preserve a movement decision after turning

Status: implemented and fully tested; all ten full seed0 benchmark cases verified.
No winning setting; the prototype remains OFF in selected profiles. Motivated by the read-only
[actual-action audit](rotation-audit-v1/audit.json) and the reference's movement
pipeline, not a claimed causal explanation of the whole throughput gap.

In the2574-task R05 run, there are583740 turns and453960 forwards. Excluding
adjacent task/goal/assignment changes conservatively leaves105055 immediate
inverse-turn pairs (18.22% of turn-predecessor transitions). Only35.25% of those
turns are immediately followed by forward motion. Generic R04 has12.77% immediate
reversals. The stronger R05 field still completes more tasks with fewer forward
moves than the generic control; raw motion count is not itself the objective.

A narrow first prototype can reuse the existing verified TemporalWarmStart
history, without implementing the reference's whole two-step pipeline:

1. For an ordinary robot whose previous selected path began with a planned turn,
   propose the next action from that complete path's shifted suffix. Require
   exact next-step position/heading, unchanged goal and a currently legal suffix.
2. Keep CGAR primary/recovery/pocket/witness paths authoritative. Reuse the
   existing monotone collision-reset closure before retaining any promise.
   Robots with no eligible promise reset to the ordinary compatible seed.
3. Start retained robots at a nonzero compatible suffix and restrict searchable
   alternatives to its first action. Keep the ordinary seed at index zero solely
   as the unchanged score reference; search alternatives and rollbacks cannot
   return a promised robot to that unconstrained seed. This preserves the score
   offset used by annealing and the diagnostics' current-cell convention.
4. Validate that every final promised choice is still nonzero and has the
   promised first action. Ordinary-seed auto-rotation cannot run for it. Keep
   operation IDs, candidate cost order, actual expected headings, all five-slot
   collision checks and complete fixed search work. A deadline is an error.
5. Disable ordinary warm/mixed starts in the first prototype to isolate the
   constraint. Default OFF must preserve complete historical trajectories.

Meaningful tests: a completely occupied four-robot rotation cycle, a protected
seed that resets the dependent promise cycle, goal/state mismatch, no history,
wrong timestep, deadline failure, and serial/parallel action equivalence. Report
retained/reset counts. Evaluate full matched controls at1s on both maps.

This is a restricted one-step promise after a planned turn. It is not the full
reference pipeline, multi-future evaluation, or a new liveness proof. If it loses,
retain the negative result before designing a more faithful rollout transfer.

Selector: `CGAR_TEMPORAL_PROMISE_AFTER_TURN=1`, default OFF, strict boolean.
This general mechanism uses no map name, density threshold or horizon. It
excludes ordinary-seed auto-rotations, retaining only planned rotations. The
first prototype does not coexist with warm/mixed starts. Cumulative activation,
retention and collision-reset receipts are separate from warm-start counters.

Current test coverage includes full occupied forward and delayed-forward cycles,
protected/goal-change dependency reset, unchanged score reference, sorted legal
alternatives, non-turn exclusion, stale state, timeout propagation, disabled
trajectory equivalence, and serial/parallel production episodes with protected
primary paths and guidance updates. All fixtures and the complete existing suite passed on four bound GRID cores.

## Predeclared full-horizon comparison

Source aa63d48; build and complete tests passed. Four physical cores per case, exact archived
MR24 inputs, planner seed0, complete1000/2000 steps, enforced1000ms decisions,
32decimalGB, shared EPYC9354 hosts. Variant settings are frozen in the adjacent
`promise-*-variants.json` files before runs begin.

* Generic RANDOM-04: selected1503-task control versus after-turn promises.
* RANDOM-04 TRICK: field +direct pickup4, matching OFF (prior1481 seed0), a2x2
  comparison of promises and explicit short-task preference. This revisits the
  separate trick branch; it does not replace the selected generic profile.
* RANDOM-05 TRICK: field +direct pickup4 +matching64 (prior2574 seed0), the same
  2x2 comparison. The current individual record2608 is seed2, not this seed0
  control. Compare paired seed0 results first, then replicate any improvement.

`CGAR_TRICK_SHORT_TASKS=1` here removes forced-oldest admissions. HRRN was already
OFF in these controls, so that setting does not silently bundle an aging change.
Started ownership and CGAR motion protections stay intact. Report throughput and
waiting tails without imposing a fairness veto. Every trick matrix requires the
correct explicit `--trick RANDOM-04` or `--trick RANDOM-05`.

The existing known-horizon scheduler requires an unrestricted full core, which
these pocket-bearing instances do not provide. It cannot simply be enabled here;
a future horizon transfer must use a valid free-space distance bound or a clearly
labelled heuristic. Keep that change separate from this controlled comparison.

## Complete measured outcome

Source aa63d48cdb75f61774474faad025f049cc631f76; binary
9fd0c126b7546880e08bce1e646f487d4f15cccf47a9e7a9f60e4964a111b82d.
All10 full cases passed1000ms and32decimalGB checks. Maximum entry226.56ms.
All three disabled controls exactly reproduce their previous complete
trajectories. [Build proof](promise-build-v1/checks.json),
[paired data and control equivalence](promise-summary.json).

| Full seed0 case | Control | Promises | Short preference | Both |
|---|---:|---:|---:|---:|
| RANDOM-04 generic |1503|1388|—|—|
| RANDOM-04 field +direct pickup4 |1481|1392|1346|1334|
| RANDOM-05 field +matching +direct pickup4 |2574|2548|2539|2546|

No variant improves its paired control. Keep the selected generic RANDOM-04 and
field-guided RANDOM-05 profiles unchanged. These single-seed negatives do not
prove that all forms of pipelining or less-fair scheduling lose. The movement
constraint is much narrower than the standalone algorithm's fleet-wide two-step
pipeline; compatibility resets discard most candidate after-turn suffixes.
A faithful joint-prefix or multi-future mechanism remains a separate experiment.

Full result details:
[generic RANDOM-04](../results/random04-promise-generic-full-v1/summary.md),
[field RANDOM-04](../results/random04-promise-trick-full-v1/summary.md),
[field RANDOM-05](../results/random05-promise-trick-full-v1/summary.md).

The [actual-action audit](promise-rotation-audit-v1/audit.json) confirms that
promises reduce stable-goal inverse turns:12.77%->11.14% on generic RANDOM-04,
16.43%->14.24% on field RANDOM-04,18.22%->16.76% on field RANDOM-05. Turn-to-forward
transitions increase slightly. Throughput still falls. Therefore reducing this
local symptom alone is not a successful throughput objective. Exact eligible
promise retention/reset totals are retained in the paired summary; compatibility
with the reset ordinary fleet cancels most proposed suffixes.
