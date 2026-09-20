# Warehouse bottleneck and research review

Prepared 2026-09-20 UTC (2026-09-19 evening in New York). This review answers the
request to diagnose throughput, inspect NMS/KittyKnight, search the literature,
and consolidate the next experiments with the persistent Fable session.

The recent throughput plateau is real. The confirmed six-seed mean remains
**143,941.2**, best single **144,510**, versus the local KittyKnight run's
**152,981**. Matching that local number requires **6.28% more completions**.
This pass establishes a better diagnosis and experiment plan, not a new score.
The strongest measured differences are loaded movement overhead and the mix of
tasks completed. More geometric regions, search rounds, or scalar-score weight
have not resolved them.

KittyKnight's reference used 38.858 GB RSS and 16 physical / 32 logical cores.
CGAR's reference reserves four physical cores and enforces 32,000,000,000 bytes
RSS. NMS's saved full run reports154,981 completions but one entry timeout and
about39.744GB RSS, so it is not an admissible comparator under our contract.
[Full competitor records](../../sequences-20260918/results/leaders-full/results.json). This is a local target, not an official or equal-resource SoTA comparison.
The general-purpose algorithm constraint remains: no map IDs, supplied warehouse
edge weights, lane templates, fleet caps, or hidden future tasks.

## 1. Where the robot time goes

Six new GRID replays cover **300 million robot steps** of the CURRENT four-region
reference, rather than the older no-region policy. All six trajectory hashes
agree with the prior independent replay inputs; every phase/action count matches
production; four hand-counted fixtures pass. Each run uses 10,000 robots and all
5,000 steps. [Accounting and provenance](../results/current-bottleneck-audit-v50/accounting.json),
[reproducible accounting script](account.py).

The following identity includes unfinished work and all empty travel, including
travel before task reassignment. CGAR is pooled over six seeds; KittyKnight is
one run. Units are **robot steps per completed task**.

| Component | CGAR | KittyKnight | CGAR minus KK |
|---|---:|---:|---:|
| Shortest spatial chain of completed tasks | 241.861 | 233.836 | +8.025 |
| Forward travel beyond that chain | 7.686 | 4.594 | +3.093 |
| Turns on completed loaded tasks | 9.747 | 5.678 | +4.068 |
| Waits on completed loaded tasks | 13.552 | 8.411 | +5.142 |
| All empty travel/turn/wait time | 64.295 | 65.691 | **−1.397** |
| Loaded time on unfinished tasks | 10.223 | 8.628 | +1.595 |
| Unassigned robot time | 0 | 0 | 0 |
| **Total** | **347.364** | **326.838** | **+20.526** |

The loaded detour/turn/wait difference is **12.302 steps per completion**.
That is the largest combined behavioral difference to investigate. The completed
task-chain difference is another **8.025 steps**. There is no idle fleet capacity
to recover, and empty work per completion is already slightly lower in CGAR.
Increasing pickup table quotas previously produced identical trajectories.
Optimizing pickup distance alone is therefore not the best-supported next move.

These differences are **not causal savings or independent improvement budgets**.
Turns can be necessary; waiting can prevent collisions; detours can improve
traffic elsewhere. The completed cohorts differ. Longer chains may result from
assignment, completion ordering, or censoring at step 5,000. The accounting does
not prove that HRRN selects the wrong tasks or that eliminating 12.3 steps is
feasible. A fixed-goal motion experiment and an assignment experiment must be
interpreted separately before testing their combination.

CGAR spends about 81.5% of all robot steps carrying tasks. Its final 1,000-step
windows complete 29,753–30,004 tasks, so this reference shows no late collapse.
Direct primary/recovery intervention counts are small, but do not bound their
indirect effects on the fleet. All 38,586 free cells belong to the certified core;
expanding the orientation oracle's pocket domain cannot help this instance.

### Compute bottlenecks are a separate issue

