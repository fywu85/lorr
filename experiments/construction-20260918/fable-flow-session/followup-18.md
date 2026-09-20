# Diagnosis and next experiments after the plateau

Continue our SAME persistent session, Fable 5.1 at max effort. Read-only review;
no tests have been delegated to you. The user explicitly requests: identify the
major throughput bottleneck, compare CGAR with NMS/KittyKnight, search broadly in
the literature, then consolidate with you to design the next steps. I performed
the source inspection and primary-source search below. Challenge my priorities
and causal claims. Do not simply endorse another parameter sweep.

Constraints: warehouse focus, but generic graph algorithms only, no map IDs,
templates, prelearned warehouse edge files, lane/parity rules or fleet disabling.
10,000 robots, all 5,000 steps; complete scheduler+planner <=1 second or explicit
failure, fixed prescribed work (no clock-selected partial incumbent), RSS <32
decimal GB, isolated GRID physical cores. All revealed task stops are legal;
hidden future tasks are not. Current reference uses four physical cores.

## New verified accounting, CURRENT four-region reference, all six seeds

Tasks: 144510/143933/144107/143134/143934/144029, mean143941.1667. Local KK152981,
requiring +6.2802%. KK used 38.858GB and 16 physical/32 logical cores; it is not
an equal-resource or official score comparison. Best single remains144510.

Six independent GRID replays of the current raw trajectories: SHA agrees with
previous independent audit, 300M action counts exactly match production, four
hand-counted boundary/action fixtures pass. Exact work identity below, units
robot steps per completed task; CGAR pooled over all six, KK one run:

| Component | CGAR | KK | Difference |
|---|---:|---:|---:|
| Shortest spatial chain of completed tasks |241.86094|233.83571|+8.02523|
| Completed loaded forward detours |7.68612|4.59356|+3.09255|
| Completed loaded turns |9.74660|5.67848|+4.06812|
| Completed loaded waits |13.55233|8.41068|+5.14165|
| ALL empty time incl. unfinished and reassignment |64.29481|65.69141|-1.39660|
| Loaded time on unfinished tasks |10.22335|8.62814|+1.59521|
| Idle |0|0|0|
| Total |347.36414|326.83797|+20.52617|

Interpretation: loaded motion overhead difference12.3023, completed task-cohort
chain difference8.0252; empty travel already slightly better in CGAR. These are
observational accounting differences, NOT independent recoverable savings.
Turns/waits/detours include necessary work; longer completed chains can reflect
assignment, completion ordering, censoring. No causal attribution to HRRN yet.
Steady final1000 tasks29753..30004, no late collapse. Direct protected/recovery
robot-step counts tiny, but they do not bound indirect fairness effects. No idle,
no safety-cancel or blocked-forward entries. All38586 free cells are certified
core, no pocket-domain issue on this map.

V50 duplicate deadline callback optimization now full-verified seeds0/2:
EXACT existing trajectories144510/144107; all10000 complete entries valid,
max.9192/.8867s, means406.12/384.13ms, RSS11.337GB, CPU1.72/1.77 cores of4,
33.1..34.9min. Full timings are different allocations, not causal speed ratios.
No throughput gain. Same-core200-step A/B/A was~1.8% wall reduction.

## Actual active competitor code, not inactive snapshots

KK active CMake compiles wppl_planner (NOT my_planner). Archive da7d428.
warehouse_large.json: LNS, LaCAM2 initializer, PP repair with SIPP, adaptive
destroy, group8, CT/CAT/PATH windows10, execution3; max_agents=-1 and corners
enabled. It loads a warehouse-specific weight file; borrowing that is excluded.
The initializer has consider_rotation=false but oriented heuristics; do not
infer the complete planner ignores rotation from that initializer flag.

KK NeighborGenerator::generateNeighborByRandomWalk starts with delayed agent,
walks a cheaper candidate route and collects agents whose reservations obstruct
it; other destroy operators use intersections/random agents. Adaptive weights
use improvement. LocalOptimizer::runPP shuffles chosen robots, removes/replans
their paths against other reservations with orientation-aware single-agent
space-time search; incomplete or worse neighborhood is restored. This is more
targeted than CGAR four large geometric regions with five-slot temporal repair.
There are caveats in their weighted-cost random-walk comments; transfer mechanism,
not code wholesale.

