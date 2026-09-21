# Next PILOT transfers into CGAR

The user explicitly requested PILOT reference on 2026-09-21. This review reads
frozen source and recorded settings only. The independent solver, its logs,
builds, jobs and held-out task streams remain untouched. PILOT throughput is not
CGAR throughput. Source identities are in [source-review.json](source-review.json);
selected settings are in [reference-records](reference-records-20260921.json).

The updated goal is at least 10% above max(published NMS, KK) on RANDOM-01–05,
and an aspirational 5% on the other five instances. GAME currently clears its
new target. Other instances still need work; Warehouse is explicitly a stretch.

## What the latest source changes about the diagnosis

PILOT's selected RANDOM-01/02 methods use a 20-action rolling window and **no
traffic field**. Their retained prefixes are 6 and 10 actions. RANDOM-03 uses a
20-action window, 14 retained actions, tuned flow guidance and annealed LNS.
Those are longer coordinated paths than CGAR's current five-action pool; simply
adding more independently randomized five-action starts does not transfer them.
The new CGAR four-start RANDOM-03 comparisons all lost to its one-start control.

`window.cpp` evaluates complete paths by paid action cost plus exact remaining
chain potential. Time-space A* includes task stage in its state and consumes only
one stop per physical action. It repairs small groups against all other robots'
reservations, rolls back unsuccessful bounded repairs, and completes the declared
iteration count. Candidate initialization can rank cumulative progress throughout
the window; retained and fresh seeds compete before repair. This is distinct from
CGAR's newly tested pure terminal chain potential, which doubles loaded waiting
on RANDOM-03 when enabled alone.

The crowded policy is different. `Engine::advance` executes an existing set of
forward/wait promises while deciding the following moves on exactly predicted
occupancy. Idle robots pre-rotate; moving robots obey a kinematic continuation
mask. It handles dead-end eviction and complete simultaneous chains/cycles.
`evaluate` compares roots under common future perturbations, explicitly checking
that continuations preserve the same first actions and promises. The score is
remaining-chain progress plus dispersion, not a larger collection of unrelated
one-step constructions. These distinctions matter for RANDOM-04/05.

PILOT uses 16 physical cores / 32 SMT workers in these selected records. CGAR's
current RANDOM records use four physical cores. More allocation is authorized
when fixed-work experiments actually need it; timeouts must still fail, and no
partial best-so-far fallback is introduced.

## Bounded next implementation

Start with a general rolling-window improvement layer **seeded by CGAR's complete
validated plan**, initially for RANDOM-01/02/03. Keep the selected CGAR controls
and all instance fields separately reproducible. The existing complete-chain
oracle supplies oriented stage costs on small maps within its explicit memory
budget; do not silently allocate quadratic tables on large maps.

1. Convert the selected five-action plan to full state trajectories. Reconcile
   actual wait-seed rotations, then extend safely with waits to the declared
   window. Check every vertex, head-on edge, heading and service transition.
2. Preserve CGAR's protected first actions and their supporting moves. Recovery
   witnesses, pocket permissions, parked robots and protected target ownership
   remain authoritative. A projected future path must not claim to simulate an
   unknown future primary/recovery decision. Conservative protected tails are an
   explicit approximation to measure, not a liveness proof for a new kernel.
3. Try bounded time-space A* repairs of small ordinary-robot groups, with complete
   rollback when a group cannot be repaired within its fixed node allowance.
   Compare paid action cost plus terminal chain potential; keep the existing
   five-step objective as a separate control. Use a fixed number of completed
   repair attempts and propagate deadline errors.
4. Reconcile the returned first actions with CGAR state, commitment and movement
   counters. Retained history must be validated against actual executed states,
   task identities/stages and current protections; reset conflicts explicitly.
5. Test small instances against independent action/service search, protected
   motion conflicts, repeated stops, task swaps, serial/parallel identity,
   disabled trajectory identity and failure after a visited improvement. Run
   full strict one-second comparisons before recording a frontier improvement.

This is a proposal for implementation, not an implemented or measured improvement.
The crowded pipeline/continuation transfer remains a subsequent, separate change.
A useful read-only diagnostic first measures how often a five-step plan advertises
a forward move next step, then replans another wait under an unchanged task and
compatible protections. That can distinguish repeated deferral from genuinely
blocked traffic without assuming static-potential correctness implies progress.

The exact full-chain transfer is already implemented: RANDOM-05 score-only improves
2,956 to 3,027 on seed 0, while changing its priority order loses. RANDOM-01 can
reach 647, but its new three-seed mean is unchanged. RANDOM-02/03/04 regress.
Do not enable the new chain score globally or merge its ordering effect into a
claimed general throughput improvement.
