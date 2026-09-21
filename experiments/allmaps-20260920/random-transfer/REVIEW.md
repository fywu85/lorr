# Transfer from the independent RANDOM-05 solver into CGAR

The user requested this focused study on2026-09-20, after making throughput the
primary metric. Read-only reference: source233f5bf, the independently replayed
3872-task standalone result. [Source fingerprints](source-review.json).
The other agent's working tree, tests, builds and jobs remain untouched. New
CGAR experiments may evaluate the archived RANDOM-05 input in separate CGAR
run directories; they are not changes to the standalone solver.

## What the code actually does

| Mechanism | Frozen RANDOM-05 implementation | CGAR difference and transfer |
|---|---|---|
| Exact remaining task cost | `Chain::Chain/cost`, engine.cpp564-602: backward dynamic programming over all four arrival headings at every remaining waypoint | CGAR's main temporal score targets the current waypoint; remaining-chain order adds spatial tail distance. The existing next-errand option is not this full chain metric. A chain-conditioned oriented oracle is a substantial general candidate. |
| Motion pipeline | `Engine::advance`,803-1144: execute prior forward promises, choose the following movement on exact predicted occupancy, pre-rotate idle robots; a moving robot can only continue straight in the next promised move | CGAR searches a five-step reservation footprint each call and usually discards its ordinary suffix. A pipeline proposal must be reconciled with CGAR primary/recovery paths, not substituted as a standalone solver. |
| Priority search | `compute`,1482-1633: incumbent priority offsets, mutations, one-quarter global restarts, multiple elite parents and retained vectors | Existing CGAR workers share a construction order; repair random streams differ. First transfer: independently perturb construction order, optionally retain the winning offsets, then run the unchanged CGAR kernel. |
| Multiple continuations | `rollout/evaluate`,1182-1347: several depth8 futures share the exact first actions and promises; average their progress scores | CGAR chooses workers using one five-step score. A later transfer can evaluate candidates under common future perturbations. It needs a faithful task/heading simulator and fixed complete work, not a renamed larger worker count. |
| Assignment | `match`,631-732: jointly match idle and unopened-task robots to all eligible tasks, exact Hungarian below a declared size; guided pickup cost plus0.25chain length and small keep bonus | CGAR's later matching is bounded local unopened-task permutation, with cooldown and one-retarget limit. The present generic matching-factor test is a first measurement; full-pool joint assignment remains a separate proposed change. |
| Cycles and dead ends | `advance`: evacuating pocket priority, optimistic spatial intent for turns, and profitable completely occupied rectangles move as one cycle | CGAR has certificates, protected recovery and temporal displacement. Test cycle proposals within those constraints if motion diagnostics justify them. Do not remove protections just to resemble the reference. |
| Traffic field | `Graph`,324-455: all-pairs demand assignment, stronger opposing-flow prices, then soft directional edges; the selected field/flip is tuned to this map | CGAR currently learns flow from early trajectories. The chosen RANDOM-05 field is a trick; any field transfer must be behind `--trick RANDOM-04`/`RANDOM-05` and independently fingerprinted. |
| Known horizon | `compute`: suppress goals unlikely to finish before the declared end, while retaining started-task ownership | This is a trick, not a generic scheduling guarantee. Keep it separate from pipeline/priority/chain improvements. |

The standalone result without horizon triage is3503tasks on four cores; its
field remains tuned. Therefore cutoff alone cannot explain its strength. The
standalone3770four-core candidate also beat matched NMS on two frozen fresh
inputs by25.42%aggregate. Those results establish useful ideas to study; they
are not CGAR results or matched evidence for a CGAR transfer.

Recent standalone work also supplies negative controls: operation-sequence
prototypes remained weaker than its pipeline; component recombination, added
completion rewards, and age retention lost in the recorded full cases. Do not
assume more search, stronger fairness, or more elaborate scoring improves tasks.

## First bounded transfer: persistent construction priorities

`CGAR_TEMPORAL_PRIORITY_NOISE` defaults0. Positive noise perturbs the existing
current-goal/remaining-chain distance order in physical-forward-cost units.
This adapts the reference's priority-search structure; it does not copy its
age-based PIBT policy. Candidate scoring/power stay identical across workers.
Worker0 re-evaluates the parent unchanged, every fourth alternative restarts,
and other alternatives mutate30%of entries by default. Optional persistence
retains the selected complete search's offsets for the next consecutive step.
No score, plan or assignment is reused without evaluating the current state.