KK scheduler.cpp:810 greedy_matching_warehouse reconsiders all unopened tasks,
including assigned but not picked up. It groups tasks by first endpoint, sorts
task chain cost and robot pickup cost per endpoint, globally selects cheapest
pair. All robot/endpoint distances have weighted orientation-aware pickup costs;
task chain uses weighted endpoint-to-endpoint heuristic. Generic grouping could
use currently revealed first-stop cells instead of supplied map_stats endpoints.
Exact factorization applies to additive costs, not automatically age/(pickup+
chain) HRRN. KK88043 reassignments but its ALL empty time/task is higher; its
last-assignment pickup7.26 vs first53.0 erases earlier travel and is misleading
as a throughput explanation.

NMS archive44950d1: enabled PIBTS multi-action+annealing+graph guidance and task
reassignment;32 threads, PIBTS_STEPS=-1 time-selected, scheduler rebuild350ms,
trivial solve150ms, scheduler LNS_TIME=0. rebuild_dp prices every free task for
each refreshed robot, prioritizes stale robot lists. Warehouse get_dist uses
5*oriented weighted pickup+weighted chain. All unopened tasks can be rematched.
Motion uses32 independent worker copies and selects best score. CGAR already
borrowed much of temporal multi-action/repair; more workers mixed six seeds.
Cannot copy their elapsed-time work or full table memory uncritically.

## Negative evidence we must respect

- Four vs16 global workers mixed six seeds. Eight geometric regions lose both
  full seeds; zero-temperature loses both; S513 fails deadline seed0 and loses
  seed2; 3Mglobal/4regionalrounds loses both;2M version timeout. No more such sweep.
- Warm shifted plans/mixed starts, next-errand score, fractional turn extras lose.
- Expanded pickup quota128/256 reproduces exact64 trajectories: no gain.
- Removing age HRRN and pickup weights3/5 regresses/collapses. Existing pool
  matching/pair swaps were mixed or negative six seeds.
- V35 separate one/two-owner transactional pass: first pair+0.339%, then six-seed
  mean134202.5 vs134590.3 (-.288%,3win3lose). Bounded8robots,256roots,
  2048candidates/root,65536global; accept complete scalar improvement. This is
  not full MD-PIBT dependency-graph/backtracking, but multiowner is NOT untried.
  Old snapshot~171..175 stationary robots had2owner easiestforward candidate;
  current reference opportunity has NOT been re-audited.
- Intended-route guidance WAS extensively tested: oriented A*, remaining route
  flow counts, opposing/load penalties, complete local route-to-go potential,
  corrected beyond-waypoint bug, bounded reconnection/refinement. On old109244
  baseline, unit39066/opposing42051/reconnect28071/refine33507, with valid full
  runs but congestion collapse. A new soft local guide differs from hard
  route-to-go preference, but needs a specific mechanism and gate to retry.

## Primary literature search (read primary papers/reports, not search snippets)

1. PIBT https://arxiv.org/abs/1901.11282 ; RHCR https://arxiv.org/abs/2005.07371 ;
   MAPF-LNS https://www.ijcai.org/proceedings/2021/568 : foundation, rolling
   collision windows and selected small-group replanning. Already partly used.
2. WPPL, SoCS2024 https://arxiv.org/abs/2404.16162 : PIBT + parallel window LNS,
   realistic rotations/large fleets, congestion and myopia. Strongest direct
   motion-transfer candidate, not proof of gain in our scaffold.
3. Guided-PIBT/traffic flow AAAI2024 https://arxiv.org/html/2308.11234v2 :
   planned-route forward/reverse counts, vertex load, lexicographic contraflow
   first then length/load, continuous route refinement. Classical cardinal
   moves, external goals. Our failed route guide is related but not identical.
4. GGO IJCAI2024 https://arxiv.org/abs/2402.01446 ; OnlineGGO AAAI2025
   https://arxiv.org/html/2411.16506v1 : throughput-optimized offline/online
   edge-weight learning. Generic online updater conceivable; map-trained weights
   not allowed; training/rotation/resource/generalization transfer unproven.
5. Lightweight Effective Preference Construction in PIBT,2025
   https://arxiv.org/html/2505.12623v1 : lexicographic distance,hindrance,regret,
   random. Hindrance measures obstruction of neighbors' next progress; regret
   updates from forced alternative choices over repeated passes. Classical
   nonoriented model; cheap local externality heuristic, no liveness guarantee
   transfers automatically. Need full primary-score tie coverage audit; our
   tiny seed-rotation-tie counter does NOT count all candidate ties.
