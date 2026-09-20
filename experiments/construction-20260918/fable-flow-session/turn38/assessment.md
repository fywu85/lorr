# Persistent Fable turn38: assessment

Same authorized Claude Code session, Fable5.1/max, completed10:59:45UTC. Read-only
source review; it did not run tools or tests. Source delta covers V80 short guards/
tests and V81 backward Dial arithmetic. Prepared payload17,912bytes/twochangedsources.

No concrete correctness defect found. The relative index follows the current
integer distance, each positive edge is below the bucket count, and LIFO order/
deadline positions are preserved. It suggests a defensive scratch-size assertion.
Current compute is private: init, set_forward_costs and prefetch each allocate
max_edge_cost+1, with positive/capped costs already enforced. An undersized scratch
is not reachable through the current API; this is future hardening, not a discovered
runtime bug. Existing independent serial/parallel oracle coverage passes.

The requested additional closed-loop short+matching fixture is not a missing
integration check: V80 already completed four real200-step/10,000robot cases with
nativebands/matching64 xshortOFF/ON xweight1/5, with full simulator validation,
and separate independent protection tests. Existing small128tick episodes test
serial/parallel repeat service with matching OFF. We do not claim those episodes
cover matching ON, and do not rerun redundant fixtures without a new concern.

Prioritize a read-only budget-inclusive matching shadow if full native short
preference leaves an empty-travel deficit after shortening completed chains.
The review's claim that retarget capacity was the binding constraint is stronger
than measured evidence: prior counts show exclusion, not a validated budget2
throughput effect. A shadow should preserve protected/fair/cooldown rules, use
const peeks, own cursors, no commits, and deduplicate witnessed cycles with a
clearly declared task-level rule. Excluded guidance savings are neither physical
steps nor causal throughput. Calibrating them by an OFF/ON accounting difference
is at most a heuristic magnitude comparison on endogenous trajectories.

Native chain-price coverage/choice audit ranks second. Near-complete coverage and
near-uniform price ratios are conjectures; unopened-holder table coverage does
not establish whole-free-chain coverage. Generic resident pricing already lost.
A ratio can approximately rescale pickup weighting, subject to integer rounding,
clamping and the actual admission rule; task-specific differences need evidence.

Do not launch8M search just because compute is available. If later warranted,
sample objective gain against CANDIDATE evaluations at complete-attempt boundaries.
The current quota is4M candidate evaluations with a1M repair-attempt ceiling;
Fable's suggested1M–4M attempt curve confuses these units. Existing peak audit
records extrema, not the whole requested curve, so extra instrumentation would
be needed. Full score/chain/empty comparisons remain the priority.