V50 removes one duplicate root deadline callback without changing prescribed
search work. Full seeds 0/2 now reproduce **144,510 / 144,107**, their complete
trajectory hashes, and every sampled global/regional/rotation/orientation search
counter exactly. All 10,000 complete schedule+plan entries meet one second;
22 source/test hashes, binary, CPU bindings and RSS are verified.
[Full checks](../results/deadline-duplicate-full-v50/verification.json),
[reproduction](verify_v50.py),
[exact V50 source](https://github.com/fywu85/lorr/commit/cb6a666da8aee461d5a34cab01d7b7ccaa76a201).

Those runs average **384.1–406.1 ms per step**, reach **919.2 ms** maximum,
use **1.72–1.77 CPU cores on average** of four reserved, peak at **11.337 GB RSS**,
and take **33.1–34.9 minutes**. The full runs use different allocations from the
older reference and are not a controlled speedup estimate. The prior same-core
200-step A/B/A measured about 1.8% wall reduction. Neither result is a throughput
gain. Faster fixed-work execution creates deadline margin; it does not change
robot decisions by itself. Additional cores are useful only for a demonstrated
parallel algorithm or independent benchmark cases.

## 2. What is actually different in NMS and KittyKnight

Source inspection follows the compiled entries. KittyKnight's active tree is
**`wppl_planner`**, not its separate `my_planner` directory. The repository holds
KK archive `da7d428b40cc1425b381adefe9709d4accbfdfe1` and NMS archive
`44950d1b61b7482b4529a9dc127fa9d220962bdc`.

| Mechanism | Current CGAR | Active competitor implementation | Transfer decision |
|---|---|---|---|
| Motion construction | Five-action temporal candidates; recursive displacement accepts one distinct blocking owner | NMS multi-action PIBT, randomized repair and independent workers | Much is already borrowed. More workers were mixed across six seeds. |
| Repair neighborhood | Four large geometric regions, frozen cross-boundary reservations, two rounds | KK chooses small groups (configured size 8), including robots obstructing cheaper paths of delayed robots; other operators use intersections/random robots | **Promising structural difference:** release/replan interacting groups transactionally, first at the same horizon and score. |
| Lookahead | Five slots | KK configures ten-step path/constraint windows and execution window 3, with LaCAM2 initialization and PP/SIPP repair | Potentially useful after isolating neighborhood effects. Ten-step local planning requires valid reservations for all other robots through step 10. |
| Assignment coverage | Sparse candidate discovery plus 64 complete pickup fields; age-sensitive whole-chain ranking | NMS refreshes all-task lists for selected robots; KK matches all available robot/pickup-endpoint combinations | Broader candidate coverage is different from more copies of already-complete fields. Measure omitted better candidates before porting a larger matcher. |
| Chain cost | Cached spatial BFS, with Manhattan fallback when unavailable | Competitors use their weighted task-chain metrics as well as weighted pickup distances | Test a consistent oriented/traffic chain price, with coverage and mean-scale controls. |
| Unopened tasks | Existing assignment generally retained; optional pool/swap variants tested previously | Both reconsider tasks assigned but not yet picked up | Broad rematching differs from pair swaps, but previous negative CGAR results and KK's higher empty work argue against copying it blindly. |
| Guidance | Generic published observed-flow tolls and bounded complete orientation tables | KK loads a supplied warehouse weight file; NMS uses graph guidance and large heuristic structures | Borrow generic online mechanisms only. Do not import warehouse-specific weights or exceed the memory cap. |
| Work selection | Prescribed counts/work, explicit timeout failure | Competitors use elapsed-time search budgets; NMS configures 32 threads | Any transfer must use explicit fixed work and full-decision validation. |

Immutable source links: [KK active build](https://github.com/fywu85/lorr/blob/86738330aec29986d1863486505d6a1aba5a81da/Code-Archive/2024%20Competition/Team_Kitty_Knight/da7d428b40cc1425b381adefe9709d4accbfdfe1/CMakeLists.txt#L54), [KK configuration](https://github.com/fywu85/lorr/blob/86738330aec29986d1863486505d6a1aba5a81da/Code-Archive/2024%20Competition/Team_Kitty_Knight/da7d428b40cc1425b381adefe9709d4accbfdfe1/wppl_planner/configs/warehouse_large.json#L1), [KK neighborhood selection](https://github.com/fywu85/lorr/blob/86738330aec29986d1863486505d6a1aba5a81da/Code-Archive/2024%20Competition/Team_Kitty_Knight/da7d428b40cc1425b381adefe9709d4accbfdfe1/wppl_planner/src/LNS/Parallel/NeighborGenerator.cpp#L196), [KK group replanning](https://github.com/fywu85/lorr/blob/86738330aec29986d1863486505d6a1aba5a81da/Code-Archive/2024%20Competition/Team_Kitty_Knight/da7d428b40cc1425b381adefe9709d4accbfdfe1/wppl_planner/src/LNS/Parallel/LocalOptimizer.cpp#L95), [KK assignment](https://github.com/fywu85/lorr/blob/86738330aec29986d1863486505d6a1aba5a81da/Code-Archive/2024%20Competition/Team_Kitty_Knight/da7d428b40cc1425b381adefe9709d4accbfdfe1/wppl_planner/src/scheduler.cpp#L810), [NMS assignment](https://github.com/fywu85/lorr/blob/86738330aec29986d1863486505d6a1aba5a81da/Code-Archive/2024%20Competition/Team_No_Man%27s_Sky/44950d1b61b7482b4529a9dc127fa9d220962bdc/Solution/Scheduler/scheduler_solver.cpp#L13), [NMS settings](https://github.com/fywu85/lorr/blob/86738330aec29986d1863486505d6a1aba5a81da/Code-Archive/2024%20Competition/Team_No_Man%27s_Sky/44950d1b61b7482b4529a9dc127fa9d220962bdc/Solution/settings.hpp#L20), [CGAR temporal constructor](https://github.com/fywu85/lorr/blob/86738330aec29986d1863486505d6a1aba5a81da/cgar/cgar_planner/temporal_pibt.hpp#L14).

Source anchors for reproducibility:

- KK `wppl_planner/configs/warehouse_large.json`: LNS/PP/SIPP, neighbor size 8,
  ten-step windows, map-weight path, all agents enabled. The LaCAM initializer's
  `consider_rotation=false` does **not** mean the whole planner ignores rotation.
- KK `wppl_planner/src/LNS/Parallel/NeighborGenerator.cpp:196,405`: delayed-agent
  selection and collection of reservation owners obstructing a cheaper walk.
  `LocalOptimizer.cpp:95` replans a shuffled selected group and accepts only a
  completed non-worsening repair. Transfer the mechanism, not unchecked code;
  the neighbor generator itself contains caveats about weighted costs.
- KK `wppl_planner/src/scheduler.cpp:810` and
  `WarehouseMatchingPriorityQueue.h`: group tasks by first endpoint, sort chain
  costs and robot pickup costs, and select pairs globally. Deriving endpoint
  groups from revealed tasks is generic; loading the supplied endpoint template
  is unnecessary. The additive factorization does not automatically apply to
  CGAR's age/(pickup+chain) ranking.
- NMS `Solution/Scheduler/scheduler_solver.cpp:13,53,185`: all-task candidate
  refresh, weighted pickup/chain cost, and unopened-task reassignment.
  `Solution/settings.hpp` enables multi-action PIBT, annealing, task reassignment
  and 32 threads; scheduler LNS time is zero in this snapshot.
- CGAR `cgar/cgar_planner/temporal_pibt.hpp` defines the five-slot reservation and
  single-owner recursive constructor; `temporal_regions.hpp` defines regional
  repair; `cgar.cpp:1808,2112` defines the native chain and sparse assignment.

KK reassigns 88,043 tasks in the saved run. Its short travel since the *last*
assignment omits earlier abandoned travel. The complete empty-work accounting
above is the appropriate comparison; reassignment count alone is not a benefit.
The competitor source differences are hypotheses about causes, not ablations of
the recorded performance gap.

A [new task-pool replay](TASK_POOL.md) now measures that cohort difference over
time as well as at the horizon. At step5,000, KK's unassigned tasks average527.64
cells and2,706.9 steps since reveal, versus306.20–307.31 cells and100.5–104.0
steps across the six CGAR seeds. KK still has1,642 unpicked tasks aged at least
4,000 steps; CGAR has none aged1,000 at any sampled checkpoint. This supports
persistent deferral of long tasks, while leaving the causal contribution to
throughput unknown. The KK target remains valid; its assignment waiting-time
tradeoff must be visible in any transfer. All seven original accounting reports
and prior raw hashes reproduce, with a hand-counted task-lifecycle snapshot test.

## 3. Broad primary-source literature review

Search date: 2026-09-19/20. This is a targeted broad survey across construction,
joint repair, routing guidance, assignment and learned preferences, not an
exhaustive systematic review. Papers after 2024 are possible new ideas, not claims
about what the 2024 competitors used. Published gains are not forecasts for CGAR.

| Primary source | Useful idea | Transfer limits and priority |
|---|---|---|
| [PIBT, 2019 preprint / AI 2022](https://arxiv.org/abs/1901.11282) | Priority inheritance and backtracking with structural progress conditions | Foundation already used. Classical move/wait assumptions do not automatically prove rotational CGAR liveness. |
| [RHCR, AAAI 2021](https://arxiv.org/abs/2005.07371) | Rolling collision windows and revisable plans | Supports a separate planning/execution horizon. Must budget whole decisions and validate a complete joint window. |
| [MAPF-LNS, IJCAI 2021](https://www.ijcai.org/proceedings/2021/568) | Improve a feasible solution by replanning selected robot subsets | **High priority:** interaction-based small groups; convert anytime search to bounded prescribed attempts/expansions. |
| [Traffic Flow Optimisation / Guided-PIBT, AAAI 2024](https://arxiv.org/html/2308.11234v2) | Planned-route load and opposing-flow costs; iterative route refinement | Related CGAR intended-route variants already failed badly. Its lexicographic flow objective differs, but repeating the same guide implementation is not a new experiment. Cardinal moves/external assignments differ. |
| [Guidance Graph Optimization, IJCAI 2024](https://arxiv.org/abs/2402.01446) | Optimize edge guidance for actual throughput | A generic updater is possible; importing a warehouse-trained edge file is outside scope. Training and validation must be separated. |
| [WPPL: Scaling Lifelong MAPF to More Realistic Settings, SoCS 2024](https://arxiv.org/abs/2404.16162) | PIBT initialization with parallel windowed LNS in large rotational settings | **Strong direct architectural lead** for small-group repair. Related to KK, but the paper is not a specification of the exact 2024 snapshot. |
| [OnlineGGO, AAAI 2025, v1 inspected](https://arxiv.org/html/2411.16506v1) | Adapt guidance to changing traffic with a learned update policy | Longer-term alternative to hand-set flow penalties. Training cost, unseen-map transfer and motion-model compatibility need evidence. |
| [Lightweight Effective Preference Construction in PIBT, 2025](https://arxiv.org/html/2505.12623v1) | Rank otherwise similar moves using obstruction of neighbors and regret from forced alternatives | **Cheap motion hypothesis:** audit real candidate opportunities first. Paper uses cardinal moves. Regret requires additional passes; it is not a free tie-breaker. |
| [EPIBT, 2025 preprint / AAAI 2026](https://arxiv.org/html/2511.09193v1), [NMS competition report, 2025](https://expo25.leagueofrobotrunners.org/resources/No-Man-Sky.pdf) | Multi-action operations, revisits/suffix inheritance, randomized repair | Much already transferred. The later paper's external task assignment and timeout treatment differ from this combined-track experiment. |
| [MD-PIBT, March 2026 preprint](https://arxiv.org/html/2603.23405v1) | A dependency graph supports multiple blocking robots and backtracking across parents | Plausible structural extension, including rotational settings. First refresh current blocker opportunities; our earlier two-owner extra pass lost on six seeds. |
| [Lifelong LaCAM with Local Guidance, SoCS 2026](https://arxiv.org/html/2605.16855v1) | Soft local space-time guidance initialized from previous solutions; stronger internal search scores can still hurt lifelong throughput | Distinct from our failed warm feasible-plan seeds and persistent route potentials. Most evaluations use different horizons/budgets and no rotations. Defer until a controlled obstruction test supports it. |
| [SILLM, ICRA 2025](https://arxiv.org/html/2410.21415v2) | Learn action preferences from a strong planner, then enforce safety with a PIBT variant | Longer-term. Their WPPL comparison removes rotations and learning/inference uses GPUs; not a direct CPU-only competition replacement. |
| [Verstand LoRR report, 2025](https://expo25.leagueofrobotrunners.org/resources/Verstand.pdf) | Estimate task delay from observed congestion and past completed-duration residuals | Same multi-goal rotational competition is relevant. Use generic connectivity-aware features; pickup-only density is less supported by our loaded-time gap. Completed-task feedback is selection-biased. |
| [Combined Online Task Assignment and Lifelong Path Finding: Rule-Based Systems Matter, 2025/26](https://arxiv.org/html/2502.07332v2) | Evaluate assignment jointly with a fixed planner and congestion feedback | Supports controlled scheduler/planner comparisons. Their left/right port rules and smaller, different task setting are not proposed transfers. |
| [Mixed Guidance Graph Optimization, 2026](https://arxiv.org/abs/2602.23468) | Combine soft costs with hard edge-direction restrictions | Defer hard restrictions: they can invalidate CGAR's progress certificate. |
| [LSMART, 2026](https://arxiv.org/abs/2602.15721) | Integrate assignment/planning with realistic execution uncertainty | Valuable for deployment; not the immediate deterministic LoRR bottleneck. |

[Greedy PIBT, TASE 2026](https://scholars.cityu.edu.hk/en/publications/greedy-priority-inheritance-with-backtracking-for-multi-agent-pat/)
was screened through its primary institutional abstract. Full text was not
reviewed, so its backflow/MILP variants are not ranked ahead of mechanisms with
inspectable evidence. Source retrieval did not include importing external code.

## 4. What the negative results rule out for the next iteration

These findings rule out repeating the same tests as if they were new. They do
not disprove every algorithm in the corresponding research family.

| Previous test | Evidence | Consequence |
|---|---|---|
| More regions, zero temperature, more regional rounds / less global work | Both full test seeds lose; some work/score variants timeout | Stop tuning these knobs without a new mechanism. |
| More independent global workers | Mixed six-seed results | Better internal best-of-worker score has not established repeatable throughput. |
| Larger complete pickup quotas | Identical full trajectories | Capacity alone is not the missing assignment improvement. |
| Warm/mixed seeds, revealed next-errand score, fractional turn extras | Both tested full seeds lose | Do not relabel these as new temporal guidance. |
| Pure cost assignment / larger pickup weight | Regressions or collapse | Preserve fairness while testing richer duration estimates. |
| Separate one/two-owner transaction pass | First pair +0.339%; six-seed mean −0.288%, three wins/three losses | Multi-owner is not untried. Full MD-PIBT is different, but needs a fresh opportunity audit. |
| Intended-route guidance with corrected route potential, reconnection/refinement | Old control 109,244; variants 39,066 / 42,051 / 28,071 / 33,507, all complete | Planned routes can concentrate traffic and reduce flexibility. A retry needs a specifically different soft-local mechanism and diagnostic. |

Details: [regional work](../REGIONAL_WORK_BALANCE.md),
[route guidance](../GUIDE_PATHS.md), [transactions](../TEMPORAL_TRANSACTIONS.md),
[earlier NMS transfers](../../throughput-20260918-next/NMS-transfer.md).

## 5. Experimental design before changing the reference

The immediate diagnostic should sample actual decisions of the current policy,
not a synthetic queue or the old 134k reference. Record candidate primary cost,
operation preference, selected first action, one/multiple blocking owners,
fixed/protected owners, and region-boundary membership. Use deterministic sampled
steps and roots; do not mutate RNG, caches, reservations or decisions. Separate
physical-potential ties from ties in the full scalar score, which also includes
an operation preference. Existing seed-rotation tie counters do not measure
this candidate space.

For conflict-connected repair, collect a small group around a delayed robot and
its actual obstructing reservation owners. Release all selected reservations in
a private transaction, retain every protected/outside reservation, and search
for a complete group replacement within a fixed node budget. First retain the
existing five-step horizon and objective, and compare with same-size random
groups at matched work. Merely changing root order in the existing single-owner
repair is not a new joint neighborhood solver. A ten-step version is a later
experiment and must define a complete valid extension of outside reservations;
an unspecified suffix beyond slot five is not safe evidence.

For task pricing, the held-out prediction audit supports a small controlled
test: 3,429 completed labels among 3,840 sampled assignments, leave-one-seed-out
MSE improvement 5.47%, all six folds positive. It does **not** establish better
assignments, production cache coverage, or throughput. Compare native cost,
covered oriented/traffic chain cost with imputed missing prices, and a
**ratio-only control** that changes the mean pickup/chain balance without adding
per-task oriented information. Report coverage by publication phase, spatial
BFS versus Manhattan basis, and how often rankings change. The draft is saved
as [an unapplied patch](assignment-price-draft-v51.patch),
with [status and limits](DRAFT_STATUS.md); production remains exact V50.

For local obstruction preferences, first count candidates tied on the intended
primary metric and measure whether their predicted effects on nearby robots
differ. Keep exact score-tie and physical-cost-tie variants separate. Preserve
protected progress decisions, use a deterministic fixed-work calculation, and
validate complete plans. If the proposed rule almost never changes an eligible
choice, do not fund a full parameter sweep. Regret-based repeated reconstruction
and soft space-time guidance remain separate experiments.

Run 200-step screens only for activation, deterministic OFF equivalence and
deadline/memory feasibility. Ranking requires full 5,000-step paired runs, first
seeds 0/2 and then all six before promotion. Predeclare the tested arms and report
every failure/regression, full totals, final-window counts, task-age tails,
movement accounting and resource use. These repeatedly used six seeds confirm
consistency within the benchmark suite; they are not an untouched statistical
test set. Reserve additional unseen seeds for any final SoTA/generalization claim.
Use independent isolated GRID allocations concurrently where available.

## 6. Consolidated decision after Fable's review

Fable5.1/max completed the requested review through Claude Code CLI at
2026-09-20T00:14:17.860547UTC, using the existing session
`1ebb1075-3538-49d1-93d1-a00c94fa256a`. No source reread was needed: the payload
contained the new findings and zero source deltas. It ran no tools or tests.
[Visible review](../fable-flow-session/turn19/review.md),
[local assessment and corrections](../fable-flow-session/turn19/assessment.md).

Its strongest challenge was that we should isolate the competitors' mechanisms
before committing to a planner rewrite. I accept that change in order. I do not
accept its claim that the completed-chain gap is known to be recoverable, or
that dividing overhead by chain length removes all task-cohort confounding.

| Order | Experiment | Discriminating result and next decision |
|---|---|---|
| **1: competitor diagnosis** | KK supplied/uniform guidance crossed with unopened-task rematching on/off, plus contemporary unchanged controls | Replay the same work accounting. If weights have a large effect, investigate generic online field quality; if rematching matters, prioritize the joint assignment experiment. Remaining differences still need controlled motion tests. |
| **2: assignment interaction** | Current CGAR versus additive ordinary assignment alone, bounded rematching alone, and their combination, with identical fair-admission/protected-task rules | A shorter completed chain helps only if added empty work, congestion and unfinished work do not consume the benefit. Reject collapse or persistent age-tail deterioration. Separate richer chain pricing and its ratio-only control from rematching before combining them. |
| **3: evidence-selected motion change** | Refresh current multiple-owner/region-boundary and candidate-preference audit; choose either a genuinely joint small-group solver, complete longer horizon, or delay-informed soft edge costs | Demonstrate eligible choices or complete counterfactual repairs under identical states/goals before the full policy benchmark. A null five-slot test does not reject ten-slot WPPL; predictable future conflicts alone do not prove a horizon limit. |

The first ablation is feasible without importing a new algorithm: KK's existing
`CONFIG_PATH` override selects all-one weights when each branch of the native
`map_weights_path` conditional array has an empty `value`. Retain the array schema;
a scalar empty string is invalid for the native configuration loader.
Disabling unopened rematching requires a small experiment-only scheduler patch;
keep existing unopened assignments intact and remove their robots/tasks from the
matching pool. Keep archived competitor sources unchanged and preserve frozen
patched-source/binary/config hashes. Verify unset/enabled reproduces native
code behavior and disabled preserves existing holders in deterministic fixtures; clock-limited full runs need repeated
controls, not an exact historical trajectory expectation.

Use the same input, task stream, planner configuration, CPU allocation and
resource reservation across KK arms. A four-arm factorial detects interactions;
separate two-arm comparisons alone would miss them. KK's shared weight table
also feeds its scheduler, and altered search costs change work completed within
its time budget. Thus uniform weights intervene on the whole KK policy, not
only motion guidance. Record search effort and task-cohort changes. These are
competitor diagnostics under their measured resources, never qualifying CGAR
scores or permission to exceed CGAR's 32GB/one-second contract.

The prior pool-exchange test used only32 exchanges per10ticks, demanded a pickup
improvement in addition to non-worsening total distance, and retained HRRN. It
therefore did not test whether broader unopened rematching can make short-chain
assignment work without its old empty-travel penalty. That interaction is a
reasonable new hypothesis. Historical cheaper pickups are only a diagnostic:
they omit the released task's later pickup and subsequent congestion, so they
cannot be treated as recoverable savings or a rigorous rejection bound.

The intended endpoint is a generic policy that improves all six full reference
comparisons without losing deadline, memory, collision validity or fairness.
The review leaves the best single and six-seed mean unchanged. No structural
planner or assignment change has been benchmarked or promoted in this pass.


### Diagnostic implementation update (2026-09-20 01:05 UTC)

The [frozen KK factorial](../kk_ablation/README.md) is built and running as
GRID array8899243.1–5, with held full analysis8899246. The four corrected200-step
screens pass; they are not performance rankings. Initial uniform startup failures
were a runner configuration-schema mistake and remain archived. The actual native
loader now verifies the corrected configuration. V2 fixtures additionally check
owner metadata, actual TaskManager lifecycle and synthetic nonuniform costs.

[Fable turn20](../fable-flow-session/turn20/assessment.md) prompted explicit
scheduler-time and LNS-remaining-budget observations because the native wrapper
subtracts scheduling time from the planner allowance. These diagnose, but do not
remove, changed clock-selected work. Current CGAR full seeds0/2 are also running
with the existing read-only blocker audit, jobs8899240/8899241; source remainsV50
and the audit must reproduce the confirmed reference trajectories exactly.
No new throughput record has been established.


### Current diagnostic results (2026-09-20 01:18 UTC)

The [full current-reference blocker audit](CURRENT_BLOCKERS.md) is complete and
strictly verified. Both exact reference trajectories and sampled search counters
reproduce; all10,000 complete entries meet1s and32GB. In the final window, only
2.8/3.3 stationary roots per snapshot have an unblocked first-forward candidate
with a full heuristic travel/turn improvement. Approximately323/332 instead meet
one movable owner,140/141 two owners,23/28 three or more. Stationary includes
useful rotations; owner movability is not proof of a feasible joint replacement.
Most conflict-free scalar-only improvements were operation preferences, so a
simple cheap-forward pass is weakly supported. Region-boundary effects and
complete same-state joint alternatives still require a separate probe.

The [task-pool age replay](TASK_POOL.md) is also complete. It supports persistent
long-task deferral in KK, including1,642 tasks still unpicked after at least4,000
steps, while preserving the distinction between observation and causal savings.
The five full KK factorial cases remain pending; their analysis will now include
these same pool snapshots (held job8899251 after accounting8899246). Fable turn21
is reviewing both new findings in the existing session. No new CGAR score.


### Consolidated decision after Fable turn21 (2026-09-20 01:24 UTC)

[The same-session review and local assessment](../fable-flow-session/turn21/assessment.md)
now cover the fresh blocker and task-pool evidence. Fable proposes task-centered
late binding: newly revealed free tasks attract nearby eligible unopened holders
when pickup-plus-chain cost improves. This differs from the old pickup-only,
robot-centered exchange pass. It remains conditional on the pending factorial.
For an interpretable first CGAR test, run ordinary scheduling first, preserve
finite-retarget and primary/recovery/fair/started-task protection, and compare
baseline/additive-only/retarget-only/both. Use consistent complete published cost
quotes, fixed work, coverage counters, whole-run empty accounting and age tails.
A local cost decrease across changing robot positions and metric publications
is not a proof that unlimited repeated retargeting preserves progress.

The alternative motion experiment is a complete same-state small-group
counterfactual, initially root plus one/two obstructing owners, with outsiders
fixed. A new read-only snapshot facility is needed: current artifacts do not
contain full candidate/reservation dumps. A complete witness identifies an
opportunity; a null within three robots does not rule out larger-group repair
or another horizon. Demonstrating production reachability requires faithful
replay of its ordering, power and rollback rules. These are concrete next
experiments, not a predicted6.28% gain. The five full KK cases are still running.


### NMS follow-up and track policy (2026-09-20 02:10 UTC)

The [new NMS source/replay pass](NMS_REVIEW.md) confirms that its active temporal
kernel is largely already transferred. The multi-owner routine is unused;
warehouse power is already equal. Its active guidance uses hard-coded lanes and
coordinate bands. Complete accounting measures16.755 loaded detour/turn/wait
steps per completion versus30.985 in current CGAR, while its task pool contains
1,562 unpicked tasks aged at least4,000. This is descriptive evidence from a
one-timeout/39.744GB run, not an admissible score or a causal attribution.
The earlier initial-idle residual error is explicitly corrected and preserved.

[Fable turn22](../fable-flow-session/turn22/assessment.md) prioritizes fresh-task
coverage and guidance-coherence measurements, with inference/design corrections.
The [complete small-group probe](GROUP_SNAPSHOTS.md) now passes regression,
unchanged-trajectory screens and independent exhaustive-enumeration fixtures.
Its early32groups establish no population or policy gain.

The user now permits map-specific tricks as a separate track when generic
improvement proves difficult, gated solely by `--trick <map-instance-name>`,
marked `[trick]` in commits and `TRICK` in logs. [Required track policy](../EXPERIMENT_TRACKS.md).
No trick implementation or new CGAR throughput record exists yet.