6. EPIBT AAAI2026 https://arxiv.org/html/2511.09193v1 ; actual2024 NMS report
   https://expo25.leagueofrobotrunners.org/resources/No-Man-Sky.pdf :
   multi-action operation chains, revisits/suffix inheritance, single blocker,
   randomized repair. Most backbone already transferred. Later paper uses
   externally assigned goals and different timeout treatment.
7. MD-PIBT preprint2026 https://arxiv.org/html/2603.23405v1 : dependency graph,
   multiple owners, backtrack to most recent parent and clear descendants;
   capsC/R. Rotational homogeneous and heterogeneous settings. More structural
   than our failed extra pass, but diagnosis before expensive port.
8. Lifelong LaCAM with Local Guidance (LLLG),SoCS2026
   https://arxiv.org/html/2605.16855v1 : previous-solution suffix initializes
   soft space-time guidance, local A* refinement penalizes conflicts. Distinct
   from reusing feasible warm action seeds (which failed here). Better internal
   LaCAM*/LNS scores sometimes lower lifelong throughput. Classical nonoriented,
   most tests500steps/10s; separate10k lightweightcase. Supports objective issue,
   not a directly comparable throughput promise.
9. SILLM ICRA2025 https://arxiv.org/html/2410.21415v2 : imitation of WPPL,
   learned preferences/global guidance and PIBT conflict shield. They removed
   rotations from WPPL comparison, used GPU inference/training. Long-term only.
10. Verstand2024 report2025
   https://expo25.leagueofrobotrunners.org/resources/Verstand.pdf : same LoRR
   multi-goal/rotation setting, add measured congestion delay to task cost,
   estimate coefficient from past completed duration residual. Circle/intersection
   and grid-square density proxies, same planner controlled. Generic congestion
   feature possible; pickup-only penalty less supported by our loaded-cost gap,
   coarse squares ignore connectivity, completed-task feedback selection-biased.
11. Combined Online Task Assignment and Lifelong Path Finding: Rule-Based Systems
   Matter https://arxiv.org/html/2502.07332v2 : assignment and routing coupled;
   their left/right port rules are map-specific and NOT proposed for us.
12. MixedGGO2026 https://arxiv.org/abs/2602.23468 : soft/hardedge directions,
   hard restrictions risk CGAR certificate; defer. LSMART2026 and GreedyPIBT2026
   screened, no sufficiently direct transfer beyond above; full latter unavailable.

## Tentative next steps; please reprioritize or replace

A. Keep the assignment-price draft as a small independently interpretable test:
   OFF vs covered+ratio-imputed vs ratio-only placebo (your turn18). Offline
   3429completed/3840assigned sample, heldout-seed MSE -5.47%, allfolds; predictive
   not throughput. Need resident coverage/publication-phase/nativeBFS-vs-Manhattan
   basis/shadow ranking diagnostics. Entire draft saved as patch and production
   restored exactV50; no new binary yet. Not expected to explain entiregap.
B. Main motion experiment: conflict-connected bounded small groups, initially
   SAME five-step horizon and scalar score to isolate group selection/search,
   using complete transaction and protected paths unchanged. Compare to equal
   fixed-work random groups or geometric repair, before any horizon expansion.
   Only if useful, test ten-step orientation-aware space-time/SIPP local planning
   with a valid complete outside-reservation extension and fixed expansions.
   Important: our current candidate/reservation representation is five slots;
   merely planning selected robots for10 while others stop at5 is INVALID.
C. Cheap local hindrance secondary ranking: first audit tied/near-tied real
   candidate opportunities; use deterministic local neighbor externality, preserve
   protected actions and validate all plans. Distance/score units and operation
   ID preferences complicate exact ties. Regret repetitions are a separate
   variant, not a free addition to tie-breaking.
D. Refresh actual multi-blocker rejection audit under current reference; only
   fund MD-PIBT integration if recurrent avoidable coordinated conflicts remain
   and small complete counterfactual repair succeeds. Old extra-pass negative
   result has to be in acceptance review.
E. Longer-term online congestion costs learned from observed delay/reservations,
   or soft short-term guidance, rather than repeating failed persistent route
   potentials. Need causal/shadow evidence before training or broad integration.

Question: What are the best THREE next experiments, in order, with a falsifiable
mechanism and minimum discriminating measurement? Is conflict-connected LNS
actually meaningfully different from our strong regional repair, or does it
mainly buy repeated root orders that already failed? Is assignment pricing too
weak to deserve the first full benchmark? Where do you think the largest
addressable gap is, and what observation would change your mind? Call out any
mistaken inference in this synopsis. Distinguish proof, measurement and opinion.
Please keep final review concise (~1000 words); no unperformed-test claims.