All candidates still use CGAR's temporal reservations, fixed primary and recovery
seeds, collision checks and prescribed work. A deadline is an error, never a
partial portfolio. Default0 must preserve original actions and random streams.
This is a general opt-in mechanism with no filename or map-specific branch.

The verified remaining-chain rank factor already scores1249on RANDOM-04 versus
842for the same bounded-work control, on one full1000-step seed0run. The
older uncapped profile scored997with much longer steps; NMS's published target
is2547. This is progress, not a claim that the dense gap is closed.

Predeclare new portfolio comparisons before running: chain-rank one-worker
control (4Mglobal candidate cap), eight-worker control (0.5Mper worker), and
8worker noise50 cold, noise50 persistent, noise200 persistent. Keep4regions x
4Mcandidates x2rounds, the scheduler and all other options fixed. Eight-worker
construction can overshoot per-worker caps, so compare measured candidate work
and latency rather than calling nominal4M an exact equal-work guarantee.
Full RANDOM-04 and RANDOM-05 horizons determine scores;4physical cores per case,
32decimalGB and5sdevelopment limit. Recheck promising settings at1s and on fresh
planner seeds. Fairness tails are reported without vetoing higher throughput.

## Controlled scoring checks alongside the new portfolio

The frozen RANDOM-05 rollout score is remaining-chain potential decrease; it
does not add a paid-edge surcharge to that final score. Its policy still ranks
moves by edge cost plus future cost. To isolate whether the distinction matters
in CGAR, test the existing generic `CGAR_TEMPORAL_REMAINING_FLOW=1` separately
from the chain-rank control. This removes the extra paid forward penalty in
CGAR's temporal score while retaining the learned field and motion kernel.

A separate existing `CGAR_TEMPORAL_NEXT_ERRAND=1` test uses the next visible
errand when its cached oriented table is available. This is a partial chain
continuation check, not the exact whole-chain dynamic program described above.
These selectors are mutually exclusive and tested in separate profiles. Same
full RANDOM-04/05 inputs, seed0, single worker4Mglobal and4Mregional caps,
four physical cores,5sdevelopment deadline. No map tricks or horizon knowledge.


## Completed evidence and next structural diagnostic

All 18 strict one-second portfolio replications passed. Eight candidates improve
both maps on all three planner seeds; noise50 consistently helps RANDOM-05 over
the eight-candidate control, but is mixed on RANDOM-04. Persistent vectors and
both simpler objective changes were weaker. See the linked campaign result
matrices rather than inferring success from the standalone mechanism alone.

A read-only certificate/event audit found an additional barrier on RANDOM-05:
819 free cells, certified core788, capacity787,31 initially parked robots. Its
capacity policy permanently excludes any chain with an off-core stop. In the
2036-task seed0 run,427 of431 never-assigned tasks are excluded by this rule;
none of these427 was assigned or completed. The other773 outstanding tasks are
core-eligible. On RANDOM-04 the capacity filter is inactive:124 off-core tasks
were completed. [Audit](capacity-audit-v1/audit.json). This is direct structural
evidence, not a counterfactual throughput estimate.

`capacity-variants.json` declares a diagnostic comparison using the existing
`CGAR_CERT=0` switch against the noise50 control: same two archived inputs,
planner seed0, full horizons, four physical cores, shared EPYC9354, enforced1s.
This ablates pocket certificates and capacity filtering. It retains the CGAR
primary/recovery and temporal machinery, but **does not preserve the original
certificate's liveness premise**. It is a diagnostic ablation, not an automatic
production change or a claim that ignoring dead ends is safe for progress.
The standalone solver explicitly handles pocket evacuation, so success there
would motivate a compatible CGAR mechanism; simply toggling the switch is not
that transfer. Keep all failures and stalls. No map-specific tuning is added.


The complete certificate-off diagnostic lost on both maps:1306->1274on
RANDOM-04 and2036->1953on RANDOM-05, all four full cases valid under1s. Thus the
admission restriction is real, but removing the mechanism does not improve
throughput on this control. Keep certificates enabled. Any future pocket-service
transfer needs explicit coordinated evacuation rather than assuming a disabled
certificate solves the bottleneck. [Full results](../results/dense-capacity-full-v1/summary.md).
